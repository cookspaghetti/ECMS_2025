// SeatingManager.cpp
#include "functions/SeatingManager.hpp"
#include <iostream>

// ——————————————————————————————————————————————————————————
// Constructor
// ——————————————————————————————————————————————————————————
SeatingManager::SeatingManager(int /*numVipRows*/, size_t /*seatsPerVipRow*/,
                               int numInflRows, size_t seatsPerInflRow,
                               int numStreamRooms, size_t seatsPerStreamRoom,
                               size_t generalCap)
  : influencerCapacity(numInflRows * seatsPerInflRow),
    influencerOccupied(0),
    influencerSeating(influencerCapacity),
    numStreamRooms(numStreamRooms),
    seatsPerStreamRoom(seatsPerStreamRoom),
    streamerOccupied(0),
    generalSeating(generalCap),
    generalCapacity(generalCap),
    entryQueue(VIP_ROWS*SEATS_PER_ROW
               + influencerCapacity
               + numStreamRooms*seatsPerStreamRoom
               + generalCap),
    overflowQueue(VIP_ROWS*SEATS_PER_ROW
                  + influencerCapacity
                  + numStreamRooms*seatsPerStreamRoom
                  + generalCap)
{
    // VIP grid
    for (int r = 0; r < VIP_ROWS; ++r)
        for (int s = 0; s < SEATS_PER_ROW; ++s)
            vipSeats[r][s] = SeatPosition(r+1, s+1);

    // Streaming rooms: only first numStreamRooms get real capacity
    for (int i = 0; i < MAX_STREAMING_ROOMS; ++i) {
        if (i < numStreamRooms)
            streamingRooms[i] = StreamingRoom(i+1, seatsPerStreamRoom);
        else
            streamingRooms[i] = StreamingRoom(i+1, 0);
    }
}

// ——————————————————————————————————————————————————————————
// Enqueue into entry queue
// ——————————————————————————————————————————————————————————
void SeatingManager::addToEntryQueue(const Spectator &s, bool /*quiet*/) {
    entryQueue.enqueue(s);
}

// ——————————————————————————————————————————————————————————
// Process everyone waiting to be seated
// ——————————————————————————————————————————————————————————
void SeatingManager::processEntryQueue(bool verbose) {
    if (verbose) std::cout << "Processing entry queue...\n";
    while (!entryQueue.isEmpty()) {
        Spectator sp = entryQueue.dequeue();
        bool seated = false;

        switch (sp.type) {
          case SpectatorType::VIP:
            seated = assignVIPSeatPosition(sp, !verbose);
            break;
          case SpectatorType::Influencer:
            seated = assignInfluencerSeat(sp, !verbose);
            break;
          case SpectatorType::Streamer:
            seated = assignStreamingSeat(sp, !verbose);
            break;
          default:
            seated = assignGeneralSeat(sp, !verbose);
        }

        if (!seated) {
            overflowQueue.enqueue(sp);
            switch (sp.type) {
              case SpectatorType::VIP:        ++vipOverflow;        break;
              case SpectatorType::Influencer: ++infOverflow;        break;
              case SpectatorType::Streamer:   ++streamerOverflow;   break;
              default:                        ++generalOverflow;    break;
            }
            if (verbose)
                std::cout << "[Overflow] " << sp.name
                          << " (" << toString(sp.type) << ")\n";
        }
    }
}

