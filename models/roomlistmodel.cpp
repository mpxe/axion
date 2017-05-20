#include "roomlistmodel.h"


#include "matrix/client.h"


RoomListModel::RoomListModel(matrix::Client* client, QObject* parent)
    : QAbstractListModel{parent}, client_{client}
{
}


int RoomListModel::rowCount([[maybe_unused]] const QModelIndex& parent) const
{
  return static_cast<int>(client_->room_count());
}


QVariant RoomListModel::data(const QModelIndex& index, int role) const
{
  if (role < Qt::UserRole)
    return QVariant{};

  if (auto* room = client_->room(index.row()); room) {
    switch (static_cast<RoomRole>(role)) {
      case RoomRole::Id: return QString::fromUtf8(room->id().c_str());
      case RoomRole::MainAddress: return QString::fromUtf8(room->main_address().c_str());
      case RoomRole::Name: return QString::fromUtf8(room->name().c_str());
    }
  }

  return QVariant{};
}


QHash<int, QByteArray> RoomListModel::roleNames() const
{
  QHash<int, QByteArray> names;
  names[static_cast<int>(RoomRole::Id)] = "room_id";
  names[static_cast<int>(RoomRole::MainAddress)] = "main_address";
  names[static_cast<int>(RoomRole::Name)] = "room_name";

  return names;
}


void RoomListModel::add_room(matrix::Room&& room)
{
  beginInsertRows(QModelIndex{}, rowCount(), rowCount());
  client_->add_room(std::move(room));
  endInsertRows();
}
