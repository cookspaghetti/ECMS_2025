// File: functions/SpectatorRegistration.hpp
#ifndef FUNCTIONS_SPECTATORREGISTRATION_HPP
#define FUNCTIONS_SPECTATORREGISTRATION_HPP

#include "structures/Queue.hpp"
#include "functions/SeatingManager.hpp"
#include "structures/DoublyLinkedList.hpp"
#include "dto/Spectator.hpp"

class SpectatorRegistration {
public:
    SpectatorRegistration();
    ~SpectatorRegistration();

    /// Register spectator - offers both JSON loading and manual entry options
    void registerSpectator();

    /// Loads everyone into the queue, assigns seats, then shows status.
    void displayQueue();

    /// Optional: manually dequeue/check-in one spectator.
    void checkInSpectator();

private:
    Queue<Spectator>            registrationQueue;
    SeatingManager*                         seatingManager;
    DoublyLinkedList<Spectator> waitingList;   // all spectators loaded at startup
    bool                                    processed;     // have we already assigned seats?

    // Helper method for loading JSON data
    void loadSpectatorsFromJSON();
};

#endif // FUNCTIONS_SPECTATORREGISTRATION_HPP

