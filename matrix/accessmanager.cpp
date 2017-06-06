#include "accessmanager.h"


#include <regex>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "ext/fmt/fmtlib.h"

#include "matrix/imageprovider.h"
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


inline QNetworkRequest create_request(std::string&& url)
{
  QNetworkRequest request{QUrl{QString::fromStdString(url)}};
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


matrix::AccessManager::AccessManager(std::string&& server, Client* client, ImageProvider* image_provider,
    RoomModel* room_model, RoomListModel* room_list_model, MemberListModel* member_list_model)
    : server_{std::move(server)}, client_url_base_{server_ + "/_matrix/client/r0"},
      network_{new QNetworkAccessManager{this}}, client_{client}, image_provider_{image_provider},
      room_model_{room_model}, room_list_model_{room_list_model}, member_list_model_{member_list_model}
{
  // Establish connection to reduce latency of first http request
  network_->connectToHost(QString::fromStdString(server_));
}


matrix::AccessManager::~AccessManager()
{
  if (!access_token_.empty())
    logout();
}


QNetworkReply* matrix::AccessManager::put(std::string&& url)
{
  return network_->put(create_request(std::move(url)), QByteArray{});
}


QNetworkReply* matrix::AccessManager::put(std::string&& url, std::string&& data)
{
  return network_->put(create_request(std::move(url)), QByteArray::fromStdString(data));
}


QNetworkReply* matrix::AccessManager::post(std::string&& url)
{
  return network_->post(create_request(std::move(url)), QByteArray{});
}


QNetworkReply* matrix::AccessManager::post(std::string&& url, std::string&& data)
{
  return network_->post(create_request(std::move(url)), QByteArray::fromStdString(data));
}


QNetworkReply* matrix::AccessManager::get(std::string&& url)
{
  return network_->get(create_request(std::move(url)));
}


void matrix::AccessManager::login(const QString& id, const QString& pw)
{
  json data = {
    {"type", "m.login.password"},
    {"user", id.toStdString()},
    {"password", pw.toStdString()},
  };
  auto* r = post(client_url_base_ + "/login", data.dump());
  connect(r, &QNetworkReply::finished, r, [this, r]{ handle_login(r); r->deleteLater(); });
}


void matrix::AccessManager::logout()
{
  auto* r = post(client_url_base_ + "/logout");
  connect(r, &QNetworkReply::finished, r, [r]{ r->deleteLater(); });
}


void matrix::AccessManager::send_message(const QString& room_id, const QString& text)
{
  matrix::Message message;
  message.room_id = room_id.toStdString();
  message.user_id = client_->user_id();
  message.text = text.toStdString();

  auto url = "/rooms/{}/send/m.room.message/{}?access_token={}"_format(message.room_id,
      ++transaction_id_, access_token_);
  json data = {
    {"msgtype", "m.text"},
    {"body", message.text}
  };

  auto* m = room_model_->add_message(std::move(message));
  auto* r = put(client_url_base_ + url, data.dump());

  connect(r, &QNetworkReply::finished, r, [this, m, r]{
    if (auto status_code = r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        status_code == 200 && r->bytesAvailable()) {
      auto data = json::parse(r->readAll().toStdString());
      unconfirmed_messages_.insert(std::pair{data["event_id"], m});
    }
    r->deleteLater();
  });
}


void matrix::AccessManager::request_media(const std::string& mxc_url)
{
  std::regex rx{R"(mxc://(.+)/(.+))"};
  std::smatch m;
  if (std::regex_search(mxc_url, m, rx)) {
    std::string server_name = m[1];
    std::string media_id = m[2];
    auto* r = get(server_ + "/_matrix/media/r0/download/{}/{}"_format(server_name, media_id));
    connect(r, &QNetworkReply::finished, r, [=]{ handle_media(media_id, r); r->deleteLater(); });
  }
}


void matrix::AccessManager::request_thumbnail(const std::string& mxc_url, int width, int height)
{
  std::regex rx{R"(mxc://(.+)/(.+))"};
  std::smatch m;
  if (std::regex_search(mxc_url, m, rx)) {
    std::string server_name = m[1];
    std::string media_id = m[2];
    auto* r = get(server_ + "/_matrix/media/r0/thumbnail/{}/{}?width={}&height={}&method=scale"_format(
        server_name, media_id, width, height));
    connect(r, &QNetworkReply::finished, r, [=]{ handle_media(media_id, r); r->deleteLater(); });
  }
}


void matrix::AccessManager::request_init_sync()
{
  auto* r = get(client_url_base_ + "/sync?filter={1}&access_token={0}"_format(access_token_,
      R"({"room":{"timeline":{"limit":1}}})"));
  connect(r, &QNetworkReply::finished, r, [=]{ handle_sync(r); r->deleteLater(); });
}


void matrix::AccessManager::request_long_sync()
{
  auto* r = get(client_url_base_ + "/sync?since={}&timeout={}&access_token={}"_format(next_batch_,
      30000, access_token_));
  connect(r, &QNetworkReply::finished, r, [=]{ handle_sync(r); r->deleteLater(); });
}


void matrix::AccessManager::request_user_profile(User* user)
{
  auto* r = get(client_url_base_ + "/profile/{}?access_token={}"_format(user->id(), access_token_));
  connect(r, &QNetworkReply::finished, r, [=]{ handle_user_profile(user, r); r->deleteLater(); });
}


void matrix::AccessManager::request_room_state(Room* room, RoomState state)
{
  auto* r = get(client_url_base_ + "/rooms/{}/state/{}?access_token={}"_format(room->id(),
      as_string(state), access_token_));
  connect(r, &QNetworkReply::finished, r, [=]{ handle_room_state(room, state, r); r->deleteLater(); });
}


void matrix::AccessManager::request_room_members(Room* room)
{
  auto* r = get(client_url_base_ + "/rooms/{}/members?access_token={}"_format(room->id(), access_token_));
  connect(r, &QNetworkReply::finished, r, [=]{ handle_room_members(room, r); r->deleteLater(); });
}


void matrix::AccessManager::handle_media(const std::string& media_id, QNetworkReply* reply)
{
  if (auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      status_code == 200 && reply->bytesAvailable()) {
    QPixmap p;
    p.loadFromData(reply->readAll());
    std::cout << p.width() << " x " << p.height() << std::endl;
    image_provider_->add_pixmap(QString::fromStdString(media_id), std::move(p));
  }
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
        request_init_sync();
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

  request_long_sync();
}


void matrix::AccessManager::handle_user_profile(User* user, QNetworkReply* reply)
{
  if (auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      status_code == 200 && reply->bytesAvailable()) {
    auto data = json::parse(reply->readAll().toStdString());
    user->set_display_name(data.value("displayname", ""s));
    user->set_avatar_url(data.value("avatar_url", ""s));
    request_media(user->avatar_url());
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
        auto content = u["content"];
        auto displayname = content["displayname"];
        auto avatar_url = content["avatar_url"];
        user = client_->add_user(User{std::move(id)});
        if (!displayname.is_null()) {
          user->set_display_name(displayname);
        }
        else {
          user->set_display_name(std::string{user->account_name()});
        }
        if (!avatar_url.is_null()) {
          user->set_avatar_url(avatar_url);
          request_thumbnail(user->avatar_url(), 64, 64);
        }
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
          if (sender == client_->user_id()) {
            auto it = unconfirmed_messages_.find(event["event_id"]);
            if (it != std::end(unconfirmed_messages_)) {
              auto* m = it->second;
              m->transmit_confirmed = true;
              room_model_->data_changed(m);
            }
          }
          else if (event["type"] == "m.room.message" && content["msgtype"] == "m.text") {
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
