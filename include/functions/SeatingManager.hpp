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
#include "structures/CircularQueue.hpp"
#include "structures/DoublyLinkedList.hpp"

// Fixed layout constants
static constexpr int VIP_ROWS            = 5;  // 5 rooms × 10 seats = 50 VIP seats total
static constexpr int SEATS_PER_ROW       = 10;
static constexpr int MAX_STREAMING_ROOMS = 5;

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
      : roomId(id), capacity(cap), viewers(cap) {}
};

class SeatingManager {
public:
    // numVipRows × seatsPerVipRow (→ VIP_ROWS×SEATS_PER_ROW),
    // numInflRows × seatsPerInflencerRow,
    // numStreamRooms × seatsPerStreamRoom,
    // generalCapacity total general seats
    SeatingManager(int numVipRows,
                   size_t seatsPerVipRow,
                   int numInflRows,
                   size_t seatsPerInfluencerRow,
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
    void handleOverflowWithMoreSeats(const DoublyLinkedList<Spectator>& overflowSpectators);
    void handleOverflowReassignment(const DoublyLinkedList<Spectator>& overflowSpectators);
    void handleOverflowManual(const DoublyLinkedList<Spectator>& overflowSpectators);

private:
    // Helpers for each category
    SeatPosition findNextVIPSeat() const;
    bool assignVIPSeatPosition(const Spectator &s, bool quiet);
    bool assignInfluencerSeat(const Spectator &s, bool quiet);
    bool assignStreamingSeat(const Spectator &s, bool quiet);
    bool assignGeneralSeat(const Spectator &s, bool quiet);

    int vipOverflow     = 0;
    int infOverflow     = 0;
    int streamerOverflow= 0;
    int generalOverflow = 0;
    // VIP grid
    SeatPosition vipSeats[VIP_ROWS][SEATS_PER_ROW];

    // Influencer block
    int  influencerCapacity;
    int  influencerOccupied;
    Queue<Spectator> influencerSeating;

    // Streaming rooms
    int numStreamRooms;
    size_t seatsPerStreamRoom;
    StreamingRoom streamingRooms[MAX_STREAMING_ROOMS];
    int streamerOccupied;

    // General admission
    CircularQueue<Spectator> generalSeating;
    int generalCapacity;

    // Queues
    Queue<Spectator> entryQueue;
    Queue<Spectator> overflowQueue;
    
    // Manual tracking variables for enhanced seating management
    int generalOccupied;
    int overflowCount;
    std::string generalSeatIds[5][30]; // Room x Seat matrix for seat IDs
};


#endif // FUNCTIONS_SEATINGMANAGER_HPP
