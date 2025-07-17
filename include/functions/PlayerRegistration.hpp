#ifndef PLAYER_REGISTRATION_HPP
#define PLAYER_REGISTRATION_HPP

#include "dto/Player.hpp"
#include "structures/Queue.hpp"

class PlayerRegistration {
private:
    Queue<Player> registrationQueue;
    int nextPlayerId;

public:
    PlayerRegistration();

    void registerPlayer();
    void checkInPlayer();
    void displayQueue();
};

#endif
