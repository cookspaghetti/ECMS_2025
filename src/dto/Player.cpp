#include "Player.hpp"

Player::Player()
    : id(0), name(""), age(0), gender(Gender::Male),
      email(""), phoneNum(""), rank(0), dateJoined(""),
      performanceId(0) {}

Player::Player(int id, const std::string &name, int age, Gender gender,
               const std::string &email, const std::string &phoneNum,
               int rank, const std::string &dateJoined, int performanceId)
    : id(id), name(name), age(age), gender(gender),
      email(email), phoneNum(phoneNum), rank(rank),
      dateJoined(dateJoined), performanceId(performanceId) {}
