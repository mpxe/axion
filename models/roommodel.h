#ifndef ROOM_MODEL_H
#define ROOM_MODEL_H


#include <QAbstractListModel>
#include <QString>
#include <QVariant>
#include <QHash>
#include <QByteArray>

#include "matrix/client.h"
#include "matrix/room.h"
#include "matrix/message.h"


class RoomModel : public QAbstractListModel
{
  Q_OBJECT

  Q_PROPERTY(QString room READ room_id WRITE set_room NOTIFY room_changed)
  Q_PROPERTY(QString room_name READ room_name NOTIFY room_changed)
  Q_PROPERTY(QString user READ user_id NOTIFY user_changed)

public:
  enum class RoomRole {
    UserId = Qt::UserRole, AccountName, DisplayName, AvatarId, MessageType, MessageText,
        MessageUrl, ImageId, Timestamp, TransmitConfirmed, TransmitFailed, ContentLoading
  };

  RoomModel(matrix::Client* client, QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex{}) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QString user_id() const { return client_ ? client_->user_id().c_str() : QString{}; }
  QString room_id() const { return room_ ? room_->id().c_str() : QString{}; }
  QString room_name() const { return room_ ? room_->name().c_str() : QString{}; }
  void set_room(const QString& id);
  const matrix::Room* current_room() const { return room_; }

  matrix::Message* add_message(matrix::Message&& message);
  void data_changed(matrix::Message* message);

signals:
  void room_changed();
  void user_changed();

private:
  matrix::Client* client_;
  matrix::Room* room_ = nullptr;
};


#endif  // ROOM_MODEL_H
