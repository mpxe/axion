#ifndef ROOM_LIST_MODEL_H
#define ROOM_LIST_MODEL_H


#include <QAbstractListModel>
#include <QVariant>
#include <QHash>
#include <QByteArray>

#include "matrix/room.h"
namespace matrix { class Client; }


class RoomListModel : public QAbstractListModel
{
  Q_OBJECT

public:
  enum class RoomRole { Id = Qt::UserRole, MainAddress, Name };

  RoomListModel(matrix::Client* client, QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex{}) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void add_room(matrix::Room&& room);

private:
  matrix::Client* client_;
};


#endif  // ROOM_LIST_MODEL_H
