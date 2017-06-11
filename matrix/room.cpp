#include "room.h"


#include <algorithm>
#include <regex>

#include "user.h"


std::size_t matrix::Room::index_of(const matrix::Message* message) const
{
  return std::distance(std::begin(messages_), std::find_if(std::begin(messages_),
      std::end(messages_), [message](const auto& m){ return &m == message; }));
}


matrix::User* matrix::Room::member(std::string_view id)
{
  auto it = std::find_if(std::begin(members_), std::end(members_), [id](const auto* u){
    return u->id() == id;
  });
  if (it != std::end(members_))
    return *it;

  return nullptr;
}
