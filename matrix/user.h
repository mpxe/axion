#ifndef MATRIX_USER_H
#define MATRIX_USER_H


#include <string>


namespace matrix
{


class User
{
public:
  User(std::string&& user_id, std::string&& display_name = "");

  const std::string& id() const { return id_; }
  const std::string& account_name() const { return account_name_; }
  const std::string& display_name() const { return display_name_; }
  const std::string& avatar_url() const { return avatar_url_; }

  void set_display_name(std::string&& name) { display_name_ = std::move(name); }
  void set_avatar_url(std::string&& url) { avatar_url_ = std::move(url); }

private:
  std::string id_;
  std::string account_name_;
  std::string display_name_;
  std::string avatar_url_;
};


}  // namespace matrix


#endif  // MATRIX_USER_H
