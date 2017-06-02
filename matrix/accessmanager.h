#ifndef MATRIX_ACCESS_MANAGER_H
#define MATRIX_ACCESS_MANAGER_H


#include <cstdint>
#include <string>
#include <unordered_map>

#include <QObject>
#include <QHash>
class QNetworkAccessManager;
class QNetworkReply;

#include "ext/json.hpp"

namespace matrix { class ImageProvider; class Client; class User; class Room; }

class RoomModel;
class RoomListModel;
class MemberListModel;



namespace matrix
{


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


enum class RoomState
{
  Name
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
  inline QNetworkReply* post(std::string&& url);
  inline QNetworkReply* post(std::string&& url, std::string&& data);
  inline QNetworkReply* get(std::string&& url);

  void request_media(const std::string& mxc_url);
  void request_init_sync();
  void request_long_sync();
  void request_user_profile(User* user);
  void request_room_state(Room* room, RoomState state);
  void request_room_members(Room* room);

  void handle_media(const std::string& media_id, QNetworkReply* reply);
  void handle_login(QNetworkReply* reply);
  void handle_sync(QNetworkReply* reply);
  void handle_user_profile(User* user, QNetworkReply* reply);
  void handle_room_state(Room* room, RoomState state, QNetworkReply* reply);
  void handle_room_members(Room* room, QNetworkReply* reply);

  void sync_rooms(nlohmann::json& rooms);

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
};


}  // namespace matrix


#endif  // MATRIX_ACCESS_MANAGER_H
