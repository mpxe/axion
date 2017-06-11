#ifndef MATRIX_MESSAGE_H
#define MATRIX_MESSAGE_H


#include <cstdint>
#include <string>
#include <string_view>
#include <map>


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
  bool deleted = false;
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


inline MessageType as_msgtype(std::string_view sv)
{
  static const std::map<std::string, matrix::MessageType, std::less<>> types{
    {"m.text", matrix::MessageType::Text},
    {"m.emote", matrix::MessageType::Emote},
    {"m.notice", matrix::MessageType::Notice},
    {"m.image", matrix::MessageType::Image},
    {"m.file", matrix::MessageType::File},
    {"m.location", matrix::MessageType::Location},
    {"m.video", matrix::MessageType::Video},
    {"m.audio", matrix::MessageType::Audio}
  };
  const auto it = types.find(sv);
  if (it != std::end(types))
    return it->second;
  return matrix::MessageType::Unknown;
}


inline std::string as_mxstring(MessageType type)
{
  static const std::map<matrix::MessageType, std::string> types{
    {matrix::MessageType::Text, "m.text"},
    {matrix::MessageType::Emote, "m.emote"},
    {matrix::MessageType::Notice, "m.notice"},
    {matrix::MessageType::Image, "m.image"},
    {matrix::MessageType::File, "m.file"},
    {matrix::MessageType::Location, "m.location"},
    {matrix::MessageType::Video, "m.video"},
    {matrix::MessageType::Audio, "m.audio"}
  };
  const auto it = types.find(type);
  if (it != std::end(types))
    return it->second;
  return "m.text";
}


}  // namespace matrix


#endif  // MATRIX_MESSAGE_H
