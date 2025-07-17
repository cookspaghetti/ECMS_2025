#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <ostream>
#include "general/Enum.hpp"

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

    // Function to display player information
    friend std::ostream& operator<<(std::ostream& os, const Player& player) {
        os << "ID: " << player.id
        << ", Name: " << player.name
        << ", Age: " << player.age
        << ", Gender: " << (player.gender == Gender::Male ? "Male" :
                            player.gender == Gender::Female ? "Female" : "Other")
        << ", Email: " << player.email
        << ", Phone: " << player.phoneNum
        << ", Rank: " << player.rank
        << ", Wildcard: " << (player.isWildcard ? "Yes" : "No")
        << ", Date Joined: " << player.dateJoined
        << ", Performance ID: " << player.performanceId;
        return os;
    }

};

#endif
