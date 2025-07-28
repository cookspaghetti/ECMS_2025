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
                  + generalCap),
    generalOccupied(0),  // Track general seating manually
    overflowCount(0)  // Track overflow manually
{
    // VIP grid - 5 rooms × 10 seats each = 50 VIP seats total
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

    // Initialize general seating arrays for 5 rooms × 30 seats each = 150 total
    for (int room = 0; room < 5; ++room) {
        for (int seat = 0; seat < 30; ++seat) {
            generalSeatIds[room][seat] = "";  // Empty means available
        }
    }
}

// ——————————————————————————————————————————————————————————
// Enqueue into entry queue
// ——————————————————————————————————————————————————————————
void SeatingManager::addToEntryQueue(const Spectator &s, bool /*quiet*/) {
    try {
        entryQueue.enqueue(s);
    } catch (...) {
        // Handle exception silently
    }
}

// ——————————————————————————————————————————————————————————
// Process everyone waiting to be seated
// ——————————————————————————————————————————————————————————
void SeatingManager::processEntryQueue(bool verbose) {
    if (verbose) std::cout << "Processing entry queue...\n";
    
    int processedCount = 0;
    while (!entryQueue.isEmpty()) {
        processedCount++;
        
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
            try {
                overflowQueue.enqueue(sp);
            } catch (...) {
                // Handle exception silently
            }
            overflowCount++;  // Update manual counter
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
    if (verbose) std::cout << "Finished processing " << processedCount << " spectators\n";
}

// ——————————————————————————————————————————————————————————
// Print current seating & overflow stats
// ——————————————————————————————————————————————————————————
void SeatingManager::displaySeatingStatus() const {
    std::cout << "\n=== ROOM-BY-ROOM SEATING LAYOUT ===\n";

    // Room 1 - VIP and Normal Section
    std::cout << "\n==== ROOM 1 ====\n";
    std::cout << "VIP: ";
    // Show all 10 VIP seats for Room 1 (row 0)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[0][s].occupied) {
            // Format as S00001, S00002, etc.
            char formattedId[10];
            sprintf(formattedId, "S%05d", vipSeats[0][s].spectatorId);
            std::cout << "[" << formattedId << "] ";
        } else {
            std::cout << "[empty] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs for VIP
    std::cout << "Seat IDs: ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        std::cout << "V1-" << (s+1);
        if ((s+1) < 10) std::cout << " ";
        std::cout << " ";
    }
    std::cout << "\n";

    std::cout << "\nNormal: ";
    // Show first 30 general seats (Room 1 portion)
    for (int seat = 0; seat < 30; ++seat) {
        if (!generalSeatIds[0][seat].empty()) {
            std::cout << "[" << generalSeatIds[0][seat] << "] ";
        } else {
            std::cout << "[empty] ";  // Empty Normal seat
        }
        if ((seat + 1) % 10 == 0) std::cout << "\n        ";  // New line every 10 seats
    }
    std::cout << "\n";
    
    // Show seat IDs for Room 1 general
    std::cout << "Seat IDs: ";
    for (int seat = 0; seat < 30; ++seat) {
        std::cout << "G1-" << (seat+1);
        if ((seat+1) < 10) std::cout << " ";
        std::cout << " ";
        if ((seat + 1) % 10 == 0) std::cout << "\n          ";
    }
    std::cout << "\n";

    // Room 2 - VIP and Normal Section
    std::cout << "\n==== ROOM 2 ====\n";
    std::cout << "VIP: ";
    // Show all 10 VIP seats for Room 2 (row 1)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[1][s].occupied) {
            // Format as S00001, S00002, etc.
            char formattedId[10];
            sprintf(formattedId, "S%05d", vipSeats[1][s].spectatorId);
            std::cout << "[" << formattedId << "] ";
        } else {
            std::cout << "[empty] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs for VIP
    std::cout << "Seat IDs: ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        std::cout << "V2-" << (s+1);
        if ((s+1) < 10) std::cout << " ";
        std::cout << " ";
    }
    std::cout << "\n";

    std::cout << "\nNormal: ";
    for (int seat = 0; seat < 30; ++seat) {
        if (!generalSeatIds[1][seat].empty()) {
            std::cout << "[" << generalSeatIds[1][seat] << "] ";
        } else {
            std::cout << "[empty] ";  // Empty Normal seat
        }
        if ((seat + 1) % 10 == 0) std::cout << "\n        ";
    }
    std::cout << "\n";
    
    std::cout << "Seat IDs: ";
    for (int seat = 0; seat < 30; ++seat) {
        std::cout << "G2-" << (seat+1);
        if ((seat+1) < 10) std::cout << " ";
        std::cout << " ";
        if ((seat + 1) % 10 == 0) std::cout << "\n          ";
    }
    std::cout << "\n";

    // Room 3 - VIP and Normal Section
    std::cout << "\n==== ROOM 3 ====\n";
    std::cout << "VIP: ";
    // Show all 10 VIP seats for Room 3 (row 2)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[2][s].occupied) {
            // Format as S00001, S00002, etc.
            char formattedId[10];
            sprintf(formattedId, "S%05d", vipSeats[2][s].spectatorId);
            std::cout << "[" << formattedId << "] ";
        } else {
            std::cout << "[empty] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs for VIP
    std::cout << "Seat IDs: ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        std::cout << "V3-" << (s+1);
        if ((s+1) < 10) std::cout << " ";
        std::cout << " ";
    }
    std::cout << "\n";

    std::cout << "\nNormal: ";
    for (int seat = 0; seat < 30; ++seat) {
        if (!generalSeatIds[2][seat].empty()) {
            std::cout << "[" << generalSeatIds[2][seat] << "] ";
        } else {
            std::cout << "[empty] ";  // Empty Normal seat
        }
        if ((seat + 1) % 10 == 0) std::cout << "\n        ";
    }
    std::cout << "\n";
    
    std::cout << "Seat IDs: ";
    for (int seat = 0; seat < 30; ++seat) {
        std::cout << "G3-" << (seat+1);
        if ((seat+1) < 10) std::cout << " ";
        std::cout << " ";
        if ((seat + 1) % 10 == 0) std::cout << "\n          ";
    }
    std::cout << "\n";

    // Room 4 - VIP and Normal Section
    std::cout << "\n==== ROOM 4 ====\n";
    std::cout << "VIP: ";
    // Show all 10 VIP seats for Room 4 (row 3)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[3][s].occupied) {
            // Format as S00001, S00002, etc.
            char formattedId[10];
            sprintf(formattedId, "S%05d", vipSeats[3][s].spectatorId);
            std::cout << "[" << formattedId << "] ";
        } else {
            std::cout << "[empty] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs for VIP
    std::cout << "Seat IDs: ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        std::cout << "V4-" << (s+1);
        if ((s+1) < 10) std::cout << " ";
        std::cout << " ";
    }
    std::cout << "\n";

    std::cout << "\nNormal: ";
    for (int seat = 0; seat < 30; ++seat) {
        if (!generalSeatIds[3][seat].empty()) {
            std::cout << "[" << generalSeatIds[3][seat] << "] ";
        } else {
            std::cout << "[empty] ";  // Empty Normal seat
        }
        if ((seat + 1) % 10 == 0) std::cout << "\n        ";
    }
    std::cout << "\n";
    
    std::cout << "Seat IDs: ";
    for (int seat = 0; seat < 30; ++seat) {
        std::cout << "G4-" << (seat+1);
        if ((seat+1) < 10) std::cout << " ";
        std::cout << " ";
        if ((seat + 1) % 10 == 0) std::cout << "\n          ";
    }
    std::cout << "\n";

    // Room 5 - VIP and Normal Section
    std::cout << "\n==== ROOM 5 ====\n";
    std::cout << "VIP: ";
    // Show all 10 VIP seats for Room 5 (row 4)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[4][s].occupied) {
            // Format as S00001, S00002, etc.
            char formattedId[10];
            sprintf(formattedId, "S%05d", vipSeats[4][s].spectatorId);
            std::cout << "[" << formattedId << "] ";
        } else {
            std::cout << "[empty] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs for VIP
    std::cout << "Seat IDs: ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        std::cout << "V5-" << (s+1);
        if ((s+1) < 10) std::cout << " ";
        std::cout << " ";
    }
    std::cout << "\n";

    std::cout << "\nNormal: ";
    for (int seat = 0; seat < 30; ++seat) {
        if (!generalSeatIds[4][seat].empty()) {
            std::cout << "[" << generalSeatIds[4][seat] << "] ";
        } else {
            std::cout << "[empty] ";  // Empty Normal seat
        }
        if ((seat + 1) % 10 == 0) std::cout << "\n        ";
    }
    std::cout << "\n";
    
    std::cout << "Seat IDs: ";
    for (int seat = 0; seat < 30; ++seat) {
        std::cout << "G5-" << (seat+1);
        if ((seat+1) < 10) std::cout << " ";
        std::cout << " ";
        if ((seat + 1) % 10 == 0) std::cout << "\n          ";
    }
    std::cout << "\n";

    // Influencer Section (separate area)
    std::cout << "\n==== INFLUENCER SECTION ====\n";
    std::cout << "Influencer Seating: " << influencerOccupied << "/" << influencerCapacity << " occupied\n";
    std::cout << "Note: Influencer seats are managed as a block seating area\n";

    // Streaming Rooms (separate areas)
    std::cout << "\n==== STREAMING ROOMS ====\n";
    for (int i = 0; i < numStreamRooms; ++i) {
        int currentViewers = streamingRooms[i].viewers.size();
        int capacity = streamingRooms[i].capacity;
        std::cout << "Stream Room " << (i+1) << ": ";
        
        if (currentViewers > 0) {
            std::cout << "OCCUPIED (" << currentViewers << "/" << capacity << " viewers)\n";
        } else {
            std::cout << "EMPTY (0/" << capacity << " viewers)\n";
        }
        std::cout << "Seat ID: SR" << (i+1) << "-1\n";
    }

    // Overflow Summary
    std::cout << "\n==== OVERFLOW QUEUE ====\n";
    std::cout << "Total waiting: " << overflowCount << "\n";
    std::cout << "  - VIP overflow: " << vipOverflow << "\n";
    std::cout << "  - Influencer overflow: " << infOverflow << "\n";
    std::cout << "  - Streamer overflow: " << streamerOverflow << "\n";
    std::cout << "  - General overflow: " << generalOverflow << "\n";
    
    std::cout << "\n";
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
        slot.spectatorId = s.id;  // Store the original numeric ID
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
        try {
            influencerSeating.enqueue(s);
        } catch (...) {
            return false;
        }
        
        ++influencerOccupied;
        if (!quiet)
            std::cout << s.name << " seated Influencer\n";
        return true;
    }
    return false;
}

