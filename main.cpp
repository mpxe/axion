#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QIcon>

#include "matrix/accessmanager.h"
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
  matrix::Room music{"!Xq3620DUiqCaoxq:example.com", "#music:example.com"};
  matrix::Room physics{"!636q39766251:example.com", "#spacetime:example.com"};

  matrix::User miku{"@miku:example.com", u8"初音ミク"};
  matrix::User yuzuru{"@yuzuru:example.com", "yuzuru"};
  matrix::User krauss{"@krauss:example.com", "Lawrence Krauss"};
  matrix::User smolin{"@smolin:example.com", "Lee Smolin"};

  music.add_member(matrix::User{"@self:example.com", u8"ಠ_ಠ"});
  music.add_member(matrix::User{miku});
  music.add_member(matrix::User{yuzuru});
  physics.add_member(matrix::User{"@self:example.com", u8"ಠ_ಠ"});
  physics.add_member(matrix::User{krauss});
  physics.add_member(matrix::User{smolin});

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

  message.room_id = physics.id();
  message.user_id = smolin.id();
  message.text = "Whatever is happening on very small scales near the horizon of the black hole "
      "will be enlarged by the effect whereby the wavelengths of light are stretched as the light "
      "climbs up to us. This means that jf we can observe light coming from very close to the "
      "horizon of a black hole, we may be able to see the quantum structure of space itself.";
  physics.add_message(matrix::Message{message});
  message.user_id = krauss.id();
  message.text = "If you have nothing in quantum mechanics, you will always have something.";
  physics.add_message(matrix::Message{message});

  client.add(std::move(music));
  client.add(std::move(physics));
}


}  // Anonymous namespace


int main(int argc, char *argv[])
{
  //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

  QGuiApplication app(argc, argv);
  app.setWindowIcon(QIcon{"qrc:/img/res/img/icon.png"});

  matrix::AccessManager matrix;
  matrix::Client client{};
  add_dummy_content(client);

  RoomModel room_model{client};
  RoomListModel room_list_model{client};
  MemberListModel member_list_model{client};
  room_model.set_room("!636q39766251:example.com");
  member_list_model.set_room("!636q39766251:example.com");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));
  engine.rootContext()->setContextProperty("matrix", &matrix);
  engine.rootContext()->setContextProperty("roomModel", &room_model);
  engine.rootContext()->setContextProperty("roomListModel", &room_list_model);
  engine.rootContext()->setContextProperty("memberListModel", &member_list_model);

  auto* login_page = engine.rootObjects()[0]->findChild<QObject*>("loginPage");

  if (login_page) {
    QObject::connect(&matrix, &matrix::AccessManager::login_failure, &engine,
        [login_page](const auto& s){ login_page->setProperty("errorMessage", s); });
    QObject::connect(&matrix, &matrix::AccessManager::login_success, &engine,
        [login_page]{ login_page->setProperty("success", true); });
  }

  return app.exec();
}
