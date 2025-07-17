#ifndef SEATINGMANAGER_HPP
#define SEATINGMANAGER_HPP

#include "Spectator.hpp"
#include "PriorityQueue.hpp"
#include "Queue.hpp"
#include "CircularQueue.hpp"
#include <iostream>

class SeatingManager {
private:
    // VIP Section
    Queue<Spectator> vipSeating;
    int vipCapacity;
    int vipOccupied;
    
    // Influencer Section
    Queue<Spectator> influencerSeating;
    int influencerCapacity;
    int influencerOccupied;
    
    // Streaming Setup Area
    Queue<Spectator> streamingArea;
    int streamingCapacity;
    int streamingOccupied;
    
    // General Admission with overflow management
    CircularQueue<Spectator> generalSeating;
    Queue<Spectator> overflowQueue;
    int generalCapacity;
    
    // Priority queue for managing entry based on spectator type
    PriorityQueue<Spectator> entryQueue;

public:
    SeatingManager(int vipCap, int influencerCap, int streamingCap, int generalCap);
    
    // Seating assignment methods
    bool assignSeat(const Spectator& spectator);
    bool assignVIPSeat(const Spectator& spectator);
    bool assignInfluencerSeat(const Spectator& spectator);
    bool assignStreamingSeat(const Spectator& spectator);
    bool assignGeneralSeat(const Spectator& spectator);
    
    // Queue management
    void addToEntryQueue(const Spectator& spectator);
    void processEntryQueue();
    void manageOverflow();
    
    // Utility methods
    void displaySeatingStatus() const;
    void displayOverflowStatus() const;
    int getAvailableSeats(SpectatorType type) const;
    bool hasAvailableSeats(SpectatorType type) const;
    
    // Priority calculation for different spectator types
    int calculatePriority(SpectatorType type) const;
};

// Constructor
SeatingManager::SeatingManager(int vipCap, int influencerCap, int streamingCap, int generalCap) 
    : vipSeating(vipCap), vipCapacity(vipCap), vipOccupied(0),
      influencerSeating(influencerCap), influencerCapacity(influencerCap), influencerOccupied(0),
      streamingArea(streamingCap), streamingCapacity(streamingCap), streamingOccupied(0),
      generalSeating(generalCap), overflowQueue(generalCap * 2), generalCapacity(generalCap),
      entryQueue(generalCap + vipCap + influencerCap + streamingCap) {}

// Calculate priority based on spectator type
int SeatingManager::calculatePriority(SpectatorType type) const {
    switch (type) {
        case SpectatorType::VIP: return 10;
        case SpectatorType::Influencer: return 8;
        case SpectatorType::Streamer: return 7;
        case SpectatorType::Player: return 9;  // Players get high priority
        case SpectatorType::Normal: return 1;
        default: return 0;
    }
}

// Add spectator to entry queue with priority
void SeatingManager::addToEntryQueue(const Spectator& spectator) {
    int priority = calculatePriority(spectator.type);
    entryQueue.enqueue(spectator, priority);
    std::cout << "Added " << spectator.name << " (" << toString(spectator.type) 
              << ") to entry queue with priority " << priority << std::endl;
}

// Main seating assignment logic
bool SeatingManager::assignSeat(const Spectator& spectator) {
    switch (spectator.type) {
        case SpectatorType::VIP:
            return assignVIPSeat(spectator);
        case SpectatorType::Influencer:
            return assignInfluencerSeat(spectator);
        case SpectatorType::Streamer:
            return assignStreamingSeat(spectator);
        case SpectatorType::Player:
            // Players can use VIP seating if available, otherwise general
            return assignVIPSeat(spectator) || assignGeneralSeat(spectator);
        case SpectatorType::Normal:
            return assignGeneralSeat(spectator);
        default:
            return assignGeneralSeat(spectator);
    }
}

// VIP seating assignment
bool SeatingManager::assignVIPSeat(const Spectator& spectator) {
    if (vipOccupied < vipCapacity) {
        vipSeating.enqueue(spectator);
        vipOccupied++;
        std::cout << "Assigned VIP seat to " << spectator.name << std::endl;
        return true;
    }
    std::cout << "VIP section full. Adding " << spectator.name << " to overflow." << std::endl;
    overflowQueue.enqueue(spectator);
    return false;
}

