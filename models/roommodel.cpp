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
      case RoomRole::AvatarId: {
        if (auto* user = room_->member(m->user_id); user)
          return QString::fromUtf8(user->avatar_id().c_str());
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
  names[static_cast<int>(RoomRole::AvatarId)] = "avatar_id";
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


matrix::Message* RoomModel::add_message(matrix::Message&& message)
{
  beginInsertRows(QModelIndex{}, 0, 0);
  room_->add_message(std::move(message));
  endInsertRows();
  return room_->last_message();
}


void RoomModel::data_changed(matrix::Message* message)
{
  auto i = room_->index_of(message);
  if (i < static_cast<std::size_t>(rowCount()))
    emit dataChanged(index(i), index(i));
}
