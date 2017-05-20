#ifndef ROOM_MODEL_H
#define ROOM_MODEL_H


#include <QAbstractListModel>
#include <QString>
#include <QVariant>
#include <QHash>
#include <QByteArray>

#include "matrix/room.h"
#include "matrix/message.h"
namespace matrix { class Client; }


class RoomModel : public QAbstractListModel
{
  Q_OBJECT

  Q_PROPERTY(QString room READ room_id WRITE set_room NOTIFY room_changed)
  Q_PROPERTY(QString room_name READ room_name NOTIFY room_changed)

public:
  enum class RoomRole {
    AccountName = Qt::UserRole, DisplayName, MessageText, Timestamp, TransmitConfirmed
  };

  RoomModel(matrix::Client* client, QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex{}) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QString room_id() const { return room_ ? room_->id().c_str() : QString{}; }
  QString room_name() const { return room_ ? room_->name().c_str() : QString{}; }
  void set_room(const QString& id);
  const matrix::Room* current_room() const { return room_; }

  Q_INVOKABLE void add_message(const QString& message);
  void add_message(matrix::Message&& message);

signals:
  void room_changed();

private:
  matrix::Client* client_;
  matrix::Room* room_ = nullptr;
};


#endif  // ROOM_MODEL_H
