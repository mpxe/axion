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

namespace matrix { class Client; }
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


class AccessManager : public QObject
{
  Q_OBJECT

public:
  AccessManager(std::string&& server, Client* client, RoomModel* room_model,
      RoomListModel* room_list_model, MemberListModel* member_list_model);
  ~AccessManager();

signals:
  void login_success();
  void login_failure(const QString& message);

public slots:
  void login(const QString& id, const QString& pw);
  void logout();
  void init_sync();
  void long_sync();
  void send(const QString& room_id, const QString& message);

private:
  inline QNetworkReply* post(std::string&& url);
  inline QNetworkReply* post(std::string&& url, std::string&& data);
  inline QNetworkReply* get(std::string&& url);
  void handle_sync(QNetworkReply* reply);
  void handle_login(QNetworkReply* reply);
  void sync_rooms(nlohmann::json& rooms);

  const std::string server_;
  const std::string url_base_;
  std::uint64_t transaction_id_ = 255;

  std::string user_id_;
  std::string access_token_;
  std::string next_batch_;

  QNetworkAccessManager* network_;
  Client* client_;
  RoomModel* room_model_;
  RoomListModel* room_list_model_;
  MemberListModel* member_list_model_;
};


}  // namespace matrix


#endif  // MATRIX_ACCESS_MANAGER_H