// Influencer seating assignment
bool SeatingManager::assignInfluencerSeat(const Spectator& spectator) {
    if (influencerOccupied < influencerCapacity) {
        influencerSeating.enqueue(spectator);
        influencerOccupied++;
        std::cout << "Assigned influencer seat to " << spectator.name 
                  << " (Platform: " << spectator.affiliation << ")" << std::endl;
        return true;
    }
    std::cout << "Influencer section full. Adding " << spectator.name << " to overflow." << std::endl;
    overflowQueue.enqueue(spectator);
    return false;
}

// Streaming area assignment
bool SeatingManager::assignStreamingSeat(const Spectator& spectator) {
    if (streamingOccupied < streamingCapacity) {
        streamingArea.enqueue(spectator);
        streamingOccupied++;
        std::cout << "Assigned streaming setup to " << spectator.name 
                  << " (Platform: " << spectator.affiliation << ")" << std::endl;
        return true;
    }
    std::cout << "Streaming area full. Adding " << spectator.name << " to overflow." << std::endl;
    overflowQueue.enqueue(spectator);
    return false;
}

// General seating assignment
bool SeatingManager::assignGeneralSeat(const Spectator& spectator) {
    if (!generalSeating.isFull()) {
        generalSeating.enqueue(spectator);
        std::cout << "Assigned general seat to " << spectator.name << std::endl;
        return true;
    }
    std::cout << "General seating full. Adding " << spectator.name << " to overflow." << std::endl;
    overflowQueue.enqueue(spectator);
    return false;
}

// Process the entry queue based on priority
void SeatingManager::processEntryQueue() {
    std::cout << "\n=== Processing Entry Queue ===\n";
    while (!entryQueue.isEmpty()) {
        Spectator spectator = entryQueue.dequeue();
        assignSeat(spectator);
    }
}

// Manage overflow by checking for available seats
void SeatingManager::manageOverflow() {
    std::cout << "\n=== Managing Overflow ===\n";
    Queue<Spectator> tempQueue(overflowQueue.size());
    
    // Process overflow queue
    while (!overflowQueue.isEmpty()) {
        Spectator spectator = overflowQueue.dequeue();
        if (!assignSeat(spectator)) {
            tempQueue.enqueue(spectator);  // Still can't seat, keep in overflow
        }
    }
    
    // Put remaining overflow back
    overflowQueue = tempQueue;
}

// Display current seating status
void SeatingManager::displaySeatingStatus() const {
    std::cout << "\n=== SEATING STATUS ===\n";
    std::cout << "VIP Section: " << vipOccupied << "/" << vipCapacity << " occupied\n";
    std::cout << "Influencer Section: " << influencerOccupied << "/" << influencerCapacity << " occupied\n";
    std::cout << "Streaming Area: " << streamingOccupied << "/" << streamingCapacity << " occupied\n";
    std::cout << "General Seating: " << generalSeating.size() << "/" << generalCapacity << " occupied\n";
    std::cout << "Overflow Queue: " << overflowQueue.size() << " waiting\n";
}

// Display overflow status
void SeatingManager::displayOverflowStatus() const {
    std::cout << "\n=== OVERFLOW STATUS ===\n";
    std::cout << "People in overflow queue: " << overflowQueue.size() << std::endl;
    if (overflowQueue.isEmpty()) {
        std::cout << "No overflow - all spectators seated!\n";
    } else {
        std::cout << "Overflow management active - monitoring for available seats\n";
    }
}

// Get available seats for a specific type
int SeatingManager::getAvailableSeats(SpectatorType type) const {
    switch (type) {
        case SpectatorType::VIP:
        case SpectatorType::Player:
            return vipCapacity - vipOccupied;
        case SpectatorType::Influencer:
            return influencerCapacity - influencerOccupied;
        case SpectatorType::Streamer:
            return streamingCapacity - streamingOccupied;
        case SpectatorType::Normal:
            return generalCapacity - generalSeating.size();
        default:
            return 0;
    }
}

// Check if seats are available for a type
bool SeatingManager::hasAvailableSeats(SpectatorType type) const {
    return getAvailableSeats(type) > 0;
}

#endif
