#include "accessmanager.h"


#include <tuple>
#include <regex>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "ext/fmt/fmtlib.h"

#include "util/util.h"
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


inline bool is_valid(const QNetworkReply* reply)
{
  return reply && reply->error() == QNetworkReply::NoError &&
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200 &&
      reply->bytesAvailable();
}


inline std::tuple<std::string, std::string> split_mxc(std::string_view mxc_url)
{
  static const std::regex rx{R"(mxc://(.+)/(.+))"};
  std::match_results<std::string_view::const_iterator> m;
  if (std::regex_search(std::begin(mxc_url), std::end(mxc_url), m, rx)) {
    return {m[1], m[2]};
  }
  return {"", ""};
}


inline std::tuple<std::string, std::string> split_content_type(std::string_view content_type)
{
  static const std::regex rx{R"((.+)/(.+))"};  // Type/subtype (see RFC 1521, 822)
  std::match_results<std::string_view::const_iterator> m;
  if (std::regex_search(std::begin(content_type), std::end(content_type), m, rx)) {
    return {m[1], m[2]};
  }
  return {"", ""};
}


inline std::string as_string(matrix::RoomState state)
{
  switch (state) {
    case matrix::RoomState::Name: return "m.room.name"s;
    default: return ""s;
  }
}


inline matrix::MessageType as_msgtype(std::string_view sv)
{
  static const std::map<std::string, matrix::MessageType, std::less<>> types{
    {"m.text", matrix::MessageType::Text},
    {"m.emote", matrix::MessageType::Emote},
    {"m.notice", matrix::MessageType::Notice},
    {"m.image", matrix::MessageType::Image},
    {"m.file", matrix::MessageType::File},
    {"m.location", matrix::MessageType::Location},
    {"m.video", matrix::MessageType::Video},
    {"m.audio", matrix::MessageType::Audio}
  };
  const auto it = types.find(sv);
  if (it != std::end(types))
    return it->second;
  return matrix::MessageType::Unknown;
}


inline std::string as_string(matrix::MessageType type)
{
  static const std::map<matrix::MessageType, std::string> types{
    {matrix::MessageType::Text, "m.text"},
    {matrix::MessageType::Emote, "m.emote"},
    {matrix::MessageType::Notice, "m.notice"},
    {matrix::MessageType::Image, "m.image"},
    {matrix::MessageType::File, "m.file"},
    {matrix::MessageType::Location, "m.location"},
    {matrix::MessageType::Video, "m.video"},
    {matrix::MessageType::Audio, "m.audio"}
  };
  const auto it = types.find(type);
  if (it != std::end(types))
    return it->second;
  return "m.text";
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

  if (util::starts_with(message.text, "/me ")) {
    message.type = MessageType::Emote;
    message.text.erase(0, 4);
  }
  else {
    message.type = MessageType::Text;
  }

  auto url = "/rooms/{}/send/m.room.message/{}?access_token={}"_format(message.room_id,
      ++transaction_id_, access_token_);
  json data = {
    {"msgtype", as_string(message.type)},
    {"body", message.text}
  };

  auto* m = room_model_->add_message(std::move(message));
  auto* r = put(client_url_base_ + url, data.dump());

  connect(r, &QNetworkReply::finished, r, [this, m, r]{
    if (is_valid(r)) {
      auto data = json::parse(r->readAll().toStdString());
      auto event_id = data["event_id"].get<std::string>();
      // Remote echo may have already arrived before request finished (see r0.2.0 spec local echo)
      if (auto it = std::find(std::begin(confirmed_events_), std::end(confirmed_events_), event_id);
          it != std::end(confirmed_events_)) {
        confirmed_events_.erase(it);
        m->transmit_confirmed = true;
        room_model_->data_changed(m);
      }
      else {
        unconfirmed_messages_.insert(std::pair{event_id, m});
      }
    }
    else {
      m->transmit_failed = true;
      room_model_->data_changed(m);
    }
    r->deleteLater();
  });
}


QNetworkReply* matrix::AccessManager::request_media(std::string_view server, std::string_view id)
{
  return get(server_ + "/_matrix/media/r0/download/{}/{}"_format(std::string{server}, std::string{id}));
}


QNetworkReply* matrix::AccessManager::request_media(std::string&& server, std::string&& id)
{
  return get(server_ + "/_matrix/media/r0/download/{}/{}"_format(std::move(server), std::move(id)));
}


QNetworkReply* matrix::AccessManager::request_media(std::string_view mxc_url)
{
  if (auto&& [server, id] = split_mxc(mxc_url); !server.empty() && !id.empty()) {
    return request_media(std::move(server), std::move(id));
  }
  return nullptr;
}


QNetworkReply* matrix::AccessManager::request_thumbnail(std::string_view server, std::string_view id,
    int width, int height)
{
  return get(server_ + "/_matrix/media/r0/thumbnail/{}/{}?width={}&height={}&method=scale"_format(
      std::string{server}, std::string{id}, width, height));
}


QNetworkReply* matrix::AccessManager::request_thumbnail(std::string&& server, std::string&& id,
    int width, int height)
{
  return get(server_ + "/_matrix/media/r0/thumbnail/{}/{}?width={}&height={}&method=scale"_format(
      std::move(server), std::move(id), width, height));
}


