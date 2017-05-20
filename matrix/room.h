#ifndef MATRIX_ROOM_H
#define MATRIX_ROOM_H


#include <string>
#include <string_view>
#include <vector>
#include <deque>

#include "message.h"
#include "user.h"


namespace matrix
{


class Room
{
public:
  Room() = default;
  Room(const std::string& id, const std::string& address) : id_{id}, main_address_{address} {}

  const std::string& id() const { return id_; }
  const std::string& main_address() const { return main_address_; }
  std::string name() const;

  std::size_t message_count() const { return messages_.size(); }
  std::size_t member_count() const { return members_.size(); }

  Message* message(std::size_t i) { return i < message_count() ? &messages_[i] : nullptr; }
  User* member(std::size_t i) { return i < member_count() ? &members_[i] : nullptr; }
  User* member(std::string_view id);

  void add_message(matrix::Message&& message) { messages_.push_front(std::move(message)); }
  void add_member(matrix::User&& user) { members_.push_back(std::move(user)); }

private:
  std::string id_;
  std::string main_address_;
  std::vector<std::string> local_addresses_;
  std::deque<matrix::Message> messages_;
  std::vector<matrix::User> members_;
};


}  // namespace matrix


#endif  // MATRIX_ROOM_H
