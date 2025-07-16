#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include "Enum.hpp"

class Player {
public:
    int id;
    std::string name;
    int age;
    Gender gender;
    std::string email;
    std::string phoneNum;
    int rank;
    bool isWildcard; // Indicates if the player is a wildcard entry
    std::string dateJoined;
    int performanceId; // ID of associated performance record

    Player() : id(0), name(""), age(0), gender(Gender::Male), email(""), phoneNum(""), rank(0), isWildcard(false), dateJoined(""), performanceId(0) {}
    Player(int id, const std::string& name, int age, Gender gender, const std::string& email, const std::string& phoneNum, int rank, bool isWildcard, const std::string& dateJoined, int performanceId) 
        : id(id), name(name), age(age), gender(gender), email(email), phoneNum(phoneNum), rank(rank), isWildcard(isWildcard), dateJoined(dateJoined), performanceId(performanceId) {}
};

#endif
