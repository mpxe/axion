#ifndef MATRIX_USER_H
#define MATRIX_USER_H


#include <string>


namespace matrix
{


class User
{
public:
  User(std::string&& user_id, std::string&& display_name);
  const std::string& id() const { return id_; }
  const std::string& account_name() const { return account_name_; }
  const std::string& display_name() const { return display_name_; }

private:
  std::string id_;
  std::string account_name_;
  std::string display_name_;
  int power_level_;
};


}  // namespace matrix


#endif  // MATRIX_USER_H
