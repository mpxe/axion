#ifndef MEMBER_LIST_MODEL_H
#define MEMBER_LIST_MODEL_H


#include <QAbstractListModel>
#include <QVariant>
#include <QHash>
#include <QByteArray>

#include "../matrix/client.h"


class MemberListModel : public QAbstractListModel
{
  Q_OBJECT

  Q_PROPERTY(QString room READ room_id WRITE set_room NOTIFY room_changed)

public:
  enum class MemberRole { AccountName = Qt::UserRole, DisplayName };

  MemberListModel(matrix::Client& client, QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex{}) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QString room_id() const { return room_ ? room_->id().c_str() : QString{}; }
  void set_room(const QString& id);

signals:
  void room_changed();

private:
  matrix::Client& client_;
  matrix::Room* room_ = nullptr;
};


#endif  // MEMBER_LIST_MODEL_H
