#ifndef SPECTATOR_REGISTRATION_HPP
#define SPECTATOR_REGISTRATION_HPP

#include "../dto/Spectator.hpp"
#include "../structures/Queue.hpp"
#include <iostream>

// Forward declaration
class SeatingManager;

class SpectatorRegistration {
private:
    Queue<Spectator> registrationQueue;
    int nextSpectatorId;
    SeatingManager* seatingManager;
    
    void loadDummyData();  // Private method to load demo data

public:
    SpectatorRegistration();
    ~SpectatorRegistration();  // Destructor to clean up pointer

    void registerSpectator();
    void checkInSpectator();
    void displayQueue();
    void displaySeatingStatus();
    bool hasWaitingSpectators() const;
    Spectator getNextSpectator();
};

#endif
