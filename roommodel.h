#ifndef ROOM_MODEL_H
#define ROOM_MODEL_H


#include <QSqlTableModel>
#include <QString>
#include <QVariant>
#include <QHash>
#include <QByteArray>


class RoomModel : public QSqlTableModel
{
  Q_OBJECT

  Q_PROPERTY(QString room READ room_id WRITE set_room NOTIFY room_changed)

public:
  RoomModel(QObject* parent = nullptr);

  QVariant data(const QModelIndex& index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QString room_id() const;
  void set_room(const QString& id);

  Q_INVOKABLE void sendMessage(const QString& message);

signals:
  void room_changed();

private:
  QString room_id_;
};


#endif  // ROOM_MODEL_H
