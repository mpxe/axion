#include "accessmanager.h"


#include <iostream>

#include <QNetworkAccessManager>
#include <QNetworkReply>


using namespace std::string_literals;
using json = nlohmann::json;
using error_signal = void(QNetworkReply::*)(QNetworkReply::NetworkError);


matrix::AccessManager::AccessManager() : network_{new QNetworkAccessManager{this}}
{
  // Establish connection to reduce latency of first http request
  network_->connectToHost("https://matrix.org");

  connect(network_, &QNetworkAccessManager::finished, this, [this](auto* r){ r->deleteLater(); });
}


matrix::AccessManager::~AccessManager()
{
  if (!access_token_.isEmpty())
    logout();
}


void matrix::AccessManager::login(const QString& id, const QString& pw)
{
  QNetworkRequest request{QUrl{"https://matrix.org/_matrix/client/r0/login"}};
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  json data = {
    {"type", "m.login.password"},
    {"user", id.toStdString()},
    {"password", pw.toStdString()},
  };

  auto* reply = network_->post(request, QByteArray::fromStdString(data.dump()));
  connect(reply, &QNetworkReply::readyRead, this, [this, reply]{ handle_login(reply); });
}


void matrix::AccessManager::logout()
{
  QNetworkRequest request{QUrl{"https://matrix.org/_matrix/client/r0/logout"}};
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  network_->post(request, QByteArray{});
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
      access_token_ = QString::fromStdString(get(data, "access_token", std::string{}));
      if (access_token_.isEmpty()) {
        emit login_failure("Could not read access token");
      }
      else {
        emit login_success();
      }
    }
    break;

    case LoginResponse::Invalid:
    case LoginResponse::Failed: {
      emit login_failure(QString::fromStdString(get(data, "error", "Unknown error occured"s)));
    }
    break;

    case LoginResponse::RateLimited: {
      emit login_failure("Rate-limited, try again later");
    }
    break;

    default: emit login_failure("Received unknown status code");
  }
}


void matrix::AccessManager::sync()
{
  auto url = QString{"https://matrix.org/_matrix/client/r0/sync?access_token=%1"}.arg(access_token_);
  QNetworkRequest request{QUrl{url}};
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  auto* reply = network_->get(request);
  connect(reply, &QNetworkReply::readyRead, this, [this, reply]{ handle_sync(reply); });
}


void matrix::AccessManager::handle_sync(QNetworkReply* reply)
{
  auto status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  if (status_code != 200 || !reply->bytesAvailable())
    return;

  auto data = json::parse(reply->readAll().toStdString());

  std::cout << data.dump(2) << std::endl;
}


void matrix::AccessManager::send(const QString& room, const QString& message)
{
  auto url = QString{"https://matrix.org/_matrix/client/r0/rooms/%1/send/m.room.message?access_token=%2"}
      .arg(room_ids_[room], access_token_);
  QNetworkRequest request{QUrl{url}};
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  json data = {
    {"msgtype", "m.text"},
    {"body", message.toStdString()}
  };

  network_->post(request, QByteArray::fromStdString(data.dump()));
}
