#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QIcon>

#include "axion/configuration.h"
#include "matrix/accessmanager.h"
#include "matrix/imageprovider.h"
#include "matrix/client.h"
#include "matrix/user.h"
#include "matrix/room.h"
#include "matrix/message.h"
#include "models/roommodel.h"
#include "models/roomlistmodel.h"
#include "models/memberlistmodel.h"


namespace
{


void add_dummy_content(matrix::Client& client)
{
  client.add_self(matrix::User{"@self:example.com", u8"ಠ_ಠ"});
  matrix::Room music{"!Xq3620DUiqCaoxq:example.com"};
  music.set_name("music");

  matrix::User miku{"@miku:example.com", u8"初音ミク"};
  matrix::User yuzuru{"@yuzuru:example.com", "yuzuru"};

  music.add_member(client.self());
  music.add_member(client.add_user(matrix::User{miku}));
  music.add_member(client.add_user(matrix::User{yuzuru}));

  matrix::Message message;
  message.room_id = music.id();
  message.user_id = miku.id();
  message.text = u8"こんばんは";
  music.add_message(matrix::Message{message});
  message.text = u8"かわいいですか";
  music.add_message(matrix::Message{message});
  message.user_id = yuzuru.id();
  message.text = "kawaii desu!!~~~";
  music.add_message(matrix::Message{message});

  client.add_room(std::move(music));
}


}  // Anonymous namespace


int main(int argc, char *argv[])
{
  //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

  QGuiApplication app(argc, argv);
  app.setWindowIcon(QIcon{"qrc:/img/res/img/icon.png"});

  axion::Configuration config;

  matrix::Client client{};
  add_dummy_content(client);

  RoomModel room_model{&client};
  RoomListModel room_list_model{&client};
  MemberListModel member_list_model{&client};
  room_model.set_room("!Xq3620DUiqCaoxq:example.com");
  member_list_model.set_room("!Xq3620DUiqCaoxq:example.com");

  // Engine takes ownership
  matrix::ImageProvider* image_provider = new matrix::ImageProvider{};

  matrix::AccessManager matrix{config, "http://matrix.org", &client, image_provider,
      &room_model, &room_list_model, &member_list_model};

  QQmlApplicationEngine engine;
  engine.addImageProvider(QLatin1String("matrix_media"), image_provider);
  engine.rootContext()->setContextProperty("config", &config);
  engine.rootContext()->setContextProperty("matrix", &matrix);
  engine.rootContext()->setContextProperty("roomModel", &room_model);
  engine.rootContext()->setContextProperty("roomListModel", &room_list_model);
  engine.rootContext()->setContextProperty("memberListModel", &member_list_model);
  engine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));

  auto* login_page = engine.rootObjects()[0]->findChild<QObject*>("loginPage");

  if (login_page) {
    QObject::connect(&matrix, &matrix::AccessManager::login_failure, &engine,
        [login_page](const auto& s){ login_page->setProperty("errorMessage", s); });
    QObject::connect(&matrix, &matrix::AccessManager::login_success, &engine,
        [login_page]{ login_page->setProperty("success", true); });
  }

  return app.exec();
}
