// SeatingManager.cpp
#include "functions/SeatingManager.hpp"
#include <iostream>
#include <iomanip>

// Helper function to center text within a fixed width
std::string centerText(const std::string& text, int width) {
    if (text.length() >= static_cast<size_t>(width)) return text;
    int padding = width - static_cast<int>(text.length());
    int leftPad = padding / 2;
    int rightPad = padding - leftPad;
    return std::string(leftPad, ' ') + text + std::string(rightPad, ' ');
}

// ——————————————————————————————————————————————————————————
// Constructor
// ——————————————————————————————————————————————————————————
SeatingManager::SeatingManager(int vipCapacity,
                               int influencerCapacity,
                               int numStreamRooms,
                               size_t seatsPerStreamRoom,
                               size_t generalCapacity)
  : vipCapacity(vipCapacity),
    vipSeating(vipCapacity),
    influencerCapacity(influencerCapacity),
    influencerOccupied(0),
    influencerSeating(influencerCapacity),
    numStreamRooms(numStreamRooms),
    seatsPerStreamRoom(seatsPerStreamRoom),
    streamerOccupied(0),
    generalSeating(generalCapacity),
    generalCapacity(generalCapacity),
    entryQueue(500),  // Increased capacity to handle more spectators
    overflowQueue(500),  // Increased overflow queue capacity as well
    generalOccupied(0),
    overflowCount(0)
{
    // Initialize VIP seat tracking arrays
    for (int r = 0; r < VIP_ROWS; ++r)
        for (int s = 0; s < SEATS_PER_ROW; ++s)
            vipSeats[r][s] = SeatPosition(r+1, s+1);

    // Initialize general seat tracking arrays
    for (int room = 0; room < 5; ++room) {
        for (int seat = 0; seat < 30; ++seat) {
            generalSeatIds[room][seat] = "";  // Empty means available
        }
    }

    // Streaming rooms: only first numStreamRooms get real capacity
    // Force capacity to 1 for overflow testing
    for (int i = 0; i < MAX_STREAMING_ROOMS; ++i) {
        if (i < numStreamRooms)
            streamingRooms[i] = StreamingRoom(i+1, 1);  // Force 1 seat per room for overflow testing
        else
            streamingRooms[i] = StreamingRoom(i+1, 0);
    }
}

// ——————————————————————————————————————————————————————————
// Get priority for spectator types (higher number = higher priority)
// ——————————————————————————————————————————————————————————
int getSpectatorPriority(SpectatorType type) {
    switch (type) {
        case SpectatorType::VIP:        return 5;  // Highest priority
        case SpectatorType::Influencer: return 4;
        case SpectatorType::Streamer:   return 3;
        case SpectatorType::Normal:     return 2;
        case SpectatorType::Player:     return 1;  // Lowest priority
        default:                        return 0;
    }
}

// ——————————————————————————————————————————————————————————
// Enqueue into entry queue with priority
// ——————————————————————————————————————————————————————————
void SeatingManager::addToEntryQueue(const Spectator &s, bool /*quiet*/) {
    try {
        int priority = getSpectatorPriority(s.type);
        
        if (entryQueue.isFull()) {
            return;
        }
        
        entryQueue.enqueue(s, priority);
    } catch (const std::exception& e) {
        std::cout << " Exception in addToEntryQueue for " << s.name << ": " << e.what() << "\n";
    } catch (...) {
        std::cout << " Unknown exception in addToEntryQueue for " << s.name << "\n";
    }
}

// ——————————————————————————————————————————————————————————
// Process everyone waiting to be seated
// ——————————————————————————————————————————————————————————
void SeatingManager::processEntryQueue(bool verbose) {
    std::cout << "Processing spectators for seat assignments...\n";
    
    int processedCount = 0;
    while (!entryQueue.isEmpty()) {
        processedCount++;
        
        Spectator sp = entryQueue.dequeue();
        
        bool seated = false;

        switch (sp.type) {
          case SpectatorType::VIP:
            seated = assignVIPSeat(sp, !verbose);
            if (!seated) {
                // VIP fallback to general seating with high priority
                seated = assignGeneralSeatWithPriority(sp, !verbose, 5); // Highest priority
            }
            break;
          case SpectatorType::Influencer:
            seated = assignInfluencerSeat(sp, !verbose);
            if (!seated) {
                // Influencer fallback to general seating with medium priority
                seated = assignGeneralSeatWithPriority(sp, !verbose, 4);
            }
            break;
          case SpectatorType::Streamer:
            seated = assignStreamingSeat(sp, !verbose);
            // Streamers should ONLY go to streaming rooms, no fallback to general seating
            break;
          default:
            seated = assignGeneralSeat(sp, !verbose);
        }

        if (!seated) {
            // Handle overflow for different spectator types
            if (sp.type == SpectatorType::Normal) {
                try {
                    overflowQueue.enqueue(sp);
                } catch (...) {
                    // Handle exception silently
                }
                overflowCount++;  // Update manual counter
                ++generalOverflow;
            } else if (sp.type == SpectatorType::Streamer) {
                // Streamer overflow should only happen if both streaming rooms and general seating are full
                try {
                    overflowQueue.enqueue(sp);
                } catch (...) {
                    // Handle exception silently
                }
                overflowCount++;  // Update manual counter
                ++streamerOverflow;
            }
            // VIP and Influencer overflow should not happen with proper fallback to general seating
        }
    }
    std::cout << "Completed processing " << processedCount << " spectators.\n";
}