bool SeatingManager::assignStreamingSeat(const Spectator &s, bool quiet) {
    for (int i = 0; i < numStreamRooms; ++i) {
        // Check if room's queue has capacity
        bool isFull = streamingRooms[i].viewers.isFull();
        
        if (!isFull) {
            // Create a copy of the spectator to avoid reference issues
            Spectator spectatorCopy = s;
            
            try {
                streamingRooms[i].viewers.enqueue(spectatorCopy);
                
                if (!quiet)
                    std::cout << s.name
                              << " seated in Stream Room " << (i+1)
                              << " (capacity: " << streamingRooms[i].capacity
                              << " viewers)\n";
                return true;
            } catch (const std::exception& e) {
                return false;
            } catch (...) {
                return false;
            }
        }
    }
    if (!quiet)
        std::cout << "All " << numStreamRooms
                  << " streaming rooms are full, sending to overflow\n";
    return false;
}

bool SeatingManager::assignGeneralSeat(const Spectator &s, bool quiet) {
    bool isFull = generalSeating.isFull();
    
    if (!isFull) {
        try {
            generalSeating.enqueue(s);
        } catch (...) {
            return false;
        }
        
        generalOccupied++;  // Update manual counter
        
        // Assign to seat ID tracking (distribute across rooms)
        int room = (generalOccupied - 1) / 30;  // 30 seats per room
        int seat = (generalOccupied - 1) % 30;  // seat within room
        
        if (room < 5) {  // Only if within our 5 rooms limit
            // Format as S00001, S00002, etc.
            char formattedId[10];
            sprintf(formattedId, "S%05d", s.id);
            generalSeatIds[room][seat] = formattedId;
        }
        
        if (!quiet)
            std::cout << s.name
                      << " seated General ("
                      << generalOccupied << "/"
                      << generalCapacity << ")\n";
        return true;
    }
    return false;
}

