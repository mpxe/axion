#ifndef MATRIX_MESSAGE_H
#define MATRIX_MESSAGE_H


#include <cstdint>
#include <string>


namespace matrix
{


enum class MessageType
{
  Text, Emote, Notice, Image, File, Location, Video, Audio, Unknown
};


struct Message
{
  bool transmit_confirmed = false;
  bool transmit_failed = false;
  bool redacted = false;
  bool content_loading = false;
  MessageType type;
  std::uint64_t timestamp = 0;
  std::string event_id;
  std::string room_id;
  std::string user_id;
  std::string text;
  std::string url;
  std::string image_id;
};


}  // namespace matrix


#endif  // MATRIX_MESSAGE_H
