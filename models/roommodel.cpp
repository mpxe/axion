#include "roommodel.h"


#include "matrix/client.h"


RoomModel::RoomModel(matrix::Client* client, QObject* parent)
    : QAbstractListModel{parent}, client_{client}
{
}


int RoomModel::rowCount([[maybe_unused]] const QModelIndex& parent) const
{
  if (room_)
    return static_cast<int>(room_->message_count());

  return 0;
}


QVariant RoomModel::data(const QModelIndex& index, int role) const
{
  if (!room_ || role < Qt::UserRole)
    return QVariant{};

  if (auto* m = room_->message(index.row()); m) {
    switch (static_cast<RoomRole>(role)) {
      case RoomRole::UserId: return QString::fromUtf8(m->user_id.c_str());
      case RoomRole::AccountName: {
        if (auto* user = room_->member(m->user_id); user)
          return QString::fromUtf8(user->account_name().c_str());
        return QVariant{};
      }
      case RoomRole::DisplayName: {
        if (auto* user = room_->member(m->user_id); user)
          return QString::fromUtf8(user->display_name().c_str());
        return QVariant{};
      }
      case RoomRole::MessageText: return QString::fromUtf8(m->text.c_str());
      case RoomRole::Timestamp: return m->timestamp;
      case RoomRole::TransmitConfirmed: return m->transmit_confirmed;
    }
  }

  return QVariant{};
}


QHash<int, QByteArray> RoomModel::roleNames() const
{
  QHash<int, QByteArray> names;
  names[static_cast<int>(RoomRole::UserId)] = "user_id";
  names[static_cast<int>(RoomRole::AccountName)] = "account_name";
  names[static_cast<int>(RoomRole::DisplayName)] = "display_name";
  names[static_cast<int>(RoomRole::MessageText)] = "message_text";
  names[static_cast<int>(RoomRole::Timestamp)] = "timestamp";
  names[static_cast<int>(RoomRole::TransmitConfirmed)] = "transmit_confirmed";
  return names;
}


void RoomModel::set_room(const QString& id)
{
  if (auto room_id = id.toStdString(); !room_ || room_->id() != room_id)
  {
    beginResetModel();
    room_ = client_->room(room_id);
    endResetModel();
    emit room_changed();
  }
}


void RoomModel::add_message(const QString& text)
{
  matrix::Message message;
  message.room_id = room_->id();
  message.user_id = client_->user_id();
  message.text = text.toStdString();
  message.transmit_confirmed = false;
  add_message(std::move(message));
}


void RoomModel::add_message(matrix::Message&& message)
{
  beginInsertRows(QModelIndex{}, 0, 0);
  client_->add_message(std::move(message));
  endInsertRows();
}
