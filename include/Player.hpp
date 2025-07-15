#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

class Player {
public:
    int id;
    std::string name;
    int age;
    char gender;
    std::string email;
    std::string phoneNum;
    int rank;
    std::string dateJoined;
    int performanceId; // pointer to Performance

    Player();
    Player(int id, const std::string& name, int age, char gender, const std::string& email,
           const std::string& phoneNum, int rank, const std::string& dateJoined, int performanceId);
};

#endif