bool SeatingManager::hasOverflow() const {
    return !overflowQueue.isEmpty();
}

void SeatingManager::handleOverflow() {
    std::cout << "\n=== Processing Overflow Queue ===\n";
    
    // First, reject all streamers immediately
    std::vector<Spectator> nonStreamerOverflow;
    
    while (!overflowQueue.isEmpty()) {
        Spectator s = overflowQueue.dequeue();
        overflowCount--;  // Update manual counter when dequeuing
        
        if (s.type == SpectatorType::Streamer) {
            std::cout << "REJECTED: " << s.name << " (Streamer overflow not allowed - all streaming rooms full)\n";
            continue;
        } else {
            nonStreamerOverflow.push_back(s);
        }
    }
    
    if (nonStreamerOverflow.empty()) {
        std::cout << "All overflow spectators were streamers and have been rejected.\n";
        return;
    }
    
    std::cout << "\nNon-streamer overflow spectators: " << nonStreamerOverflow.size() << "\n";
    for (const auto& s : nonStreamerOverflow) {
        std::cout << "  - " << s.name << " (" << toString(s.type) << ")\n";
    }
    
    // Provide options for handling non-streamer overflow
    std::cout << "\n=== Overflow Handling Options ===\n";
    std::cout << "1. Add more seats automatically\n";
    std::cout << "2. Try to reassign to available spaces\n";
    std::cout << "3. Reject all overflow users\n";
    std::cout << "4. Manual seat addition\n";
    std::cout << "Select option (1-4): ";
    
    int choice;
    std::cin >> choice;
    
    switch (choice) {
        case 1:
            std::cout << "\n=== Adding More Seats Automatically ===\n";
            handleOverflowWithMoreSeats(nonStreamerOverflow);
            break;
        case 2:
            std::cout << "\n=== Trying to Reassign Overflow Spectators ===\n";
            handleOverflowReassignment(nonStreamerOverflow);
            break;
        case 3:
            std::cout << "\n=== Rejecting All Overflow Users ===\n";
            for (const auto& s : nonStreamerOverflow) {
                std::cout << "REJECTED: " << s.name << " (" << toString(s.type) << ")\n";
            }
            break;
        case 4:
            std::cout << "\n=== Manual Seat Addition ===\n";
            handleOverflowManual(nonStreamerOverflow);
            break;
        default:
            std::cout << "Invalid choice. Rejecting all overflow users.\n";
            for (const auto& s : nonStreamerOverflow) {
                std::cout << "REJECTED: " << s.name << " (" << toString(s.type) << ")\n";
            }
    }
}