// ——————————————————————————————————————————————————————————
// Print current seating & overflow stats
// ——————————————————————————————————————————————————————————
void SeatingManager::displaySeatingStatus() const {
    std::cout << "\n=== ROOM-BY-ROOM SEATING LAYOUT ===\n";

    // Room 1 - VIP and Normal Section
    std::cout << "\n==== ROOM 1 ====\n";
    std::cout << "VIP     : ";
    // Show all 10 VIP seats for Room 1 (row 0)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[0][s].occupied) {
            // Format as SV00001, SV00002, etc. for VIP
            char formattedId[10];
            sprintf(formattedId, "SV%05d", vipSeats[0][s].spectatorId);
            std::cout << "[" << centerText(formattedId, 8) << "] ";
        } else {
            std::cout << "[" << centerText("empty", 8) << "] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs directly below spectator IDs
    std::cout << "          ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        char seatLabel[10];
        sprintf(seatLabel, "V1-%d", s+1);
        std::cout << "[" << centerText(seatLabel, 8) << "] ";
    }
    std::cout << "\n";

    std::cout << "\nNormal  : ";
    // Show first 30 general seats (Room 1 portion) - 10 per row with seat IDs below each row
    for (int row = 0; row < 3; ++row) {  // 3 rows of 10 seats each
        if (row > 0) std::cout << "          ";  // Indent for continuation rows
        
        // Show spectator IDs for this row of 10
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            if (!generalSeatIds[0][seat].empty()) {
                std::cout << "[" << centerText(generalSeatIds[0][seat], 8) << "] ";
            } else {
                std::cout << "[" << centerText("empty", 8) << "] ";  // Empty Normal seat
            }
        }
        std::cout << "\n";
        
        // Show seat IDs directly below this row of spectator IDs
        std::cout << "          ";  // Indent to align with "Normal  : "
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            char seatLabel[10];
            sprintf(seatLabel, "G1-%d", seat+1);
            std::cout << "[" << centerText(seatLabel, 8) << "] ";
        }
        std::cout << "\n";
    }

    // Room 2 - VIP and Normal Section
    std::cout << "\n==== ROOM 2 ====\n";
    std::cout << "VIP     : ";
    // Show all 10 VIP seats for Room 2 (row 1)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[1][s].occupied) {
            // Format as SV00001, SV00002, etc. for VIP
            char formattedId[10];
            sprintf(formattedId, "SV%05d", vipSeats[1][s].spectatorId);
            std::cout << "[" << centerText(formattedId, 8) << "] ";
        } else {
            std::cout << "[" << centerText("empty", 8) << "] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs directly below spectator IDs
    std::cout << "          ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        char seatLabel[10];
        sprintf(seatLabel, "V2-%d", s+1);
        std::cout << "[" << centerText(seatLabel, 8) << "] ";
    }
    std::cout << "\n";

    std::cout << "\nNormal  : ";
    // Show Room 2 general seats - 10 per row with seat IDs below each row
    for (int row = 0; row < 3; ++row) {  // 3 rows of 10 seats each
        if (row > 0) std::cout << "          ";  // Indent for continuation rows
        
        // Show spectator IDs for this row of 10
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            if (!generalSeatIds[1][seat].empty()) {
                std::cout << "[" << centerText(generalSeatIds[1][seat], 8) << "] ";
            } else {
                std::cout << "[" << centerText("empty", 8) << "] ";  // Empty Normal seat
            }
        }
        std::cout << "\n";
        
        // Show seat IDs directly below this row of spectator IDs
        std::cout << "          ";  // Indent to align with "Normal  : "
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            char seatLabel[10];
            sprintf(seatLabel, "G2-%d", seat+1);
            std::cout << "[" << centerText(seatLabel, 8) << "] ";
        }
        std::cout << "\n";
    }

    // Room 3 - VIP and Normal Section
    std::cout << "\n==== ROOM 3 ====\n";
    std::cout << "VIP     : ";
    // Show all 10 VIP seats for Room 3 (row 2)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[2][s].occupied) {
            // Format as SV00001, SV00002, etc. for VIP
            char formattedId[10];
            sprintf(formattedId, "SV%05d", vipSeats[2][s].spectatorId);
            std::cout << "[" << centerText(formattedId, 8) << "] ";
        } else {
            std::cout << "[" << centerText("empty", 8) << "] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs directly below spectator IDs
    std::cout << "          ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        char seatLabel[10];
        sprintf(seatLabel, "V3-%d", s+1);
        std::cout << "[" << centerText(seatLabel, 8) << "] ";
    }
    std::cout << "\n";

    std::cout << "\nNormal  : ";
    // Show Room 3 general seats - 10 per row with seat IDs below each row
    for (int row = 0; row < 3; ++row) {  // 3 rows of 10 seats each
        if (row > 0) std::cout << "          ";  // Indent for continuation rows
        
        // Show spectator IDs for this row of 10
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            if (!generalSeatIds[2][seat].empty()) {
                std::cout << "[" << centerText(generalSeatIds[2][seat], 8) << "] ";
            } else {
                std::cout << "[" << centerText("empty", 8) << "] ";  // Empty Normal seat
            }
        }
        std::cout << "\n";
        
        // Show seat IDs directly below this row of spectator IDs
        std::cout << "          ";  // Indent to align with "Normal  : "
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            char seatLabel[10];
            sprintf(seatLabel, "G3-%d", seat+1);
            std::cout << "[" << centerText(seatLabel, 8) << "] ";
        }
        std::cout << "\n";
    }

    // Room 4 - VIP and Normal Section
    std::cout << "\n==== ROOM 4 ====\n";
    std::cout << "VIP     : ";
    // Show all 10 VIP seats for Room 4 (row 3)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[3][s].occupied) {
            // Format as SV00001, SV00002, etc. for VIP
            char formattedId[10];
            sprintf(formattedId, "SV%05d", vipSeats[3][s].spectatorId);
            std::cout << "[" << centerText(formattedId, 8) << "] ";
        } else {
            std::cout << "[" << centerText("empty", 8) << "] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs directly below spectator IDs
    std::cout << "          ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        char seatLabel[10];
        sprintf(seatLabel, "V4-%d", s+1);
        std::cout << "[" << centerText(seatLabel, 8) << "] ";
    }
    std::cout << "\n";

    std::cout << "\nNormal  : ";
    // Show Room 4 general seats - 10 per row with seat IDs below each row
    for (int row = 0; row < 3; ++row) {  // 3 rows of 10 seats each
        if (row > 0) std::cout << "          ";  // Indent for continuation rows
        
        // Show spectator IDs for this row of 10
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            if (!generalSeatIds[3][seat].empty()) {
                std::cout << "[" << centerText(generalSeatIds[3][seat], 8) << "] ";
            } else {
                std::cout << "[" << centerText("empty", 8) << "] ";  // Empty Normal seat
            }
        }
        std::cout << "\n";
        
        // Show seat IDs directly below this row of spectator IDs
        std::cout << "          ";  // Indent to align with "Normal  : "
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            char seatLabel[10];
            sprintf(seatLabel, "G4-%d", seat+1);
            std::cout << "[" << centerText(seatLabel, 8) << "] ";
        }
        std::cout << "\n";
    }

    // Room 5 - VIP and Normal Section
    std::cout << "\n==== ROOM 5 ====\n";
    std::cout << "VIP     : ";
    // Show all 10 VIP seats for Room 5 (row 4)
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        if (vipSeats[4][s].occupied) {
            // Format as SV00001, SV00002, etc. for VIP
            char formattedId[10];
            sprintf(formattedId, "SV%05d", vipSeats[4][s].spectatorId);
            std::cout << "[" << centerText(formattedId, 8) << "] ";
        } else {
            std::cout << "[" << centerText("empty", 8) << "] ";  // Empty VIP seat
        }
    }
    std::cout << "\n";
    
    // Show seat IDs directly below spectator IDs
    std::cout << "          ";
    for (int s = 0; s < SEATS_PER_ROW; ++s) {
        char seatLabel[10];
        sprintf(seatLabel, "V5-%d", s+1);
        std::cout << "[" << centerText(seatLabel, 8) << "] ";
    }
    std::cout << "\n";

    std::cout << "\nNormal  : ";
    // Show Room 5 general seats - 10 per row with seat IDs below each row
    for (int row = 0; row < 3; ++row) {  // 3 rows of 10 seats each
        if (row > 0) std::cout << "          ";  // Indent for continuation rows
        
        // Show spectator IDs for this row of 10
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            if (!generalSeatIds[4][seat].empty()) {
                std::cout << "[" << centerText(generalSeatIds[4][seat], 8) << "] ";
            } else {
                std::cout << "[" << centerText("empty", 8) << "] ";  // Empty Normal seat
            }
        }
        std::cout << "\n";
        
        // Show seat IDs directly below this row of spectator IDs
        std::cout << "          ";  // Indent to align with "Normal  : "
        for (int seat = row * 10; seat < (row + 1) * 10 && seat < 30; ++seat) {
            char seatLabel[10];
            sprintf(seatLabel, "G5-%d", seat+1);
            std::cout << "[" << centerText(seatLabel, 8) << "] ";
        }
        std::cout << "\n";
    }

    // Influencer Section (separate area)
    std::cout << "\n==== INFLUENCER SECTION ====\n";
    std::cout << "Influencer Seating: " << influencerOccupied << "/" << influencerCapacity << " occupied\n";
    std::cout << "Note: Influencer seats are managed as a block seating area\n";

    // Streaming Rooms (separate areas)
    std::cout << "\n==== STREAMING ROOMS ====\n";
    for (int i = 0; i < numStreamRooms; ++i) {
        int currentViewers = streamingRooms[i].viewers.size();
        std::cout << "Stream Room " << (i+1) << ": ";
        
        if (currentViewers > 0) {
            // Get the first (and typically only) viewer in the streaming room
            // Format as S00001, S00002, etc.
            try {
                Spectator streamer = streamingRooms[i].viewers.peek();
                char formattedId[10];
                sprintf(formattedId, "S%05d", streamer.id);
                std::cout << "[" << centerText(formattedId, 8) << "]\n";
                std::cout << "               [" << centerText("SR" + std::to_string(i+1) + "-1", 8) << "]\n";
                std::cout << "               [" << centerText("Streamer", 8) << "]\n";
            } catch (...) {
                std::cout << "[" << centerText("error", 8) << "]\n";
                std::cout << "               [" << centerText("SR" + std::to_string(i+1) + "-1", 8) << "]\n";
                std::cout << "               [" << centerText("Streamer", 8) << "]\n";
            }
        } else {
            std::cout << "[" << centerText("empty", 8) << "]\n";
            std::cout << "                 [" << centerText("SR" + std::to_string(i+1) + "-1", 8) << "]\n";
            std::cout << "                 [" << centerText("Streamer", 8) << "]\n";
        }
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
bool SeatingManager::assignVIPSeat(const Spectator &s, bool /*quiet*/) {
    if (!vipSeating.isFull()) {
        // Find next available VIP seat for display tracking
        SeatPosition nextSeat;
        bool found = false;
        for (int r = 0; r < VIP_ROWS && !found; ++r) {
            for (int seat = 0; seat < SEATS_PER_ROW && !found; ++seat) {
                if (!vipSeats[r][seat].occupied) {
                    nextSeat = SeatPosition(r+1, seat+1);
                    vipSeats[r][seat].occupied = true;
                    vipSeats[r][seat].spectatorId = s.id;
                    found = true;
                }
            }
        }
        
        if (found) {
            try {
                // Use priority within VIP seating
                int priority = getSpectatorPriority(s.type);
                vipSeating.enqueue(s, priority);
                return true;
            } catch (...) {
                // Rollback the seat assignment if enqueue fails
                for (int r = 0; r < VIP_ROWS; ++r) {
                    for (int seat = 0; seat < SEATS_PER_ROW; ++seat) {
                        if (vipSeats[r][seat].occupied && vipSeats[r][seat].spectatorId == s.id) {
                            vipSeats[r][seat].occupied = false;
                            vipSeats[r][seat].spectatorId = 0;
                            return false;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool SeatingManager::assignInfluencerSeat(const Spectator &s, bool /*quiet*/) {
    if (influencerOccupied < influencerCapacity) {
        try {
            // Use priority within Influencer seating
            int priority = getSpectatorPriority(s.type);
            influencerSeating.enqueue(s, priority);
            ++influencerOccupied;
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool SeatingManager::assignStreamingSeat(const Spectator &s, bool /*quiet*/) {
    // Check each streaming room for availability
    for (int i = 0; i < numStreamRooms; ++i) {
        // Validate room index is within bounds
        if (i >= MAX_STREAMING_ROOMS) {
            break;
        }
        
        // Check if this room has capacity and space
        if (streamingRooms[i].capacity > 0) {
            bool roomFull;
            try {
                roomFull = streamingRooms[i].viewers.isFull();
            } catch (...) {
                continue;
            }
            
            if (!roomFull) {
                try {
                    streamingRooms[i].viewers.enqueue(s);
                    streamerOccupied++;
                    return true;
                } catch (...) {
                    // If enqueue fails, try next room
                    continue;
                }
            }
        }
    }
    
    return false;
}

bool SeatingManager::assignGeneralSeat(const Spectator &s, bool /*quiet*/) {
    return assignGeneralSeatWithPriority(s, false, getSpectatorPriority(s.type));
}

bool SeatingManager::assignGeneralSeatWithPriority(const Spectator &s, bool /*quiet*/, int priority) {
    bool isFull = generalSeating.isFull();
    
    if (!isFull) {
        try {
            generalSeating.enqueue(s, priority);
        } catch (...) {
            return false;
        }
        
        generalOccupied++;  // Update manual counter
        
        // Find the first available empty seat instead of using sequential assignment
        bool seatFound = false;
        int assignedRoom = -1, assignedSeat = -1;
        
        for (int room = 0; room < 5 && !seatFound; ++room) {
            for (int seat = 0; seat < 30 && !seatFound; ++seat) {
                if (generalSeatIds[room][seat].empty()) {
                    assignedRoom = room;
                    assignedSeat = seat;
                    seatFound = true;
                }
            }
        }
        
        if (seatFound) {
            // Format based on spectator type: VIP gets SV prefix, others get S prefix
            char formattedId[10];
            if (s.type == SpectatorType::VIP) {
                sprintf(formattedId, "SV%05d", s.id);
            } else {
                sprintf(formattedId, "S%05d", s.id);
            }
            generalSeatIds[assignedRoom][assignedSeat] = formattedId;
        }
        
        return true;
    }
    return false;
}

bool SeatingManager::hasOverflow() const {
    return !overflowQueue.isEmpty();
}

void SeatingManager::handleOverflow() {
    std::cout << "\n=== Processing Overflow Queue ===\n";
    
    // Separate overflow by type
    DoublyLinkedList<Spectator> vipOverflow;
    DoublyLinkedList<Spectator> influencerOverflow;
    DoublyLinkedList<Spectator> streamerOverflow;
    DoublyLinkedList<Spectator> normalOverflow;
    
    while (!overflowQueue.isEmpty()) {
        Spectator s = overflowQueue.dequeue();
        overflowCount--;  // Update manual counter when dequeuing
        
        switch (s.type) {
            case SpectatorType::VIP:
                vipOverflow.append(s);
                break;
            case SpectatorType::Influencer:
                influencerOverflow.append(s);
                break;
            case SpectatorType::Streamer:
                streamerOverflow.append(s);
                break;
            case SpectatorType::Normal:
                normalOverflow.append(s);
                break;
            default:
                break;
        }
    }
    
    std::cout << "Overflow Summary:\n";
    std::cout << "  - VIP overflow: " << vipOverflow.getSize() << "\n";
    std::cout << "  - Influencer overflow: " << influencerOverflow.getSize() << "\n";
    std::cout << "  - Streamer overflow: " << streamerOverflow.getSize() << "\n";
    std::cout << "  - Normal overflow: " << normalOverflow.getSize() << "\n";
    
    // Direct overflow handling - start manual seat management
    std::cout << "\n=== Manual Seat Management ===\n";
    handleOverflowManual(vipOverflow, influencerOverflow, streamerOverflow, normalOverflow);
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

// New helper method for drop and replace system
void SeatingManager::handleDropAndReplace(const DoublyLinkedList<Spectator>& vipOverflow,
                                         const DoublyLinkedList<Spectator>& influencerOverflow,
                                         const DoublyLinkedList<Spectator>& streamerOverflow,
                                         const DoublyLinkedList<Spectator>& normalOverflow) {
    std::cout << "Drop and Replace System allows you to remove existing users to make space for overflow users.\n";
    
    // Handle VIP overflow
    if (vipOverflow.getSize() > 0) {
        std::cout << "\n=== VIP Overflow Processing ===\n";
        for (int i = 0; i < vipOverflow.getSize(); ++i) {
            Spectator* vipSpec = vipOverflow.get(i);
            if (!vipSpec) continue;
            
            std::cout << "\nVIP overflow user: " << vipSpec->name << " (ID: " << vipSpec->id << ")\n";
            std::cout << "1. Drop a normal user from general seating to make space\n";
            std::cout << "2. Drop an influencer from general seating to make space\n";
            std::cout << "3. Skip this VIP user\n";
            std::cout << "Choice: ";
            
            int choice;
            std::cin >> choice;
            
            if (choice == 1 || choice == 2) {
                if (dropUserFromGeneralSeating(choice == 1 ? SpectatorType::Normal : SpectatorType::Influencer)) {
                    if (assignGeneralSeatWithPriority(*vipSpec, false, 5)) {
                        std::cout << "Successfully placed VIP " << vipSpec->name << " in general seating.\n";
                    } else {
                        std::cout << "Failed to assign VIP " << vipSpec->name << " even after dropping user.\n";
                    }
                } else {
                    std::cout << "No suitable user found to drop.\n";
                }
            } else {
                std::cout << "Skipped VIP " << vipSpec->name << "\n";
            }
        }
    }
    
    // Handle Streamer overflow
    if (streamerOverflow.getSize() > 0) {
        std::cout << "\n=== Streamer Overflow Processing ===\n";
        for (int i = 0; i < streamerOverflow.getSize(); ++i) {
            Spectator* streamerSpec = streamerOverflow.get(i);
            if (!streamerSpec) continue;
            
            std::cout << "\nStreamer overflow user: " << streamerSpec->name << " (ID: " << streamerSpec->id << ")\n";
            std::cout << "1. Drop an existing streamer from streaming rooms\n";
            std::cout << "2. Skip this streamer\n";
            std::cout << "Choice: ";
            
            int choice;
            std::cin >> choice;
            
            if (choice == 1) {
                if (dropUserFromStreamingRoom()) {
                    if (assignStreamingSeat(*streamerSpec, false)) {
                        std::cout << "Successfully placed streamer " << streamerSpec->name << " in streaming room.\n";
                    } else {
                        std::cout << "Failed to assign streamer " << streamerSpec->name << " even after dropping user.\n";
                    }
                } else {
                    std::cout << "No streamer found to drop from streaming rooms.\n";
                }
            } else {
                std::cout << "Skipped streamer " << streamerSpec->name << "\n";
            }
        }
    }
    
    // Handle Influencer overflow
    if (influencerOverflow.getSize() > 0) {
        std::cout << "\n=== Influencer Overflow Processing ===\n";
        for (int i = 0; i < influencerOverflow.getSize(); ++i) {
            Spectator* infSpec = influencerOverflow.get(i);
            if (!infSpec) continue;
            
            std::cout << "\nInfluencer overflow user: " << infSpec->name << " (ID: " << infSpec->id << ")\n";
            std::cout << "1. Drop a normal user from general seating to make space\n";
            std::cout << "2. Skip this influencer\n";
            std::cout << "Choice: ";
            
            int choice;
            std::cin >> choice;
            
            if (choice == 1) {
                if (dropUserFromGeneralSeating(SpectatorType::Normal)) {
                    if (assignGeneralSeatWithPriority(*infSpec, false, 4)) {
                        std::cout << "Successfully placed influencer " << infSpec->name << " in general seating.\n";
                    } else {
                        std::cout << "Failed to assign influencer " << infSpec->name << " even after dropping user.\n";
                    }
                } else {
                    std::cout << "No normal user found to drop from general seating.\n";
                }
            } else {
                std::cout << "Skipped influencer " << infSpec->name << "\n";
            }
        }
    }
    
    // Handle Normal overflow
    if (normalOverflow.getSize() > 0) {
        std::cout << "\n=== Normal User Overflow Processing ===\n";
        std::cout << "Normal users have lowest priority. Consider expanding capacity or rejecting.\n";
        for (int i = 0; i < normalOverflow.getSize(); ++i) {
            Spectator* normalSpec = normalOverflow.get(i);
            if (!normalSpec) continue;
            std::cout << "OVERFLOW: " << normalSpec->name << " (ID: " << normalSpec->id << ")\n";
        }
    }
}

// Helper method to drop a user from general seating
bool SeatingManager::dropUserFromGeneralSeating(SpectatorType targetType) {
    // Look through general seating to find a user of target type to drop
    for (int room = 0; room < 5; ++room) {
        for (int seat = 0; seat < 30; ++seat) {
            if (!generalSeatIds[room][seat].empty()) {
                // Extract the spectator ID from the formatted string
                std::string seatId = generalSeatIds[room][seat];
                
                // Check if this matches the target type we want to drop
                bool shouldDrop = false;
                if (targetType == SpectatorType::Normal && seatId.substr(0, 1) == "S") {
                    shouldDrop = true;
                } else if (targetType == SpectatorType::Influencer && seatId.substr(0, 2) == "SI") {
                    shouldDrop = true; // Note: We don't currently have SI prefix, but keeping for future
                }
                
                if (shouldDrop) {
                    std::cout << "Dropping user " << seatId << " from general seating (Room " << (room+1) << ", Seat " << (seat+1) << ")\n";
                    generalSeatIds[room][seat] = "";  // Clear the seat
                    generalOccupied--;  // Update counter
                    return true;
                }
            }
        }
    }
    return false;
}

// Helper method to drop a user from streaming rooms
bool SeatingManager::dropUserFromStreamingRoom() {
    for (int i = 0; i < numStreamRooms; ++i) {
        if (i >= MAX_STREAMING_ROOMS) break;
        
        if (!streamingRooms[i].viewers.isEmpty()) {
            try {
                Spectator droppedStreamer = streamingRooms[i].viewers.dequeue();
                std::cout << "Dropping streamer " << droppedStreamer.name << " (ID: " << droppedStreamer.id << ") from Stream Room " << (i+1) << "\n";
                streamerOccupied--;
                return true;
            } catch (...) {
                continue;
            }
        }
    }
    return false;
}

// Updated helper methods with new signatures
void SeatingManager::handleOverflowWithMoreSeats(const DoublyLinkedList<Spectator>& vipOverflow,
                                                const DoublyLinkedList<Spectator>& influencerOverflow,
                                                const DoublyLinkedList<Spectator>& streamerOverflow,
                                                const DoublyLinkedList<Spectator>& normalOverflow) {
    std::cout << "Attempting to expand seating capacity...\n";
    
    // Count overflow users
    int vipCount = vipOverflow.getSize();
    int influencerCount = influencerOverflow.getSize();
    int streamerCount = streamerOverflow.getSize();
    int normalCount = normalOverflow.getSize();
    
    std::cout << "Overflow counts - VIP: " << vipCount 
              << ", Influencer: " << influencerCount 
              << ", Streamer: " << streamerCount 
              << ", Normal: " << normalCount << "\n";
    std::cout << "Note: This would require administrative approval to add more physical seats.\n";
    std::cout << "Capacity expansion simulation completed.\n";
}

void SeatingManager::handleOverflowReassignment(const DoublyLinkedList<Spectator>& vipOverflow,
                                               const DoublyLinkedList<Spectator>& influencerOverflow,
                                               const DoublyLinkedList<Spectator>& streamerOverflow,
                                               const DoublyLinkedList<Spectator>& normalOverflow) {
    std::cout << "Attempting to reassign overflow spectators to available spaces...\n";
    
    // Try to place VIP overflow in general seating with high priority
    for (int i = 0; i < vipOverflow.getSize() && !generalSeating.isFull(); i++) {
        const Spectator* spectator = vipOverflow.get(i);
        if (spectator) {
            generalSeating.enqueue(*spectator, 5); // High priority for VIP fallback
            std::cout << "Reassigned VIP " << spectator->id << " to general seating with priority 5\n";
        }
    }
    
    // Try to place Influencer overflow in general seating
    for (int i = 0; i < influencerOverflow.getSize() && !generalSeating.isFull(); i++) {
        const Spectator* spectator = influencerOverflow.get(i);
        if (spectator) {
            generalSeating.enqueue(*spectator, 4); // High priority for Influencer fallback
            std::cout << "Reassigned Influencer " << spectator->id << " to general seating with priority 4\n";
        }
    }
    
    // Try to place Streamer overflow in general seating
    for (int i = 0; i < streamerOverflow.getSize() && !generalSeating.isFull(); i++) {
        const Spectator* spectator = streamerOverflow.get(i);
        if (spectator) {
            generalSeating.enqueue(*spectator, 3);
            std::cout << "Reassigned Streamer " << spectator->id << " to general seating with priority 3\n";
        }
    }
    
    // Try to place Normal overflow in general seating
    for (int i = 0; i < normalOverflow.getSize() && !generalSeating.isFull(); i++) {
        const Spectator* spectator = normalOverflow.get(i);
        if (spectator) {
            generalSeating.enqueue(*spectator, 2);
            std::cout << "Reassigned Normal " << spectator->id << " to general seating with priority 2\n";
        }
    }
    
    std::cout << "Reassignment completed.\n";
}

void SeatingManager::handleOverflowManual(const DoublyLinkedList<Spectator>& vipOverflow,
                                         const DoublyLinkedList<Spectator>& influencerOverflow,
                                         const DoublyLinkedList<Spectator>& streamerOverflow,
                                         const DoublyLinkedList<Spectator>& normalOverflow) {
    std::cout << "Manual overflow handling interface...\n";
    
    // Put all spectators back into the overflow queue for processing
    // VIP overflow (highest priority)
    for (int i = 0; i < vipOverflow.getSize(); ++i) {
        const Spectator* spectator = vipOverflow.get(i);
        if (spectator) {
            try {
                overflowQueue.enqueue(*spectator);
                overflowCount++;
            } catch (...) {
                std::cout << "Warning: Could not re-add VIP spectator " << spectator->id << " to overflow queue.\n";
            }
        }
    }
    
    // Influencer overflow
    for (int i = 0; i < influencerOverflow.getSize(); ++i) {
        const Spectator* spectator = influencerOverflow.get(i);
        if (spectator) {
            try {
                overflowQueue.enqueue(*spectator);
                overflowCount++;
            } catch (...) {
                std::cout << "Warning: Could not re-add Influencer spectator " << spectator->id << " to overflow queue.\n";
            }
        }
    }
    
    // Streamer overflow
    for (int i = 0; i < streamerOverflow.getSize(); ++i) {
        const Spectator* spectator = streamerOverflow.get(i);
        if (spectator) {
            try {
                overflowQueue.enqueue(*spectator);
                overflowCount++;
            } catch (...) {
                std::cout << "Warning: Could not re-add Streamer spectator " << spectator->id << " to overflow queue.\n";
            }
        }
    }
    
    // Normal overflow
    for (int i = 0; i < normalOverflow.getSize(); ++i) {
        const Spectator* spectator = normalOverflow.get(i);
        if (spectator) {
            try {
                overflowQueue.enqueue(*spectator);
                overflowCount++;
            } catch (...) {
                std::cout << "Warning: Could not re-add Normal spectator " << spectator->id << " to overflow queue.\n";
            }
        }
    }
    
    std::cout << "Restored " << overflowCount << " spectators back to overflow queue for automatic assignment.\n\n";
    
    // Simplified system: Only removal is manual, addition is automatic
    while (true) {
        std::cout << "=== Manual Seat Management ===\n";
        std::cout << "1. Remove person from seating (enter ID)\n";
        std::cout << "0. Finish management\n";
        std::cout << "Choose option: ";
        
        int option;
        std::cin >> option;
        
        if (option == 0) {
            std::cout << "Manual seat management completed.\n";
            break;
        }
        else if (option == 1) {
            std::cout << "Enter spectator ID to remove (numeric only, e.g. 22 for S00022): ";
            std::string removeIdStr;
            std::cin >> removeIdStr;
            
            try {
                int removeId = std::stoi(removeIdStr);
                if (removeSpectatorById(removeId)) {
                    // Removal message already printed by removeSpectatorById
                } else {
                    std::cout << "Spectator " << removeId << " not found in current seating.\n";
                }
            } catch (const std::exception& e) {
                std::cout << "Invalid ID format. Please enter a valid number (e.g. 22 for S00022).\n";
            }
        }
        else {
            std::cout << "Invalid option. Please choose 0 or 1.\n";
        }
        
        std::cout << "\n";
    }
}

void SeatingManager::rejectAllOverflow(const DoublyLinkedList<Spectator>& vipOverflow,
                                     const DoublyLinkedList<Spectator>& influencerOverflow,
                                     const DoublyLinkedList<Spectator>& streamerOverflow,
                                     const DoublyLinkedList<Spectator>& normalOverflow) {
    std::cout << "Rejecting all overflow users:\n";
    
    for (int i = 0; i < vipOverflow.getSize(); ++i) {
        Spectator* s = vipOverflow.get(i);
        if (s) std::cout << "REJECTED VIP: " << s->name << "\n";
    }
    
    for (int i = 0; i < influencerOverflow.getSize(); ++i) {
        Spectator* s = influencerOverflow.get(i);
        if (s) std::cout << "REJECTED Influencer: " << s->name << "\n";
    }
    
    for (int i = 0; i < streamerOverflow.getSize(); ++i) {
        Spectator* s = streamerOverflow.get(i);
        if (s) std::cout << "REJECTED Streamer: " << s->name << "\n";
    }
    
    for (int i = 0; i < normalOverflow.getSize(); ++i) {
        Spectator* s = normalOverflow.get(i);
        if (s) std::cout << "REJECTED Normal: " << s->name << "\n";
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

// Helper method to remove a spectator by ID from current seating
bool SeatingManager::removeSpectatorById(int id) {
    bool removed = false;
    std::string removedFrom = "";
    
    // Check VIP seating
    for (int r = 0; r < VIP_ROWS; ++r) {
        for (int seat = 0; seat < SEATS_PER_ROW; ++seat) {
            if (vipSeats[r][seat].occupied && vipSeats[r][seat].spectatorId == id) {
                vipSeats[r][seat].occupied = false;
                vipSeats[r][seat].spectatorId = 0;
                removedFrom = "VIP seat V" + std::to_string(r+1) + "-" + std::to_string(seat+1);
                removed = true;
                break;
            }
        }
        if (removed) break;
    }
    
    // Check general seating if not found in VIP
    if (!removed) {
        for (int room = 0; room < 5; ++room) {
            for (int seat = 0; seat < 30; ++seat) {
                if (!generalSeatIds[room][seat].empty()) {
                    std::string seatIdStr = generalSeatIds[room][seat];
                    int seatId = 0;
                    
                    // Extract numeric ID from formatted string (e.g., "S00022" -> 22, "SV00022" -> 22)
                    if (seatIdStr.length() >= 6) {
                        if (seatIdStr.substr(0, 2) == "SV") {
                            // VIP format: SV00022
                            seatId = std::stoi(seatIdStr.substr(2));
                        } else if (seatIdStr.substr(0, 1) == "S") {
                            // Normal format: S00022
                            seatId = std::stoi(seatIdStr.substr(1));
                        }
                    }
                    
                    if (seatId == id) {
                        generalSeatIds[room][seat] = "";
                        generalOccupied--;
                        
                        // Store the freed seat location for assignment
                        int freedRoom = room;
                        int freedSeat = seat;
                        
                        // IMPORTANT: Also need to remove from the actual generalSeating priority queue
                        // Since PriorityQueue doesn't support direct removal by ID, we need to rebuild it
                        // This is a workaround for the limitation of PriorityQueue
                        
                        // Create a temporary queue to hold remaining spectators
                        PriorityQueue<Spectator> tempQueue(generalCapacity);
                        
                        // Move all spectators except the removed one to temp queue
                        while (!generalSeating.isEmpty()) {
                            Spectator temp = generalSeating.dequeue();
                            if (temp.id != id) {
                                int priority = getSpectatorPriority(temp.type);
                                tempQueue.enqueue(temp, priority);
                            }
                        }
                        
                        // Move everyone back to generalSeating
                        while (!tempQueue.isEmpty()) {
                            Spectator temp = tempQueue.dequeue();
                            int priority = getSpectatorPriority(temp.type);
                            generalSeating.enqueue(temp, priority);
                        }
                        
                        removedFrom = "General seat G" + std::to_string(room+1) + "-" + std::to_string(seat+1);
                        removed = true;
                        
                        // Try to automatically assign someone from overflow queue to the SAME freed seat
                        if (overflowCount > 0) {
                            // Try to get someone from overflow queue and assign them to the exact same seat
                            try {
                                if (!overflowQueue.isEmpty()) {
                                    Spectator waitingSpectator = overflowQueue.dequeue();
                                    overflowCount--;  // Update manual counter
                                    
                                    // Try to assign to the specific freed seat
                                    if (assignSpectatorToSpecificSeat(waitingSpectator, freedRoom, freedSeat)) {
                                        std::cout << "Successfully added spectator " << waitingSpectator.id << " (" << waitingSpectator.name << ") to freed seat G" << (freedRoom+1) << "-" << (freedSeat+1) << ".\n";
                                    } else {
                                        // If assignment failed, put them back in overflow
                                        overflowQueue.enqueue(waitingSpectator);
                                        overflowCount++;
                                        std::cout << "Could not assign waiting spectator " << waitingSpectator.id << " to freed seat.\n";
                                    }
                                }
                            } catch (...) {
                                std::cout << "Error while trying to assign waiting spectator to freed seat.\n";
                            }
                        }
                        
                        break;
                    }
                }
            }
            if (removed) break;
        }
    }
    
    // Check streaming rooms if not found elsewhere
    if (!removed) {
        for (int i = 0; i < numStreamRooms; ++i) {
            if (i < MAX_STREAMING_ROOMS) {
                if (!streamingRooms[i].viewers.isEmpty()) {
                    try {
                        // Create a temporary queue to check if our target ID is in this room
                        CircularQueue<Spectator> tempQueue(streamingRooms[i].capacity);
                        bool foundTarget = false;
                        Spectator targetSpectator;
                        
                        // Check all spectators in this streaming room
                        while (!streamingRooms[i].viewers.isEmpty()) {
                            Spectator current = streamingRooms[i].viewers.dequeue();
                            if (current.id == id) {
                                foundTarget = true;
                                targetSpectator = current;
                                // Don't add back to temp queue - this removes them
                            } else {
                                tempQueue.enqueue(current);
                            }
                        }
                        
                        // Put back all the other spectators
                        while (!tempQueue.isEmpty()) {
                            streamingRooms[i].viewers.enqueue(tempQueue.dequeue());
                        }
                        
                        if (foundTarget) {
                            streamerOccupied--;
                            removedFrom = "Streaming Room " + std::to_string(i+1);
                            removed = true;
                            
                            // Try to automatically assign a waiting streamer to this freed streaming room seat
                            if (overflowCount > 0) {
                                try {
                                    // Look for a streamer in the overflow queue
                                    CircularQueue<Spectator> tempOverflowQueue(500);
                                    bool foundWaitingStreamer = false;
                                    Spectator waitingStreamer;
                                    
                                    // Go through overflow queue looking for a streamer
                                    while (!overflowQueue.isEmpty()) {
                                        Spectator current = overflowQueue.dequeue();
                                        overflowCount--;
                                        
                                        if (!foundWaitingStreamer && current.type == SpectatorType::Streamer) {
                                            foundWaitingStreamer = true;
                                            waitingStreamer = current;
                                            // Don't add to temp queue - we'll assign them
                                        } else {
                                            tempOverflowQueue.enqueue(current);
                                        }
                                    }
                                    
                                    // Put back all non-streamer overflow spectators
                                    while (!tempOverflowQueue.isEmpty()) {
                                        overflowQueue.enqueue(tempOverflowQueue.dequeue());
                                        overflowCount++;
                                    }
                                    
                                    // Assign the waiting streamer to this specific streaming room
                                    if (foundWaitingStreamer) {
                                        if (!streamingRooms[i].viewers.isFull()) {
                                            streamingRooms[i].viewers.enqueue(waitingStreamer);
                                            streamerOccupied++;
                                            std::cout << "** System assigned waiting streamer " << waitingStreamer.id << " (" << waitingStreamer.name << ") to freed Streaming Room " << (i+1) << " **\n";
                                        } else {
                                            // Put back in overflow if room somehow became full
                                            overflowQueue.enqueue(waitingStreamer);
                                            overflowCount++;
                                        }
                                    }
                                    
                                } catch (...) {
                                    std::cout << "Error while trying to assign waiting streamer to freed streaming room.\n";
                                }
                            }
                            
                            break;
                        }
                        
                    } catch (...) {
                        std::cout << "Error while checking streaming room " << (i+1) << " for spectator " << id << "\n";
                        continue;
                    }
                }
            }
        }
    }
    
    if (removed) {
        std::cout << "Removed " << id << " from " << removedFrom << "\n";
        
        // Note: For general seating, automatic assignment to the same freed seat 
        // is handled within the general seating removal section above
        
        // Show updated seating availability after removal
        std::cout << "\n--- Updated Seating Status After Removal ---\n";
        
        // Count occupied VIP seats
        int vipOccupied = 0;
        for (int r = 0; r < VIP_ROWS; ++r) {
            for (int s = 0; s < SEATS_PER_ROW; ++s) {
                if (vipSeats[r][s].occupied) {
                    vipOccupied++;
                }
            }
        }
        
        std::cout << "VIP Seating: " << vipOccupied << "/" << vipCapacity << " occupied\n";
        std::cout << "Influencer Seating: " << influencerOccupied << "/" << influencerCapacity << " occupied\n";
        std::cout << "General Seating: " << generalOccupied << "/" << generalCapacity << " occupied\n";
        std::cout << "Stream Rooms: " << streamerOccupied << "/" << (numStreamRooms * seatsPerStreamRoom) << " occupied\n";
        
        // Show overflow counts
        std::cout << "\nOverflow Queue Status:\n";
        std::cout << "Total waiting: " << overflowCount << "\n";
        if (overflowCount > 0) {
            std::cout << "** There are " << overflowCount << " people still waiting who could potentially be seated! **\n";
        } else {
            std::cout << "No one in overflow queue.\n";
        }
        std::cout << "-------------------------------------------\n\n";
    }
    
    return removed;
}

// Helper method to add a spectator to an available seat
bool SeatingManager::addSpectatorToAvailableSeat(const Spectator& spectator) {
    bool assigned = false;
    std::string assignedLocation = "";
    
    // Try to assign based on spectator type with fallback logic
    switch (spectator.type) {
        case SpectatorType::VIP:
            assigned = assignVIPSeat(spectator, false);
            if (assigned) {
                // Find the VIP seat that was just assigned
                for (int r = 0; r < VIP_ROWS && assignedLocation.empty(); ++r) {
                    for (int s = 0; s < SEATS_PER_ROW && assignedLocation.empty(); ++s) {
                        if (vipSeats[r][s].occupied && vipSeats[r][s].spectatorId == spectator.id) {
                            assignedLocation = "VIP seat V" + std::to_string(r+1) + "-" + std::to_string(s+1);
                        }
                    }
                }
            } else {
                // VIP fallback to general seating with high priority
                assigned = assignGeneralSeatWithPriority(spectator, false, 5);
                if (assigned) {
                    // Find the general seat that was just assigned
                    for (int room = 0; room < 5 && assignedLocation.empty(); ++room) {
                        for (int seat = 0; seat < 30 && assignedLocation.empty(); ++seat) {
                            if (!generalSeatIds[room][seat].empty()) {
                                std::string seatIdStr = generalSeatIds[room][seat];
                                int seatId = 0;
                                
                                // Extract numeric ID from formatted string
                                if (seatIdStr.length() >= 6) {
                                    if (seatIdStr.substr(0, 2) == "SV") {
                                        seatId = std::stoi(seatIdStr.substr(2));
                                    } else if (seatIdStr.substr(0, 1) == "S") {
                                        seatId = std::stoi(seatIdStr.substr(1));
                                    }
                                }
                                
                                if (seatId == spectator.id) {
                                    assignedLocation = "General seat G" + std::to_string(room+1) + "-" + std::to_string(seat+1);
                                }
                            }
                        }
                    }
                }
            }
            break;
        case SpectatorType::Influencer:
            assigned = assignInfluencerSeat(spectator, false);
            if (assigned) {
                assignedLocation = "Influencer Section (block seating)";
            } else {
                // Influencer fallback to general seating with medium priority
                assigned = assignGeneralSeatWithPriority(spectator, false, 4);
                if (assigned) {
                    // Find the general seat that was just assigned
                    for (int room = 0; room < 5 && assignedLocation.empty(); ++room) {
                        for (int seat = 0; seat < 30 && assignedLocation.empty(); ++seat) {
                            if (!generalSeatIds[room][seat].empty()) {
                                std::string seatIdStr = generalSeatIds[room][seat];
                                int seatId = 0;
                                
                                // Extract numeric ID from formatted string
                                if (seatIdStr.length() >= 6) {
                                    if (seatIdStr.substr(0, 2) == "SV") {
                                        seatId = std::stoi(seatIdStr.substr(2));
                                    } else if (seatIdStr.substr(0, 1) == "S") {
                                        seatId = std::stoi(seatIdStr.substr(1));
                                    }
                                }
                                
                                if (seatId == spectator.id) {
                                    assignedLocation = "General seat G" + std::to_string(room+1) + "-" + std::to_string(seat+1);
                                }
                            }
                        }
                    }
                }
            }
            break;
        case SpectatorType::Streamer:
            assigned = assignStreamingSeat(spectator, false);
            if (assigned) {
                // Find which streaming room was assigned
                for (int i = 0; i < numStreamRooms; ++i) {
                    if (i < MAX_STREAMING_ROOMS) {
                        try {
                            // Check if this room contains our spectator (simplified check)
                            if (!streamingRooms[i].viewers.isEmpty()) {
                                assignedLocation = "Streaming Room SR" + std::to_string(i+1) + "-1";
                                break; // Assume it's the most recently assigned room
                            }
                        } catch (...) {
                            continue;
                        }
                    }
                }
            } else {
                // Streamer fallback to general seating
                assigned = assignGeneralSeat(spectator, false);
                if (assigned) {
                    // Find the general seat that was just assigned
                    for (int room = 0; room < 5 && assignedLocation.empty(); ++room) {
                        for (int seat = 0; seat < 30 && assignedLocation.empty(); ++seat) {
                            if (!generalSeatIds[room][seat].empty()) {
                                std::string seatIdStr = generalSeatIds[room][seat];
                                int seatId = 0;
                                
                                // Extract numeric ID from formatted string
                                if (seatIdStr.length() >= 6) {
                                    if (seatIdStr.substr(0, 2) == "SV") {
                                        seatId = std::stoi(seatIdStr.substr(2));
                                    } else if (seatIdStr.substr(0, 1) == "S") {
                                        seatId = std::stoi(seatIdStr.substr(1));
                                    }
                                }
                                
                                if (seatId == spectator.id) {
                                    assignedLocation = "General seat G" + std::to_string(room+1) + "-" + std::to_string(seat+1);
                                }
                            }
                        }
                    }
                }
            }
            break;
        case SpectatorType::Normal:
            assigned = assignGeneralSeat(spectator, false);
            if (assigned) {
                // Find the general seat that was just assigned
                for (int room = 0; room < 5 && assignedLocation.empty(); ++room) {
                    for (int seat = 0; seat < 30 && assignedLocation.empty(); ++seat) {
                        if (!generalSeatIds[room][seat].empty()) {
                            std::string seatIdStr = generalSeatIds[room][seat];
                            int seatId = 0;
                            
                            // Extract numeric ID from formatted string
                            if (seatIdStr.length() >= 6) {
                                if (seatIdStr.substr(0, 2) == "SV") {
                                    seatId = std::stoi(seatIdStr.substr(2));
                                } else if (seatIdStr.substr(0, 1) == "S") {
                                    seatId = std::stoi(seatIdStr.substr(1));
                                }
                            }
                            
                            if (seatId == spectator.id) {
                                assignedLocation = "General seat G" + std::to_string(room+1) + "-" + std::to_string(seat+1);
                            }
                        }
                    }
                }
            }
            break;
        default:
            assigned = false;
    }
    
    // Show assignment details
    if (assigned && !assignedLocation.empty()) {
        std::cout << "** System assigned spectator " << spectator.id << " to " << assignedLocation << " **\n";
    }
    
    return assigned;
}

// Helper method to assign a spectator to a specific freed seat
bool SeatingManager::assignSpectatorToSpecificSeat(const Spectator& spectator, int room, int seat) {
    // Verify the seat is actually empty
    if (!generalSeatIds[room][seat].empty()) {
        std::cout << "   Seat G" << (room+1) << "-" << (seat+1) << " is not empty!\n";
        return false;
    }
    
    // Verify general seating has space
    if (generalSeating.isFull()) {
        std::cout << "   General seating queue is full!\n";
        return false;
    }
    
    try {
        // Add to the priority queue
        int priority = getSpectatorPriority(spectator.type);
        generalSeating.enqueue(spectator, priority);
        
        // Update manual counter
        generalOccupied++;
        
        // Assign to the specific seat
        char formattedId[10];
        if (spectator.type == SpectatorType::VIP) {
            sprintf(formattedId, "SV%05d", spectator.id);
        } else {
            sprintf(formattedId, "S%05d", spectator.id);
        }
        generalSeatIds[room][seat] = formattedId;
        
        return true;
        
    } catch (...) {
        std::cout << "   Exception during specific seat assignment\n";
        return false;
    }
}
