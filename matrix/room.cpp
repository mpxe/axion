#include "room.h"


#include <algorithm>
#include <regex>


matrix::User* matrix::Room::member(std::string_view id)
{
  auto it = std::find_if(std::begin(members_), std::end(members_), [id](const auto* u){
    return u->id() == id;
  });
  if (it != std::end(members_))
    return *it;

  return nullptr;
}