void SeatingManager::addMoreSeats() {
    std::cout << "\n=== Adding More Seats Randomly ===\n";
    
    // Add some seats to streaming rooms that aren't full
    for (int i = 0; i < numStreamRooms; ++i) {
        if (!streamingRooms[i].viewers.isFull()) {
            // Try to add capacity (simplified - in real implementation would need dynamic allocation)
            std::cout << "Stream Room " << (i+1) << " has space for more viewers\n";
        }
    }
    
    // Add to general seating if not full
    if (!generalSeating.isFull()) {
        std::cout << "General seating has space for more spectators\n";
    }
    
    std::cout << "Random seat addition completed\n";
}

// Helper method for handling overflow with automatic seat addition
void SeatingManager::handleOverflowWithMoreSeats(const std::vector<Spectator>& overflowSpectators) {
    std::cout << "Attempting to expand seating capacity...\n";
    
    for (const auto& s : overflowSpectators) {
        bool assigned = false;
        
        // Try to assign to existing spaces first
        switch (s.type) {
            case SpectatorType::VIP:
                assigned = assignVIPSeatPosition(s, false);
                break;
            case SpectatorType::Influencer:
                assigned = assignInfluencerSeat(s, false);
                break;
            case SpectatorType::Normal:
                assigned = assignGeneralSeat(s, false);
                break;
            default:
                break;
        }
        
        if (!assigned) {
            // If general seating, try to add to general capacity
            if (s.type == SpectatorType::Normal && generalOccupied < 200) {  // Allow expansion up to 200
                generalCapacity = std::min(200, generalCapacity + 10);  // Expand by 10
                std::cout << "Expanded general seating capacity to " << generalCapacity << "\n";
                assigned = assignGeneralSeat(s, false);
            }
        }
        
        if (!assigned) {
            std::cout << "Could not assign even with expansion: " << s.name << "\n";
        }
    }
}

