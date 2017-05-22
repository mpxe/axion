#include "accessmanager.h"


#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "ext/fmt/fmtlib.h"

#include "matrix/client.h"
#include "matrix/user.h"
#include "matrix/room.h"
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


inline std::string as_string(matrix::RoomState state)
{
  switch (state) {
    case matrix::RoomState::Name: return "m.room.name"s;
    default: return ""s;
  }
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
  connect(reply, &QNetworkReply::readyRead, reply, [this, reply]{ handle_login(reply); });
}


void matrix::AccessManager::logout()
{
  post("/logout");
}


void matrix::AccessManager::send_message(const QString& room_id, const QString& message)
{
  auto url = "/rooms/{}/send/m.room.message?access_token={}"_format(room_id.toStdString(),
      access_token_);
  json data = {
    {"msgtype", "m.text"},
    {"body", message.toStdString()}
  };
  post(std::move(url), data.dump());
}


void matrix::AccessManager::init_sync()
{
  auto* reply = get("/sync?filter={1}&access_token={0}"_format(access_token_,
      R"({"room":{"timeline":{"limit":1}}})"));
  connect(reply, &QNetworkReply::readyRead, reply, [=]{ handle_sync(reply); });
}


void matrix::AccessManager::long_sync()
{
  auto* reply = get("/sync?since={}&timeout={}&access_token={}"_format(next_batch_, 30000,
      access_token_));
  connect(reply, &QNetworkReply::readyRead, reply, [=]{ handle_sync(reply); });
}


void matrix::AccessManager::request_user_profile(User* user)
{
  auto* reply = get("/profile/{}?access_token={}"_format(user->id(), access_token_));
  connect(reply, &QNetworkReply::readyRead, reply, [=]{ handle_user_profile(user, reply); });
}


void matrix::AccessManager::request_room_state(Room* room, RoomState state)
{
  auto* reply = get("/rooms/{}/state/{}?access_token={}"_format(room->id(), as_string(state),
      access_token_));
  connect(reply, &QNetworkReply::readyRead, reply, [=]{ handle_room_state(room, state, reply); });
}


void matrix::AccessManager::request_room_members(Room* room)
{
  auto* reply = get("/rooms/{}/members?access_token={}"_format(room->id(), access_token_));
  connect(reply, &QNetworkReply::readyRead, reply, [=]{ handle_room_members(room, reply); });
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
      access_token_ = data.value("access_token", ""s);
      if (access_token_.empty()) {
        emit login_failure("Could not read access token");
      }
      else {
        home_server_ = data.value("home_server", ""s);
        auto* self = client_->add_self(User{data.value("user_id", ""s)});
        request_user_profile(self);
        init_sync();
        emit login_success();
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


void matrix::AccessManager::handle_user_profile(User* user, QNetworkReply* reply)
{
  if (auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      status_code == 200 && reply->bytesAvailable()) {
    auto data = json::parse(reply->readAll().toStdString());
    user->set_display_name(data.value("displayname", ""s));
    user->set_avatar_url(data.value("avatar_url", ""s));
  }
}


void matrix::AccessManager::handle_room_state(Room* room, RoomState state, QNetworkReply* reply)
{
  if (auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      status_code == 200 && reply->bytesAvailable()) {
    auto data = json::parse(reply->readAll().toStdString());
    switch (state) {
      case RoomState::Name: room->set_name(data.value("name", ""s)); break;
    }
  }
}


void matrix::AccessManager::handle_room_members(Room* room, QNetworkReply* reply)
{
  if (auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      status_code == 200 && reply->bytesAvailable()) {
    auto data = json::parse(reply->readAll().toStdString());
    for (const json& u : data["chunk"]) {
      auto id = u["user_id"].get<std::string>();
      if (auto* user = client_->user(id); user) {
        room->add_member(user);
      }
      else {
        const json& content = u["content"];
        user = client_->add_user(User{std::move(id), content["displayname"]});
        room->add_member(user);
      }
    }
  }
}


void matrix::AccessManager::sync_rooms(json& rooms)
{
  for (auto it = std::begin(rooms["join"]); it != std::end(rooms["join"]); ++it) {
    std::string room_id = it.key();
    if (auto* room = client_->room(room_id); !room) {
      room = room_list_model_->add_room(Room{room_id});
      request_room_state(room, RoomState::Name);
      request_room_members(room);
    }
    if (auto* room = client_->room(room_id); room) {
      if (const auto& timeline = it.value()["timeline"]; !timeline.empty()) {
        for (const auto& event : timeline["events"]) {
          const auto& content = event["content"];
          const auto sender = event["sender"].get<std::string>();
          if (sender != client_->user_id() && event["type"] == "m.room.message" &&
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
