// File: functions/SpectatorRegistration.hpp
#ifndef FUNCTIONS_SPECTATORREGISTRATION_HPP
#define FUNCTIONS_SPECTATORREGISTRATION_HPP

#include <string>
#include "structures/DoublyLinkedList.hpp"
#include "dto/Spectator.hpp"
#include "functions/SeatingManager.hpp"

class SpectatorRegistration {
public:
    SpectatorRegistration();
    ~SpectatorRegistration();

    /// Loads all registered spectators from JSON
    void loadSpectatorsFromJSON(const std::string& filename);

    /// Loads check-in flags and immediately enqueues into SeatingManager
    void loadCheckIns(const std::string& filename);

    /// Manually add a new Spectator to the system
    void registerSpectator();

    /// Manually check-in one Spectator (and enqueue for seating)
    void checkInSpectator();

    /// Tell SeatingManager to process everyone enqueued so far
    void displayQueue();

private:
    DoublyLinkedList<Spectator> allSpectators;
    SeatingManager*             seatingManager;
    bool                        dataLoaded;
};

#endif // FUNCTIONS_SPECTATORREGISTRATION_HPP
