#ifndef MEMBERS_MODEL_H
#define MEMBERS_MODEL_H


#include <QSqlTableModel>
#include <QVariant>
#include <QHash>
#include <QByteArray>


class MembersModel : public QSqlTableModel
{
  Q_OBJECT

public:
  MembersModel(QObject* parent = nullptr);

  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
};


#endif  // MEMBERS_MODEL_H
