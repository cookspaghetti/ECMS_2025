#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "functions/SpectatorRegistration.hpp"
#include "functions/SeatingManager.hpp"
#include "dto/Spectator.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

//——————————————————————————————————————————————————————————
// SpectatorRegistration implementation
//——————————————————————————————————————————————————————————

SpectatorRegistration::SpectatorRegistration() 
  : nextSpectatorId(1001),
    // VIP: 3 rows×10 seats, Influencer: 2 blocks×5 seats,
    // Streaming: 5 rooms×10 viewers, General: 20 seats
    seatingManager(new SeatingManager(
        /*numVipRows=*/3, /*seatsPerVipRow=*/10,
        /*numInflRows=*/2, /*seatsPerInfluRow=*/5,
        /*numStreamRooms=*/5, /*seatsPerStreamRoom=*/10,
        /*generalCapacity=*/20
    )),
    dataLoaded(false)
{}

SpectatorRegistration::~SpectatorRegistration() {
    delete seatingManager;
}

SpectatorType SpectatorRegistration::parseSpectatorType(const std::string& typeStr) {
    if      (typeStr == "VIP")         return SpectatorType::VIP;
    else if (typeStr == "Streamer")    return SpectatorType::Streamer;
    else if (typeStr == "Influencer")  return SpectatorType::Influencer;
    else if (typeStr == "Player")      return SpectatorType::Player;
    else                                return SpectatorType::Normal;
}

Gender SpectatorRegistration::parseGender(const std::string& genderStr) {
    return (genderStr == "Female") ? Gender::Female : Gender::Male;
}

void SpectatorRegistration::loadSpectatorsFromJSON() {
    if (dataLoaded) {
        std::cout << "Spectator data already loaded.\n";
        return;
    }

    try {
        std::string path = "data/spectators.json";
        if (!std::filesystem::exists(path))
            path = "c:/Users/CHUA/Documents/GitHub/ECMS_2025/data/spectators.json";

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open spectators.json\n";
            return;
        }

        json arr; file >> arr; file.close();
        std::cout << "Loading spectator data...\n";

        int vipCnt=0, infCnt=0, strCnt=0, plyCnt=0, norCnt=0;
        int loaded=0;
        for (auto &item : arr) {
            int id = std::stoi(item["id"].get<std::string>().substr(1));
            std::string name = item["name"];
            Gender g = parseGender(item["gender"]);
            std::string email = item["email"];
            std::string phone = item["phoneNum"];
            auto type = parseSpectatorType(item["type"]);
            std::string aff = item["affiliation"];

            switch(type) {
              case SpectatorType::VIP:         vipCnt++; break;
              case SpectatorType::Influencer:  infCnt++; break;
              case SpectatorType::Streamer:    strCnt++; break;
              case SpectatorType::Player:      plyCnt++; break;
              default:                         norCnt++; break;
            }

            Spectator s{id,name,g,email,phone,type,aff};
            registrationQueue.enqueue(s);
            loaded++;
        }

        // hand off all to seatingManager
        while (!registrationQueue.isEmpty()) {
            auto s = registrationQueue.dequeue();
            seatingManager->addToEntryQueue(s, /*quiet=*/true);
        }

        std::cout << "\n=== PROCESSING SPECTATOR SEATING ===\n";
        seatingManager->processEntryQueue(/*verbose=*/true);  // Changed to true for debugging
        std::cout << "=====================================\n";

        dataLoaded = true;
        std::cout << "Loaded " << loaded << " spectators.\n"
                  << "  VIP: " << vipCnt << "\n"
                  << "  Players: " << plyCnt << "\n"
                  << "  Influencers: " << infCnt << "\n"
                  << "  Streamers: " << strCnt << "\n"
                  << "  Normal: " << norCnt << "\n"
                  << "Use 'Display Queue' to view seating.\n";

    } catch (const std::exception &e) {
        std::cout << "JSON error: " << e.what() << "\n";
    }
}

void SpectatorRegistration::registerSpectator() {
    if (!dataLoaded) {
        std::cout << "\nLoading from JSON...\n";
        loadSpectatorsFromJSON();
        std::cout << "\n" << std::string(50,'=') << "\n";
        std::cout << "Done. Use 'Display Queue'.\n";
    } else {
        std::cout << "\nAlready loaded. Use 'Display Queue'.\n";
    }
}

void SpectatorRegistration::checkInSpectator() {
    if (registrationQueue.isEmpty()) {
        std::cout << "\nNo spectators waiting.\n";
        return;
    }
    auto s = registrationQueue.dequeue();
    std::cout << "\nChecked in: " << s.name
              << " (ID " << s.id << ", " << toString(s.type) << ")\n";
}

void SpectatorRegistration::displayQueue() {
    std::cout << "\n=== Live Stream & Spectator Management System ===\n";
    if (!dataLoaded) {
        std::cout << "No data loaded. Use 'Register Spectator'.\n";
        return;
    }
    std::cout << "Registration queue: "
              << (registrationQueue.isEmpty() ? "All seated"
                                              : std::to_string(registrationQueue.size()) + " waiting")
              << "\n\n";
    seatingManager->displaySeatingStatus();
    std::cout << "==========================================\n";
}

