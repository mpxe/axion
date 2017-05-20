#include "memberlistmodel.h"


#include "matrix/client.h"
#include "matrix/room.h"


MemberListModel::MemberListModel(matrix::Client* client, QObject* parent)
    : QAbstractListModel{parent}, client_{client}
{
}


int MemberListModel::rowCount([[maybe_unused]] const QModelIndex& parent) const
{
  if (room_)
    return static_cast<int>(room_->member_count());

  return 0;
}


QVariant MemberListModel::data(const QModelIndex& index, int role) const
{
  if (!room_ || role < Qt::UserRole)
    return QVariant{};

  if (auto* user = room_->member(index.row()); user) {
    switch (static_cast<MemberRole>(role)) {
      case MemberRole::UserId: return QString::fromUtf8(user->id().c_str());
      case MemberRole::AccountName: return QString::fromUtf8(user->account_name().c_str());
      case MemberRole::DisplayName: return QString::fromUtf8(user->display_name().c_str());
    }
  }

  return QVariant{};
}


QHash<int, QByteArray> MemberListModel::roleNames() const
{
  QHash<int, QByteArray> names;
  names[static_cast<int>(MemberRole::UserId)] = "user_id";
  names[static_cast<int>(MemberRole::AccountName)] = "account_name";
  names[static_cast<int>(MemberRole::DisplayName)] = "display_name";

  return names;
}


QString MemberListModel::room_id() const
{
  return room_ ? room_->id().c_str() : QString{};
}


void MemberListModel::set_room(const QString& id)
{
  if (auto room_id = id.toStdString(); !room_ || room_->id() != room_id)
  {
    beginResetModel();
    room_ = client_->room(room_id);
    endResetModel();
    emit room_changed();
  }
}
