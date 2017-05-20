#include "accessmanager.h"


#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "ext/fmt/fmtlib.h"

#include "matrix/client.h"
#include "models/roommodel.h"
#include "models/roomlistmodel.h"
#include "models/memberlistmodel.h"


using namespace std::string_literals;
using namespace fmt::literals;
using json = nlohmann::json;
using error_signal = void(QNetworkReply::*)(QNetworkReply::NetworkError);


namespace
{


inline QNetworkRequest create_request(const std::string& url_base, std::string&& url)
{
  QNetworkRequest request{QUrl{QString::fromStdString(url_base + url)}};
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  return request;
}


}  // Anonymous namespace


matrix::AccessManager::AccessManager(std::string&& server, Client* client, RoomModel* room_model,
    RoomListModel* room_list_model, MemberListModel* member_list_model)
    : server_{std::move(server)}, url_base_{server_ + "/_matrix/client/r0"},
      network_{new QNetworkAccessManager{this}}, client_{client}, room_model_{room_model},
      room_list_model_{room_list_model}, member_list_model_{member_list_model}
{
  // Establish connection to reduce latency of first http request
  network_->connectToHost(QString::fromStdString(server_));

  connect(network_, &QNetworkAccessManager::finished, this, [this](auto* r){ r->deleteLater(); });
}


matrix::AccessManager::~AccessManager()
{
  if (!access_token_.empty())
    logout();
}


QNetworkReply* matrix::AccessManager::post(std::string&& url)
{
  return network_->post(create_request(url_base_, std::move(url)), QByteArray{});
}


QNetworkReply* matrix::AccessManager::post(std::string&& url, std::string&& data)
{
  return network_->post(create_request(url_base_, std::move(url)), QByteArray::fromStdString(data));
}


QNetworkReply* matrix::AccessManager::get(std::string&& url)
{
  return network_->get(create_request(url_base_, std::move(url)));
}


void matrix::AccessManager::login(const QString& id, const QString& pw)
{
  json data = {
    {"type", "m.login.password"},
    {"user", id.toStdString()},
    {"password", pw.toStdString()},
  };
  auto* reply = post("/login", data.dump());
  connect(reply, &QNetworkReply::readyRead, this, [this, reply]{ handle_login(reply); });
}


void matrix::AccessManager::logout()
{
  post("/logout");
}


void matrix::AccessManager::handle_login(QNetworkReply* reply)
{
  auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  auto response = static_cast<LoginResponse>(status_code);

  json data;
  if (reply->bytesAvailable() > 0ll)
    data = json::parse(reply->readAll().toStdString());

  switch (response) {
    case LoginResponse::Authenticated: {
      user_id_ = data.value("user_id", ""s);
      client_->set_user_id(std::string{user_id_});
      access_token_ = data.value("access_token", ""s);
      if (access_token_.empty()) {
        emit login_failure("Could not read access token");
      }
      else {
        emit login_success();
        init_sync();
      }
    }
    break;

    case LoginResponse::Invalid:
    case LoginResponse::Failed: {
      emit login_failure(QString::fromStdString(data.value("error", "Unknown error occured"s)));
    }
    break;

    case LoginResponse::RateLimited: {
      emit login_failure("Rate-limited, try again later");
    }
    break;

    default: emit login_failure("Received unknown status code");
  }
}


void matrix::AccessManager::init_sync()
{
  auto* reply = get("/sync?filter={1}&access_token={0}"_format(access_token_,
      R"({"room":{"timeline":{"limit":1}}})"));
  connect(reply, &QNetworkReply::readyRead, this, [this, reply]{ handle_sync(reply); });
}


void matrix::AccessManager::long_sync()
{
  auto* reply = get("/sync?since={}&timeout={}&access_token={}"_format(next_batch_, 30000,
      access_token_));
  connect(reply, &QNetworkReply::readyRead, this, [this, reply]{ handle_sync(reply); });
}


void matrix::AccessManager::handle_sync(QNetworkReply* reply)
{
  if (auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      status_code != 200) {
    std::cerr << "handle_sync: " << status_code << '\n';
  }

  if (!reply->bytesAvailable())
    return;

  auto data = json::parse(reply->readAll().toStdString());
  next_batch_ = data.value("next_batch", ""s);

  sync_rooms(data["rooms"]);

  long_sync();
}


void matrix::AccessManager::sync_rooms(json& rooms)
{
  for (auto it = std::begin(rooms["join"]); it != std::end(rooms["join"]); ++it) {
    std::string room_id = it.key();
    if (auto* room = client_->room(room_id); !room) {
      room_list_model_->add_room(Room{room_id, "#test:matrix.org"});
    }
    if (auto* room = client_->room(room_id); room) {
      if (const auto& timeline = it.value()["timeline"]; !timeline.empty()) {
        for (const auto& event : timeline["events"]) {
          const auto& content = event["content"];
          const auto sender = event["sender"].get<std::string>();
          if (sender != user_id_ && event["type"] == "m.room.message" &&
              content["msgtype"] == "m.text") {
            Message message;
            message.event_id = event["event_id"].get<std::string>();
            message.room_id = room_id;
            message.user_id = std::move(sender);
            message.type = MessageType::Text;
            message.text = content["body"].get<std::string>();
            if (room == room_model_->current_room())
              room_model_->add_message(std::move(message));
            else
              room->add_message(std::move(message));
          }
        }
      }
    }
  }
}


void matrix::AccessManager::send(const QString& room_id, const QString& message)
{
  auto url = "/rooms/{}/send/m.room.message?access_token={}"_format(room_id.toStdString(),
      access_token_);
  json data = {
    {"msgtype", "m.text"},
    {"body", message.toStdString()}
  };
  post(std::move(url), data.dump());
}
