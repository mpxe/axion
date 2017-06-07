#ifndef UTIL_H
#define UTIL_H


#include <cstdint>
#include <string>
#include <string_view>


namespace util
{


bool starts_with(const std::string_view text, const std::string_view token)
{
  if (text.empty() || token.empty() || token.size() > text.size())
    return false;

  auto a = std::begin(text);
  auto b = std::begin(token);
  auto last = std::end(token);

  if (*a != *b)
    return false;

  while (b != last && *(++a) == *(++b));

  if (b == last)
    return true;

  return false;
}


}  // namespace util


#endif  // UTIL_H
