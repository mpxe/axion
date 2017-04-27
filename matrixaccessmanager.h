#ifndef MATRIX_ACCESS_MANAGER_H
#define MATRIX_ACCESS_MANAGER_H


#include <string>
#include <unordered_map>

#include <QObject>
#include <QHash>
class QNetworkAccessManager;
class QNetworkReply;

#include "ext/json.hpp"


namespace matrix
{


enum class LoginResponse
{
  Authenticated = 200,
  Invalid = 400,
  Failed = 403,
  RateLimited = 429
};


template<typename T> inline T get(nlohmann::json j, const std::string& key, const T& def)
{
  if (j.count(key)) {
    return j[key];
  }
  return def;
}


template<typename T> inline T get(nlohmann::json j, const std::string& key, T&& def)
{
  if (j.count(key)) {
    return j[key];
  }
  return def;
}


class MatrixAccessManager : public QObject
{
  Q_OBJECT

public:
  MatrixAccessManager();
  ~MatrixAccessManager();

signals:
  void login_success();
  void login_failure(const QString& message);

public slots:
  void login(const QString& id, const QString& pw);
  void logout();
  void sync();
  void send(const QString& room, const QString& message);

private:
  void handle_sync(QNetworkReply* reply);
  void handle_login(QNetworkReply* reply);

  QNetworkAccessManager* network_;
  QString access_token_;
  QHash<QString, QString> room_ids_;
};


}  // namespace matrix


#endif  // MATRIX_ACCESS_MANAGER_H
