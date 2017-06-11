#ifndef MATRIX_ROOM_H
#define MATRIX_ROOM_H


#include <string>
#include <string_view>
#include <vector>
#include <deque>

#include "message.h"


namespace matrix
{


class User;
enum class RoomState { Name };


class Room
{
public:
  Room() = default;
  Room(const std::string& id) : id_{id} {}

  const std::string& id() const { return id_; }
  const std::string& main_address() const { return main_address_; }
  const std::string& name() const { return name_; }

  std::size_t message_count() const { return messages_.size(); }
  std::size_t member_count() const { return members_.size(); }
  std::size_t index_of(const Message* message) const;

  Message* message(std::size_t i) { return i < message_count() ? &messages_[i] : nullptr; }
  Message* last_message() { return messages_.empty() ? nullptr : &messages_.front(); }
  User* member(std::size_t i) { return i < member_count() ? members_[i] : nullptr; }
  User* member(std::string_view id);

  void set_name(std::string&& name) { name_ = std::move(name); }
  void set_main_address(std::string&& main_address) { main_address_ = std::move(main_address); }

  void add_message(Message&& message) { messages_.push_front(std::move(message)); }
  void add_member(User* user) { members_.push_back(user); }

private:
  std::string id_;
  std::string name_;
  std::string main_address_;
  std::vector<std::string> local_addresses_;
  std::deque<Message> messages_;
  std::vector<User*> members_;
};


inline std::string as_mxstring(RoomState state)
{
  using namespace std::string_literals;
  switch (state) {
    case RoomState::Name: return "m.room.name"s;
    default: return ""s;
  }
}


}  // namespace matrix


#endif  // MATRIX_ROOM_H
