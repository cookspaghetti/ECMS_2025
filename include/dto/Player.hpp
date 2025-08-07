#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <ostream>
#include "general/Enum.hpp"

class Player {
public:
    std::string id;
    std::string name;
    int age;
    Gender gender;
    std::string email;
    std::string phoneNum;
    int points;
    bool isEarlyBird; // Indicates if the player registered early
    bool isWildcard; // Indicates if the player is a wildcard entry
    bool isLate; // Indicates if the player registered late
    std::string dateJoined;

    Player() : id(""), name(""), age(0), gender(Gender::Male), email(""), phoneNum(""), points(0), isEarlyBird(false), isWildcard(false), isLate(false), dateJoined("") {}
    Player(const std::string& id, const std::string& name, int age, Gender gender, const std::string& email, const std::string& phoneNum, int points, bool isEarlyBird, bool isWildcard, bool isLate, const std::string& dateJoined)
        : id(id), name(name), age(age), gender(gender), email(email), phoneNum(phoneNum), points(points), isEarlyBird(isEarlyBird), isWildcard(isWildcard), isLate(isLate), dateJoined(dateJoined) {}

    // Equality operator
    bool operator==(const Player& other) const {
        return id == other.id;
    }
    
    // Inequality operator
    bool operator!=(const Player& other) const {
        return !(*this == other);
    }

    // Function to display player information
    friend std::ostream& operator<<(std::ostream& os, const Player& player) {
        os << "ID: " << player.id
        << ", Name: " << player.name
        << ", Age: " << player.age
        << ", Gender: " << (player.gender == Gender::Male ? "Male" :
                            player.gender == Gender::Female ? "Female" : "Other")
        << ", Email: " << player.email
        << ", Phone: " << player.phoneNum
        << ", Points: " << player.points
        << ", Early Bird: " << (player.isEarlyBird ? "Yes" : "No")
        << ", Wildcard: " << (player.isWildcard ? "Yes" : "No")
        << ", Late Registration: " << (player.isLate ? "Yes" : "No")
        << ", Date Joined: " << player.dateJoined;
        return os;
    }

};

inline std::string genderToString(Gender gender) {
    switch (gender) {
        case Gender::Male:   
            return "Male";
        case Gender::Female: 
            return "Female";
        default:             
            return "Other";
    }
}

#endif
