// 5 streamers rooms
// 10 vip, 30 normal seats
// vip, influcencer, general, late comer
// vip : [A1], [id]
// [empty]
// bottom is streamer room (is diff with spectator room)
// streamer first row id, second row platform

#ifndef FUNCTIONS_SEATINGMANAGER_HPP
#define FUNCTIONS_SEATINGMANAGER_HPP

#include <iostream>
#include <string>
#include "dto/Spectator.hpp"
#include "structures/Queue.hpp"
#include "structures/PriorityQueue.hpp"
#include "structures/CircularQueue.hpp"
#include "structures/DoublyLinkedList.hpp"  // Needed for method signatures
#include "../helper/JsonLoader.hpp"

// Fixed layout constants
static constexpr int VIP_ROWS            = 5;  // 5 rooms × 10 seats = 50 VIP seats total
static constexpr int SEATS_PER_ROW       = 10;
static constexpr int MAX_STREAMING_ROOMS = 5;  // 5 streaming rooms as per original design

/// Holds a single VIP seat’s state
struct SeatPosition {
    int  row;
    int  seat;
    bool occupied;
    int spectatorId;  // Changed from spectatorName to spectatorId
    SeatPosition(int r=0,int s=0)
      : row(r), seat(s), occupied(false), spectatorId(0) {}
};

/// One streaming‐room container
struct StreamingRoom {
    int roomId;
    int capacity;
    CircularQueue<Spectator> viewers;
    StreamingRoom(int id=0,int cap=0)
      : roomId(id), capacity(cap), viewers(cap > 0 ? cap : 1) {}  // Ensure at least size 1
};

class SeatingManager {
public:
    // Constructor: vipCapacity, influencerCapacity, numStreamRooms, seatsPerStreamRoom, generalCapacity
    SeatingManager(int vipCapacity,
                   int influencerCapacity,
                   int numStreamRooms,
                   size_t seatsPerStreamRoom,
                   size_t generalCapacity);

    // Enqueue for later seating
    void addToEntryQueue(const Spectator &s, bool quiet);

    // Seat everyone, spilling into overflow only when all bins are full
    void processEntryQueue(bool verbose);

    // Print VIP rows, influencer block, each stream room, general & overflow
    void displaySeatingStatus() const;

    // Overflow management methods
    bool hasOverflow() const;
    void handleOverflow();
    void addMoreSeats();
    void rejectOverflowUsers();
    
    // Helper methods for overflow handling
    void handleOverflowWithMoreSeats(const DoublyLinkedList<Spectator>& vipOverflow,
                                    const DoublyLinkedList<Spectator>& influencerOverflow,
                                    const DoublyLinkedList<Spectator>& streamerOverflow,
                                    const DoublyLinkedList<Spectator>& normalOverflow);
    void handleOverflowReassignment(const DoublyLinkedList<Spectator>& vipOverflow,
                                   const DoublyLinkedList<Spectator>& influencerOverflow,
                                   const DoublyLinkedList<Spectator>& streamerOverflow,
                                   const DoublyLinkedList<Spectator>& normalOverflow);
    void handleOverflowManual(const DoublyLinkedList<Spectator>& vipOverflow,
                             const DoublyLinkedList<Spectator>& influencerOverflow,
                             const DoublyLinkedList<Spectator>& streamerOverflow,
                             const DoublyLinkedList<Spectator>& normalOverflow);
    void rejectAllOverflow(const DoublyLinkedList<Spectator>& vipOverflow,
                          const DoublyLinkedList<Spectator>& influencerOverflow,
                          const DoublyLinkedList<Spectator>& streamerOverflow,
                          const DoublyLinkedList<Spectator>& normalOverflow);
    
    // New drop and replace methods
    void handleDropAndReplace(const DoublyLinkedList<Spectator>& vipOverflow,
                             const DoublyLinkedList<Spectator>& influencerOverflow,
                             const DoublyLinkedList<Spectator>& streamerOverflow,
                             const DoublyLinkedList<Spectator>& normalOverflow);
    bool dropUserFromGeneralSeating(SpectatorType targetType);
    bool dropUserFromStreamingRoom();
    
    // Manual seat management helpers
    bool removeSpectatorById(int id);
    bool addSpectatorToAvailableSeat(const Spectator& spectator);
    bool assignSpectatorToSpecificSeat(const Spectator& spectator, int room, int seat);

private:
    // Helpers for each category
    bool assignVIPSeat(const Spectator &s, bool quiet);
    bool assignInfluencerSeat(const Spectator &s, bool quiet);
    bool assignStreamingSeat(const Spectator &s, bool quiet);
    bool assignGeneralSeat(const Spectator &s, bool quiet);
    bool assignGeneralSeatWithPriority(const Spectator &s, bool quiet, int priority);

    int vipOverflow     = 0;
    int infOverflow     = 0;
    int streamerOverflow= 0;
    int generalOverflow = 0;
    
    // Member variables ordered to match constructor initialization
    int vipCapacity;
    PriorityQueue<Spectator> vipSeating;
    // Display tracking for VIP seats (5 rooms × 10 seats each)
    SeatPosition vipSeats[VIP_ROWS][SEATS_PER_ROW];

    int influencerCapacity;
    int influencerOccupied;
    PriorityQueue<Spectator> influencerSeating;

    // Streaming rooms
    int numStreamRooms;
    size_t seatsPerStreamRoom;
    int streamerOccupied;
    StreamingRoom streamingRooms[MAX_STREAMING_ROOMS];

    // General admission with priority support
    PriorityQueue<Spectator> generalSeating;  // Changed from CircularQueue to PriorityQueue
    int generalCapacity;
    // Display tracking for general seats (5 rooms × 30 seats each)
    std::string generalSeatIds[5][30];

    // Queues
    PriorityQueue<Spectator> entryQueue;    // Priority queue for seat assignment
    CircularQueue<Spectator> overflowQueue; // CircularQueue for overflow handling
    
    // Manual tracking variables for enhanced seating management
    int generalOccupied;
    int overflowCount;
};


#endif // FUNCTIONS_SEATINGMANAGER_HPP
