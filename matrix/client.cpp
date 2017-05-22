#include "client.h"


#include <algorithm>


namespace
{


matrix::User* find_user(std::list<matrix::User>& users, std::string_view id)
{
  auto it = std::find_if(std::begin(users), std::end(users), [id](const auto& u){
    return u.id() == id;
  });
  if (it != std::end(users))
    return &*it;
  return nullptr;
}


matrix::Room* find_room(std::list<matrix::Room>& rooms, std::string_view id)
{
  auto it = std::find_if(std::begin(rooms), std::end(rooms), [id](const auto& r){
    return r.id() == id;
  });
  if (it != std::end(rooms))
    return &*it;
  return nullptr;
}


}  // Anonymous namespace


auto matrix::Client::room(std::size_t i) -> Room*
{
  auto it = std::begin(rooms_);
  std::advance(it, i);
  return &*it;
}


auto matrix::Client::user(std::string_view id) -> User*
{
  return find_user(users_, id);
}


auto matrix::Client::room(std::string_view id) -> Room*
{
  return find_room(rooms_, id);
}


auto matrix::Client::add_self(User&& self) -> User*
{
  users_.push_back(std::move(self));
  self_ = &users_.back();
  return self_;
}


auto matrix::Client::add_user(User&& user) -> User*
{
  users_.push_back(std::move(user));
  return &users_.back();
}


auto matrix::Client::add_room(Room&& room) -> Room*
{
  rooms_.push_back(std::move(room));
  return &rooms_.back();
}


void matrix::Client::add_message(Message&& message)
{
  if (auto* r = find_room(rooms_, message.room_id); r)
    r->add_message(std::move(message));
}
