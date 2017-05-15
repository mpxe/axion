#include "client.h"


#include <algorithm>


namespace
{


matrix::Room* find_room(std::vector<matrix::Room>& rooms, std::string_view id)
{
  auto it = std::find_if(std::begin(rooms), std::end(rooms), [id](const auto& r){
    return r.id() == id;
  });
  if (it != std::end(rooms))
    return &*it;
  return nullptr;
}


}  // Anonymous namespace


void matrix::Client::add(matrix::Message&& message)
{
  if (auto* r = find_room(rooms_, message.room_id); r)
    r->add_message(std::move(message));
}


void matrix::Client::add(matrix::Room&& room)
{
  rooms_.push_back(std::move(room));
}


matrix::Room* matrix::Client::room(std::string_view id)
{
  return find_room(rooms_, id);
}
