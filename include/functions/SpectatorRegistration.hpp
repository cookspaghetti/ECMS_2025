// File: functions/SpectatorRegistration.hpp
#ifndef FUNCTIONS_SPECTATORREGISTRATION_HPP
#define FUNCTIONS_SPECTATORREGISTRATION_HPP

#include "structures/Queue.hpp"
#include "functions/SeatingManager.hpp"
#include "structures/DoublyLinkedList.hpp"
#include "dto/Spectator.hpp"
#include <cstring>

// Simple struct to store check-in information
struct CheckInInfo {
    char spectatorId[10];  // e.g., "S00001"
    char checkInDateTime[25]; // e.g., "2025-01-15 08:30:00"
    
    CheckInInfo() {
        spectatorId[0] = '\0';
        checkInDateTime[0] = '\0';
    }
    
    CheckInInfo(const char* id, const char* dateTime) {
        strncpy(spectatorId, id, 9);
        spectatorId[9] = '\0';
        strncpy(checkInDateTime, dateTime, 24);
        checkInDateTime[24] = '\0';
    }
};

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
    bool                                    dataLoaded;    // track if JSON data has been loaded
    DoublyLinkedList<CheckInInfo>           checkedInSpectators; // check-in data

    // Helper method for loading JSON data
    void loadSpectatorsFromJSON();
    
    // Helper method to load check-in data
    void loadCheckInData();
    
    // Helper method to find check-in info for a spectator
    CheckInInfo* findCheckInInfo(const char* spectatorId);
};

#endif // FUNCTIONS_SPECTATORREGISTRATION_HPP

