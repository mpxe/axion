#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtQml>
#include <QIcon>

#include "matrixaccessmanager.h"
#include "roommodel.h"
#include "membersmodel.h"


namespace
{


void connect_to_db()
{
  QSqlDatabase database = QSqlDatabase::database();
  if (!database.isValid()) {
    database = QSqlDatabase::addDatabase("QSQLITE");
    if (!database.isValid())
      qFatal("Cannot add database: %s", qPrintable(database.lastError().text()));
  }

  const QDir dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  if (!dir.mkpath("."))
    qFatal("Failed to create writable directory at %s", qPrintable(dir.absolutePath()));

  // Ensure that we have a writable location on all devices
  const QString filename = dir.absolutePath() + "/chat-database.sqlite3";

  // When using the SQLite driver, open() will create the SQLite database if it doesn't exist
  database.setDatabaseName(filename);
  if (!database.open()) {
    qFatal("Cannot open database: %s", qPrintable(database.lastError().text()));
    QFile::remove(filename);
  }
}


}  // anonymous namespace


int main(int argc, char *argv[])
{
  //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

  QGuiApplication app(argc, argv);
  app.setWindowIcon(QIcon{"qrc:/img/res/img/icon.png"});

  qmlRegisterType<MembersModel>("im.axion", 1, 0, "MembersModel");
  qmlRegisterType<RoomModel>("im.axion", 1, 0, "RoomModel");

  connect_to_db();

  matrix::MatrixAccessManager matrix;

  QQmlApplicationEngine engine;
  engine.load(QUrl(QLatin1String("qrc:/main.qml")));
  engine.rootContext()->setContextProperty("matrix", &matrix);

  auto* login_page = engine.rootObjects()[0]->findChild<QObject*>("loginPage");

  if (login_page) {
    QObject::connect(&matrix, &matrix::MatrixAccessManager::login_failure, &engine,
        [login_page](const auto& s){ login_page->setProperty("errorMessage", s); });
    QObject::connect(&matrix, &matrix::MatrixAccessManager::login_success, &engine,
        [login_page]{ login_page->setProperty("success", true); });
  }

  return app.exec();
}
