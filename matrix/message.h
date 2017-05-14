#ifndef MATRIX_MESSAGE_H
#define MATRIX_MESSAGE_H


#include <cstdint>
#include <string>


namespace matrix
{


enum class MessageType
{
  Text, Emote, Notice, Image, File, Location, Video, Audio
};


struct Message
{
  std::string event_id;
  std::string room_id;
  std::string user_id;
  std::uint64_t timestamp = 0;
  std::string type;
  std::string text;
  bool transmit_confirmed = false;
  bool redacted = false;
};


}  // namespace matrix


#endif  // MATRIX_MESSAGE_H
