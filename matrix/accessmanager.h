#ifndef MATRIX_ACCESS_MANAGER_H
#define MATRIX_ACCESS_MANAGER_H


#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include <QObject>
#include <QHash>
class QNetworkAccessManager;
class QNetworkReply;

#include "ext/json.hpp"

#include "matrix/room.h"
class RoomModel;
class RoomListModel;
class MemberListModel;



namespace matrix
{


class ImageProvider;
class Client;
class User;
class Message;


enum class LoginResponse
{
  Authenticated = 200,
  Invalid = 400,
  Failed = 403,
  RateLimited = 429
};


enum class RoomStateResponse
{
  NotAMember = 403,
  UnknownState = 404
};


class AccessManager : public QObject
{
  Q_OBJECT

public:
  AccessManager(std::string&& server, Client* client, ImageProvider* image_provider,
      RoomModel* room_model, RoomListModel* room_list_model, MemberListModel* member_list_model);
  ~AccessManager();

signals:
  void login_success();
  void login_failure(const QString& message);

public slots:
  void login(const QString& id, const QString& pw);
  void logout();
  void send_message(const QString& room_id, const QString& message);

private:
  inline QNetworkReply* put(std::string&& url);
  inline QNetworkReply* put(std::string&& url, std::string&& data);
  inline QNetworkReply* post(std::string&& url);
  inline QNetworkReply* post(std::string&& url, std::string&& data);
  inline QNetworkReply* get(std::string&& url);

  QNetworkReply* request_media(std::string&& server, std::string&& id);
  QNetworkReply* request_media(std::string_view mxc_url);
  QNetworkReply* request_thumbnail(std::string&& server, std::string&& id, int width, int height);
  QNetworkReply* request_thumbnail(std::string_view mxc_url, int width, int height);

  void request_init_sync();
  void request_long_sync();
  void request_user_profile(User* user);
  void request_room_state(Room* room, RoomState state);
  void request_room_members(Room* room);

  void handle_content(std::string_view id, QNetworkReply* reply);
  void handle_login(QNetworkReply* reply);
  void handle_sync(QNetworkReply* reply);
  void handle_user_profile(User* user, QNetworkReply* reply);
  void handle_room_state(Room* room, RoomState state, QNetworkReply* reply);
  void handle_room_members(Room* room, QNetworkReply* reply);

  void download_thumbnail(std::string_view mxc_url, int width, int height);
  void sync_rooms(const nlohmann::json& rooms);
  void sync_room(Room* room, const nlohmann::json& timeline);

  const std::string server_;
  std::string home_server_;
  std::string client_url_base_;
  std::uint64_t transaction_id_ = 255;

  std::string access_token_;
  std::string next_batch_;

  QNetworkAccessManager* network_;
  Client* client_;
  ImageProvider* image_provider_;
  RoomModel* room_model_;
  RoomListModel* room_list_model_;
  MemberListModel* member_list_model_;

  std::map<std::string, Message*> unconfirmed_messages_;
};


}  // namespace matrix


#endif  // MATRIX_ACCESS_MANAGER_H