// Helper method for trying to reassign overflow spectators
void SeatingManager::handleOverflowReassignment(const std::vector<Spectator>& overflowSpectators) {
    for (const auto& s : overflowSpectators) {
        std::cout << "Processing overflow spectator: " << s.name 
                  << " (" << toString(s.type) << ")\n";
        
        bool assigned = false;
        switch (s.type) {
            case SpectatorType::VIP:
                assigned = assignVIPSeatPosition(s, false);
                break;
            case SpectatorType::Influencer:
                assigned = assignInfluencerSeat(s, false);
                break;
            case SpectatorType::Normal:
                assigned = assignGeneralSeat(s, false);
                break;
            default:
                break;
        }
        
        if (!assigned) {
            std::cout << "Could not assign overflow spectator: " << s.name << "\n";
        }
    }
}

// Helper method for manual overflow handling
void SeatingManager::handleOverflowManual(const std::vector<Spectator>& overflowSpectators) {
    std::cout << "Manual seat addition for " << overflowSpectators.size() << " spectators:\n";
    
    for (const auto& s : overflowSpectators) {
        std::cout << "\n--- " << s.name << " (" << toString(s.type) << ") ---\n";
        std::cout << "1. Try to assign to existing space\n";
        std::cout << "2. Add to general seating (expand capacity)\n";
        std::cout << "3. Reject this spectator\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                bool assigned = false;
                switch (s.type) {
                    case SpectatorType::VIP:
                        assigned = assignVIPSeatPosition(s, false);
                        break;
                    case SpectatorType::Influencer:
                        assigned = assignInfluencerSeat(s, false);
                        break;
                    case SpectatorType::Normal:
                        assigned = assignGeneralSeat(s, false);
                        break;
                    default:
                        break;
                }
                if (!assigned) {
                    std::cout << "Assignment failed for " << s.name << "\n";
                }
                break;
            }
            case 2:
                if (s.type == SpectatorType::Normal) {
                    generalCapacity = std::min(200, generalCapacity + 1);
                    std::cout << "Expanded general capacity to " << generalCapacity << "\n";
                    if (!assignGeneralSeat(s, false)) {
                        std::cout << "Assignment still failed for " << s.name << "\n";
                    }
                } else {
                    std::cout << "Can only expand general seating for normal spectators\n";
                }
                break;
            case 3:
                std::cout << "REJECTED: " << s.name << "\n";
                break;
            default:
                std::cout << "Invalid choice. REJECTED: " << s.name << "\n";
        }
    }
}

void SeatingManager::rejectOverflowUsers() {
    std::cout << "\n=== Rejecting Overflow Users ===\n";
    
    int rejectedCount = 0;
    while (!overflowQueue.isEmpty()) {
        Spectator s = overflowQueue.dequeue();
        overflowCount--;  // Update manual counter when dequeuing
        std::cout << "REJECTED: " << s.name << " (Type: " << static_cast<int>(s.type) << ")\n";
        rejectedCount++;
    }
    
    std::cout << "Total rejected users: " << rejectedCount << "\n";
}
