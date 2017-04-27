#include "roommodel.h"


#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>


namespace
{


const char* messages_table = "Messages";


void create_table()
{
  if (!QSqlDatabase::database().tables().contains(messages_table)) {
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS 'Messages' ("
        "'event_id' TEXT PRIMARY KEY NOT NULL,"
        "'room_id' TEXT NOT NULL,"
        "'user_id' TEXT NOT NULL,"
        "'timestamp' INT NOT NULL,"
        "'message_type' TEXT NOT NULL,"
        "'message' TEXT NOT NULL"
        ")")) {
      qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
    }

    // Example data
    query.exec("INSERT INTO Messages VALUES('$1444812213350496Caaaa:example.com', "
        "'!Xq3620DUiqCaoxq:example.com', '@miku:example.com', 1042, 'm.text', 'こんばんは')");

    query.exec("INSERT INTO Messages VALUES('$1444812213350496Cbbbb:example.com', "
        "'!Xq3620DUiqCaoxq:example.com', '@yuzuru:example.com', 50789, 'm.text' ,'kawaii desu!!~~~')");

    query.exec("INSERT INTO Messages VALUES('$1444812213350496Ccccc:example.com', "
        "'!Xq3620DUiqCaoxq:example.com', '@miku:example.com', 20123, 'm.text', 'かわいいですか')");

    query.exec("INSERT INTO Messages VALUES('$1444812213350496Cdddd:example.com', "
        "'!Xq3620DUiqCaoxq:example.com', '@krauss:example.com', 50790, 'm.text', "
        "'If you have nothing in quantum mechanics, you will always have something.')");
  }
}


}  // anonymous namespace


RoomModel::RoomModel(QObject* parent) : QSqlTableModel{parent}
{
  create_table();
  setTable(messages_table);
  setSort(3, Qt::DescendingOrder);
  setEditStrategy(QSqlTableModel::OnManualSubmit);
  select();
}


QString RoomModel::room_id() const
{
  return room_id_;
}


void RoomModel::set_room(const QString& id)
{
  if (room_id_ != id)
  {
    room_id_ = id;
    setFilter(QString::fromLatin1("(room_id = '%1')").arg(room_id_));
    select();
    emit room_changed();
  }
}


QVariant RoomModel::data(const QModelIndex& index, int role) const
{
  if (role < Qt::UserRole)
    return QSqlTableModel::data(index, role);

  const QSqlRecord r = record(index.row());
  return r.value(role - Qt::UserRole);
}


QHash<int, QByteArray> RoomModel::roleNames() const
{
  QHash<int, QByteArray> names;
  names[Qt::UserRole] = "event_id";
  names[Qt::UserRole + 1] = "room_id";
  names[Qt::UserRole + 2] = "user_id";
  names[Qt::UserRole + 3] = "timestamp";
  names[Qt::UserRole + 4] = "message_type";
  names[Qt::UserRole + 5] = "message";
  return names;
}


void RoomModel::sendMessage(const QString& message)
{
  QSqlRecord r = record();
  r.setValue("event_id", QString::number(QDateTime::currentMSecsSinceEpoch()));
  r.setValue("user_id", "@self:example.com");
  r.setValue("room_id", room_id_);
  r.setValue("timestamp", QDateTime::currentMSecsSinceEpoch());
  r.setValue("message_type", "m.text");
  r.setValue("message", message);
  if (!insertRecord(rowCount(), r)) {
    qWarning() << "Failed to send message: " << lastError().text();
    return;
  }

  submitAll();
}
