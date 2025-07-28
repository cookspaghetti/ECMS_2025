#ifndef FUNCTIONS_SEATINGMANAGER_HPP
#define FUNCTIONS_SEATINGMANAGER_HPP

#include <iostream>
#include <string>
#include "dto/Spectator.hpp"
#include "structures/Queue.hpp"
#include "structures/CircularQueue.hpp"

// Fixed layout constants
static constexpr int VIP_ROWS            = 3;
static constexpr int SEATS_PER_ROW       = 10;
static constexpr int MAX_STREAMING_ROOMS = 5;

/// Holds a single VIP seat’s state
struct SeatPosition {
    int  row;
    int  seat;
    bool occupied;
    std::string spectatorName;
    SeatPosition(int r=0,int s=0)
      : row(r), seat(s), occupied(false), spectatorName("") {}
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
};


#endif // FUNCTIONS_SEATINGMANAGER_HPP
