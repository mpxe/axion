#include "membersmodel.h"


#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>


namespace
{


const char* members_table = "Members";


void create_table()
{
  if (QSqlDatabase::database().tables().contains(members_table)) {
    return;
  }

  QSqlQuery query;
  if (!query.exec("CREATE TABLE IF NOT EXISTS 'Members' ("
      "'user_id' TEXT PRIMARY KEY NOT NULL,"
      "'account_name' TEXT NOT NULL,"
      "'display_name' TEXT NOT NULL"
      ")")) {
    qFatal("Failed to query database: %s", qPrintable(query.lastError().text()));
  }

  query.exec("INSERT INTO Members VALUES('@miku:example.com', 'miku', '初音ミク')");
  query.exec("INSERT INTO Members VALUES('@yuzuru:example.com', 'yuzuru', 'Yuzuru')");
  query.exec("INSERT INTO Members VALUES('@krauss:example.com', 'krauss', 'Lawrence Krauss')");
  query.exec("INSERT INTO Members VALUES('@smolin:example.com', 'smolin', 'Lee Smolin')");
}


}  // anonymous namespace


MembersModel::MembersModel(QObject* parent) : QSqlTableModel{parent}
{
  create_table();
  setTable(members_table);
  setSort(1, Qt::AscendingOrder);
  select();
  setEditStrategy(QSqlTableModel::OnManualSubmit);
}


QVariant MembersModel::data(const QModelIndex& index, int role) const
{
  if (role < Qt::UserRole)
    return QSqlTableModel::data(index, role);

  const QSqlRecord r = record(index.row());
  return r.value(role - Qt::UserRole);
}


QHash<int, QByteArray> MembersModel::roleNames() const
{
  QHash<int, QByteArray> names;
  names[Qt::UserRole] = "user_id";
  names[Qt::UserRole + 1] = "account_name";
  names[Qt::UserRole + 2] = "display_name";
  return names;
}
