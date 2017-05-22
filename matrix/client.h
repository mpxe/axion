#ifndef MATRIX_CLIENT_H
#define MATRIX_CLIENT_H


#include <string>
#include <string_view>
#include <list>

#include "user.h"
#include "room.h"
#include "message.h"


namespace matrix
{


class Client
{
public:
  User* self() { return self_; }
  const std::string& user_id() const { return self_->id(); }
  std::size_t room_count() const { return rooms_.size(); }

  auto user(std::string_view id) -> User*;
  auto room(std::size_t i) -> Room*;
  auto room(std::string_view id) -> Room*;

  auto add_self(User&& self) -> User*;
  auto add_user(User&& user) -> User*;
  auto add_room(Room&& room) -> Room*;
  void add_message(Message&& message);

private:
  User* self_;
  std::list<Room> rooms_;
  std::list<User> users_;
};


}  // namespace matrix


#endif  // MATRIX_CLIENT_H
