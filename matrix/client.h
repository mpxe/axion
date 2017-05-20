#ifndef MATRIX_CLIENT_H
#define MATRIX_CLIENT_H


#include <string>
#include <string_view>
#include <list>

#include "room.h"
#include "message.h"


namespace matrix
{


class Client
{
public:
  void set_user_id(std::string&& id) { user_id_ = std::move(id); }
  void add_message(matrix::Message&& message);
  void add_room(matrix::Room&& room);

  const std::string& user_id() { return user_id_; }
  std::size_t room_count() const { return rooms_.size(); }

  Room* room(std::size_t i);
  Room* room(std::string_view id);

private:
  std::string user_id_;
  std::list<matrix::Room> rooms_;
};


}  // namespace matrix


#endif  // MATRIX_CLIENT_H
