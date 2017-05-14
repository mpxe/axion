#include "room.h"


#include <algorithm>
#include <regex>


matrix::User* matrix::Room::member(std::string_view id)
{
  auto it = std::find_if(std::begin(members_), std::end(members_), [id](const auto& u){
    return u.id() == id;
  });
  if (it != std::end(members_))
    return &*it;

  return nullptr;
}


std::string matrix::Room::name() const
{
  // Extract room name from room address, e.g. room from #room:example.com
  std::regex rx{R"(#(.+):)"};
  std::smatch m;
  if (std::regex_search(main_address_, m, rx))
    return m[1];
  return {};
}