// ——————————————————————————————————————————————————————————
// Print current seating & overflow stats
// ——————————————————————————————————————————————————————————
void SeatingManager::displaySeatingStatus() const {
    std::cout << "\n=== SEATING STATUS ===\n";

    std::cout << "Overflow Queue: " 
              << overflowQueue.size() << " waiting\n"
              << "  - VIP overflow: "        << vipOverflow        << "\n"
              << "  - Influencer overflow: " << infOverflow        << "\n"
              << "  - Streamer overflow: "   << streamerOverflow   << "\n"
              << "  - General overflow: "    << generalOverflow    << "\n\n";

    // VIP
    std::cout << "VIP Seating:\n";
    for (int r = 0; r < VIP_ROWS; ++r) {
        int cnt = 0;
        for (int s = 0; s < SEATS_PER_ROW; ++s)
            if (vipSeats[r][s].occupied) ++cnt;
        std::cout << "  Row " << (r+1) << ": " << cnt << "/" << SEATS_PER_ROW << "\n";
    }

    // Influencer
    std::cout << "\nInfluencer Seating: "
              << influencerOccupied << "/" << influencerCapacity << "\n";

    // Streaming rooms
    std::cout << "\nStreaming Rooms:\n";
    for (int i = 0; i < MAX_STREAMING_ROOMS; ++i) {
        int cnt = streamingRooms[i].viewers.size();
        std::cout << "  [Room " << (i+1) << "] "
                  << cnt << "/" << streamingRooms[i].capacity
                  << (i >= numStreamRooms ? " (inactive)" : "")
                  << "\n";
    }

    // General
    std::cout << "\nGeneral Admission: "
              << generalSeating.size() << "/" << generalCapacity << "\n\n";
}

// ——————————————————————————————————————————————————————————
// Helpers
// ——————————————————————————————————————————————————————————
SeatPosition SeatingManager::findNextVIPSeat() const {
    for (int r = 0; r < VIP_ROWS; ++r)
      for (int s = 0; s < SEATS_PER_ROW; ++s)
        if (!vipSeats[r][s].occupied)
          return SeatPosition(r+1, s+1);
    return SeatPosition();  // none available
}

bool SeatingManager::assignVIPSeatPosition(const Spectator &s, bool quiet) {
    SeatPosition p = findNextVIPSeat();
    if (p.row != -1) {
        auto &slot = vipSeats[p.row-1][p.seat-1];
        slot.occupied = true;
        slot.spectatorName = s.name;
        if (!quiet)
            std::cout << s.name
                      << " seated VIP at R" << p.row
                      << " S" << p.seat << "\n";
        return true;
    }
    return false;
}

bool SeatingManager::assignInfluencerSeat(const Spectator &s, bool quiet) {
    if (influencerOccupied < influencerCapacity) {
        influencerSeating.enqueue(s);
        ++influencerOccupied;
        if (!quiet)
            std::cout << s.name << " seated Influencer\n";
        return true;
    }
    return false;
}

bool SeatingManager::assignStreamingSeat(const Spectator &s, bool quiet) {
    if (!quiet) {
        std::cout << "[DEBUG] Assigning " << s.name << " to streaming room:\n";
        for (int i = 0; i < numStreamRooms; ++i)
            std::cout << "  Room " << (i+1) << ": "
                      << streamingRooms[i].viewers.size()
                      << "/" << streamingRooms[i].capacity
                      << " (full: "
                      << (streamingRooms[i].viewers.isFull() ? "yes" : "no")
                      << ")\n";
    }
    for (int i = 0; i < numStreamRooms; ++i) {
        if (!streamingRooms[i].viewers.isFull()) {
            streamingRooms[i].viewers.enqueue(s);
            if (!quiet)
                std::cout << s.name
                          << " seated in Stream Room " << (i+1)
                          << " (" << streamingRooms[i].viewers.size()
                          << "/" << streamingRooms[i].capacity
                          << " viewers)\n";
            return true;
        }
    }
    if (!quiet)
        std::cout << "[DEBUG] All " << numStreamRooms
                  << " streaming rooms are full, sending to overflow\n";
    return false;
}

bool SeatingManager::assignGeneralSeat(const Spectator &s, bool quiet) {
    if (!generalSeating.isFull()) {
        generalSeating.enqueue(s);
        if (!quiet)
            std::cout << s.name
                      << " seated General ("
                      << generalSeating.size() << "/"
                      << generalCapacity << ")\n";
        return true;
    }
    return false;
}
