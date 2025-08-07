#ifndef PLAYER_PAIR_HPP
#define PLAYER_PAIR_HPP

#include "../dto/Player.hpp"

struct PlayerPair {
    Player first;
    Player second;
    
    // Default constructor
    PlayerPair() {}
    
    // Constructor with two players
    PlayerPair(const Player& p1, const Player& p2) : first(p1), second(p2) {}
    
    // Copy constructor
    PlayerPair(const PlayerPair& other) : first(other.first), second(other.second) {}
    
    // Assignment operator
    PlayerPair& operator=(const PlayerPair& other) {
        if (this != &other) {
            first = other.first;
            second = other.second;
        }
        return *this;
    }
    
    // Equality operator
    bool operator==(const PlayerPair& other) const {
        return (first == other.first && second == other.second) ||
               (first == other.second && second == other.first);
    }
    
    // Inequality operator
    bool operator!=(const PlayerPair& other) const {
        return !(*this == other);
    }
    
    // Check if a player is in this pair
    bool contains(const Player& player) const {
        return first == player || second == player;
    }
    
    // Get the other player in the pair
    Player getOther(const Player& player) const {
        if (first == player) {
            return second;
        } else if (second == player) {
            return first;
        }
        // Return empty player if not found
        return Player();
    }
    
    // Swap the order of players
    void swap() {
        Player temp = first;
        first = second;
        second = temp;
    }
};

#endif
