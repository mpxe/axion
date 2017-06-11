#ifndef UTIL_H
#define UTIL_H


#include <cstdint>
#include <string>
#include <string_view>


namespace util
{


bool starts_with(std::string_view sv, std::string_view token)
{
  if (sv.empty() || token.empty() || token.size() > sv.size())
    return false;

  auto a = std::begin(sv);
  auto b = std::begin(token);
  auto last = std::end(token);

  if (*a != *b)
    return false;

  while (b != last && *(++a) == *(++b));

  if (b == last)
    return true;

  return false;
}


inline std::tuple<std::string_view, std::string_view> split_first(std::string_view sv, char token)
{
  auto i = sv.find(token);
  if (i != sv.npos) {
    return {sv.substr(0, i), sv.substr(i+1)};
  }

  return {sv, std::string_view{}};
}


}  // namespace util


#endif  // UTIL_H
