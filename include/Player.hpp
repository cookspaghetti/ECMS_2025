#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

enum class Gender {
    Male,
    Female
};

class Player {
public:
    int id;
    std::string name;
    int age;
    Gender gender;
    std::string email;
    std::string phoneNum;
    int rank;
    std::string dateJoined;
    int performanceId; // ID of associated performance record

    Player();
    Player(int id, const std::string& name, int age, Gender gender,
           const std::string& email, const std::string& phoneNum,
           int rank, const std::string& dateJoined, int performanceId);
};

#endif
