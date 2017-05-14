#ifndef MATRIX_CLIENT_H
#define MATRIX_CLIENT_H


#include <string>
#include <string_view>
#include <unordered_map>

#include "room.h"
#include "message.h"


namespace matrix
{


class Client
{
public:
  void add(matrix::Message&& message);
  void add(matrix::Room&& room);

  std::size_t room_count() const { return rooms_.size(); }

  Room* room(std::size_t i) { return i < room_count() ? &rooms_[i] : nullptr; }
  Room* room(std::string_view id);

private:
  std::string user_id_;
  std::vector<matrix::Room> rooms_;
};


}  // namespace matrix


#endif  // MATRIX_CLIENT_H
