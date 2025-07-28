#ifndef SPECTATORREGISTRATION_HPP
#define SPECTATORREGISTRATION_HPP

#include <string>
#include "dto/Spectator.hpp"
#include "functions/SeatingManager.hpp"
#include "structures/CircularQueue.hpp"

class SpectatorRegistration {
public:
    SpectatorRegistration();
    ~SpectatorRegistration();

    // user-facing operations
    void registerSpectator();
    void checkInSpectator();
    void displayQueue();
    void displaySeatingStatus();
    bool hasWaitingSpectators() const;
    Spectator getNextSpectator();
    void implementEmergencySeating(int additionalCapacity);

private:
    // parsing helpers
    SpectatorType parseSpectatorType(const std::string& typeStr);
    Gender        parseGender(const std::string& genderStr);
    void          loadSpectatorsFromJSON();

    // internal state
    int                              nextSpectatorId;
    SeatingManager*                  seatingManager;
    CircularQueue<Spectator>         registrationQueue;
    bool                             dataLoaded;
};

#endif // SPECTATORREGISTRATION_HPP