//——————————————————————————————————————————————————————————
// SeatingManager implementation
//——————————————————————————————————————————————————————————

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

void SeatingManager::addToEntryQueue(const Spectator &s, bool /*quiet*/) {
    entryQueue.enqueue(s);
}

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

        // if (!seated) {
        //     overflowQueue.enqueue(sp);
        //     if (verbose)
        //         std::cout << "[Overflow] " << sp.name
        //                   << " (" << toString(sp.type) << ")\n";
        // }

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

void SeatingManager::displaySeatingStatus() const {
    std::cout << "\n=== SEATING STATUS ===\n";

    std::cout << "Overflow Queue: " 
          << overflowQueue.size() << " waiting\n";
    std::cout << "  - VIP overflow: "        << vipOverflow        << "\n";
    std::cout << "  - Influencer overflow: " << infOverflow        << "\n";
    std::cout << "  - Streamer overflow: "   << streamerOverflow   << "\n";
    std::cout << "  - General overflow: "    << generalOverflow    << "\n";

    // VIP
    std::cout << "VIP Seating:\n";
    for (int r = 0; r < VIP_ROWS; ++r) {
        int cnt = 0;
        for (int s = 0; s < SEATS_PER_ROW; ++s)
            if (vipSeats[r][s].occupied) ++cnt;
        std::cout << "  Row " << (r+1)
                  << ": " << cnt << "/" << SEATS_PER_ROW << "\n";
    }

    // Influencers
    std::cout << "Influencer Seating: "
              << influencerOccupied << "/" << influencerCapacity << "\n";

    // Streaming rooms
    std::cout << "Streaming Rooms:\n";
    for (int i = 0; i < MAX_STREAMING_ROOMS; ++i) {
        int cnt = streamingRooms[i].viewers.size();
        std::cout << "  [Room " << (i+1) << "] "
                  << cnt << "/" << streamingRooms[i].capacity
                  << " viewers";
        if (i >= numStreamRooms) {
            std::cout << " (inactive)";
        }
        std::cout << "\n";
    }

    // General
    std::cout << "General Admission: "
              << generalSeating.size() << "/" << generalCapacity << "\n";

    // Overflow
    std::cout << "Overflow Queue: "
              << overflowQueue.size() << " waiting\n\n";
}

// Helpers

SeatPosition SeatingManager::findNextVIPSeat() const {
    for (int r = 0; r < VIP_ROWS; ++r)
      for (int s = 0; s < SEATS_PER_ROW; ++s)
        if (!vipSeats[r][s].occupied)
          return SeatPosition(r+1, s+1);
    return SeatPosition();  // none
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
    // Debug: Check room states
    if (!quiet) {
        std::cout << "[DEBUG] Assigning " << s.name << " to streaming room:\n";
        for (int i = 0; i < numStreamRooms; ++i) {
            std::cout << "  Room " << (i+1) << ": " 
                      << streamingRooms[i].viewers.size() 
                      << "/" << streamingRooms[i].capacity 
                      << " (full: " << (streamingRooms[i].viewers.isFull() ? "yes" : "no") << ")\n";
        }
    }
    
    // Option A: Sequential assignment (current behavior)
    // Try each of the numStreamRooms in order
    for (int i = 0; i < numStreamRooms; ++i) {
        if (!streamingRooms[i].viewers.isFull()) {
            streamingRooms[i].viewers.enqueue(s);
            if (!quiet) {
                std::cout 
                  << s.name 
                  << " seated in Stream Room " << (i+1)
                  << " (" 
                  << streamingRooms[i].viewers.size()
                  << "/" << streamingRooms[i].capacity
                  << " viewers)\n";
            }
            return true;
        }
    }
    
    /* Option B: Round-robin distribution (uncomment to use)
    // Find the room with the least viewers
    int bestRoom = -1;
    int minViewers = INT_MAX;
    
    for (int i = 0; i < numStreamRooms; ++i) {
        if (!streamingRooms[i].viewers.isFull() && 
            streamingRooms[i].viewers.size() < minViewers) {
            minViewers = streamingRooms[i].viewers.size();
            bestRoom = i;
        }
    }
    
    if (bestRoom != -1) {
        streamingRooms[bestRoom].viewers.enqueue(s);
        if (!quiet) {
            std::cout 
              << s.name 
              << " seated in Stream Room " << (bestRoom+1)
              << " (" 
              << streamingRooms[bestRoom].viewers.size()
              << "/" << streamingRooms[bestRoom].capacity
              << " viewers)\n";
        }
        return true;
    }
    */
    
    // all rooms are full → overflow
    if (!quiet) {
        std::cout << "[DEBUG] All " << numStreamRooms << " streaming rooms are full, sending to overflow\n";
    }
    return false;
}

bool SeatingManager::assignGeneralSeat(const Spectator &s, bool quiet) {
    if (!generalSeating.isFull()) {
        generalSeating.enqueue(s);
        if (!quiet)
            std::cout << s.name
                      << " seated General (" << generalSeating.size()
                      << "/" << generalCapacity << ")\n";
        return true;
    }
    return false;
}