QNetworkReply* matrix::AccessManager::request_thumbnail(std::string_view mxc_url, int width, int height)
{
  if (auto&& [server, id] = split_mxc(mxc_url); !server.empty() && !id.empty()) {
    return request_thumbnail(std::move(server), std::move(id), width, height);
  }
  return nullptr;
}


void matrix::AccessManager::request_init_sync()
{
  auto* r = get(client_url_base_ + "/sync?filter={1}&access_token={0}"_format(access_token_,
      R"({"room":{"timeline":{"limit":20}}})"));
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


void matrix::AccessManager::download_thumbnail(std::string_view mxc_url, int width, int height)
{
  if (auto* r = request_thumbnail(mxc_url, width, height); r) {
    connect(r, &QNetworkReply::finished, r, [=]{
      handle_content(std::get<1>(split_mxc(mxc_url)), r);
      r->deleteLater();
    });
  }
}


void matrix::AccessManager::handle_content(std::string_view id, QNetworkReply* reply)
{
  if (is_valid(reply)) {
    auto content_type = reply->header(QNetworkRequest::ContentTypeHeader).toString().toStdString();
    if (auto [type, subtype] = split_content_type(content_type); type == "image") {
      QPixmap p;
      p.loadFromData(reply->readAll(), subtype.c_str());
      image_provider_->add_pixmap(QString::fromStdString(std::string{id}), std::move(p));
    }
  }
}


void matrix::AccessManager::handle_login(QNetworkReply* reply)
{
  auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  auto response = static_cast<LoginResponse>(status_code);

  json data;
  if (reply->bytesAvailable() > 0)
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
  if (is_valid(reply)) {
    auto data = json::parse(reply->readAll().toStdString());
    next_batch_ = data.value("next_batch", ""s);
    sync_rooms(data["rooms"]);
  }

  request_long_sync();
}


void matrix::AccessManager::handle_user_profile(User* user, QNetworkReply* reply)
{
  if (is_valid(reply)) {
    auto data = json::parse(reply->readAll().toStdString());
    user->set_display_name(data.value("displayname", ""s));
    user->set_avatar_url(data.value("avatar_url", ""s));
    request_media(user->avatar_url());
  }
}


void matrix::AccessManager::handle_room_state(Room* room, RoomState state, QNetworkReply* reply)
{
  if (is_valid(reply)) {
    auto data = json::parse(reply->readAll().toStdString());
    switch (state) {
      case RoomState::Name: room->set_name(data.value("name", ""s)); break;
    }
  }
}


void matrix::AccessManager::handle_room_members(Room* room, QNetworkReply* reply)
{
  if (is_valid(reply)) {
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
          download_thumbnail(user->avatar_url(), 64, 64);
        }
        room->add_member(user);
      }
    }
  }
}


void matrix::AccessManager::sync_rooms(const json& rooms)
{
  for (auto it = std::begin(rooms["join"]); it != std::end(rooms["join"]); ++it) {
    std::string room_id = it.key();
    auto* room = client_->room(room_id);

    // User joined a new room
    if (!room) {
      room = room_list_model_->add_room(Room{room_id});
      request_room_state(room, RoomState::Name);
      request_room_members(room);
    }

    if (const auto& timeline = it.value()["timeline"]; !timeline.empty()) {
      sync_room(room, timeline);
    }
  }
}


void matrix::AccessManager::sync_room(Room* room, const json& timeline)
{
  for (const auto& event : timeline["events"]) {
    const auto& content = event["content"];
    const auto sender = event["sender"].get<std::string>();
    if (sender == client_->user_id()) {
      // Confirm transmitted messages by comparing local with remote echo from stream
      auto it = unconfirmed_messages_.find(event["event_id"]);
      if (it != std::end(unconfirmed_messages_)) {
        auto* m = it->second;
        unconfirmed_messages_.erase(it);
        m->transmit_confirmed = true;
        room_model_->data_changed(m);
      }
      else {
        // Remote echo arrived before original send request finished (see r0.2.0 spec local echo)
        confirmed_events_.push_back(event["event_id"]);
      }
    }
    else if (event["type"] == "m.room.message") {
      Message message;
      message.event_id = event["event_id"].get<std::string>();
      message.room_id = room->id();
      message.user_id = std::move(sender);
      message.type = as_msgtype(content["msgtype"].get<std::string>());
      message.text = content["body"].get<std::string>();

      if (message.type == matrix::MessageType::Image) {
        message.url = content["url"].get<std::string>();
        message.content_loading = true;
      }

      if (room == room_model_->current_room())
        room_model_->add_message(std::move(message));
      else
        room->add_message(std::move(message));

      if (auto* m = room->last_message(); m && m->type == matrix::MessageType::Image) {
        if (auto* r = request_thumbnail(m->url, 512, 512); r) {
          connect(r, &QNetworkReply::finished, r, [this, r, m]{
            auto id = std::get<1>(split_mxc(m->url));
            handle_content(std::string{id}, r);
            m->image_id = id;
            m->content_loading = false;
            room_model_->data_changed(m);
            r->deleteLater();
          });
        }
      }
    }
  }
}
