// #include <iostream>
// #include <string>
// #include <fstream>
// #include <filesystem>

// #include "functions/SpectatorRegistration.hpp"
// #include "functions/SeatingManager.hpp"
// #include "dto/Spectator.hpp"
// #include "nlohmann/json.hpp"

// using json = nlohmann::json;

// //——————————————————————————————————————————————————————————
// // SpectatorRegistration implementation
// //——————————————————————————————————————————————————————————

// SpectatorRegistration::SpectatorRegistration() 
//   : nextSpectatorId(1001),
//     // VIP: 3 rows×10 seats, Influencer: 2 blocks×5 seats,
//     // Streaming: 5 rooms×10 viewers, General: 20 seats
//     seatingManager(new SeatingManager(
//         /*numVipRows=*/3, /*seatsPerVipRow=*/10,
//         /*numInflRows=*/2, /*seatsPerStreamRoom=*/5,
//         /*numStreamRooms=*/5, /*seatsPerStreamRoom=*/10,
//         /*generalCapacity=*/20
//     )),
//     dataLoaded(false)
// {}

// SpectatorRegistration::~SpectatorRegistration() {
//     delete seatingManager;
// }

// SpectatorType SpectatorRegistration::parseSpectatorType(const std::string& typeStr) {
//     if      (typeStr == "VIP")        return SpectatorType::VIP;
//     else if (typeStr == "Streamer")   return SpectatorType::Streamer;
//     else if (typeStr == "Influencer") return SpectatorType::Influencer;
//     else if (typeStr == "Player")     return SpectatorType::Player;
//     else                               return SpectatorType::Normal;
// }

// Gender SpectatorRegistration::parseGender(const std::string& genderStr) {
//     return (genderStr == "Female") ? Gender::Female : Gender::Male;
// }

// void SpectatorRegistration::loadSpectatorsFromJSON() {
//     if (dataLoaded) {
//         std::cout << "Spectator data already loaded.\n";
//         return;
//     }
//     try {
//         std::string path = "data/spectators.json";
//         if (!std::filesystem::exists(path))
//             path = "c:/Users/CHUA/Documents/GitHub/ECMS_2025/data/spectators.json";

//         std::ifstream file(path);
//         if (!file.is_open()) {
//             std::cout << "Error: Cannot open spectators.json\n";
//             return;
//         }

//         json arr; file >> arr; file.close();
//         std::cout << "Loading spectator data.\n";

//         int vipCnt=0, infCnt=0, strCnt=0, plyCnt=0, norCnt=0;
//         int loaded=0;
//         for (auto &item : arr) {
//             int id = std::stoi(item["id"].get<std::string>().substr(1));
//             std::string name = item["name"];
//             Gender g = parseGender(item["gender"]);
//             std::string email = item["email"];
//             std::string phone = item["phoneNum"];
//             auto type = parseSpectatorType(item["type"]);
//             std::string aff = item["affiliation"];

//             switch(type) {
//                 case SpectatorType::VIP:         vipCnt++; break;
//                 case SpectatorType::Influencer:  infCnt++; break;
//                 case SpectatorType::Streamer:    strCnt++; break;
//                 case SpectatorType::Player:      plyCnt++; break;
//                 default:                         norCnt++; break;
//             }

//             Spectator s{id, name, g, email, phone, type, aff};
//             registrationQueue.enqueue(s);
//             loaded++;
//         }

//         // hand off all to seatingManager
//         while (!registrationQueue.isEmpty()) {
//             auto s = registrationQueue.dequeue();
//             seatingManager->addToEntryQueue(s, /*quiet=*/true);
//         }

//         std::cout << "\n=== PROCESSING SPECTATOR SEATING ===\n";
//         seatingManager->processEntryQueue(/*verbose=*/true);
//         std::cout << "=====================================\n";

//         dataLoaded = true;
//         std::cout << "Loaded " << loaded << " spectators.\n"
//                   << "  VIP: " << vipCnt << "\n"
//                   << "  Players: " << plyCnt << "\n"
//                   << "  Influencers: " << infCnt << "\n"
//                   << "  Streamers: " << strCnt << "\n"
//                   << "  Normal: " << norCnt << "\n"
//                   << "Use 'Display Queue' to view seating.\n";
//     } catch (const std::exception &e) {
//         std::cout << "JSON error: " << e.what() << "\n";
//     }
// }

// void SpectatorRegistration::registerSpectator() {
//     if (!dataLoaded) {
//         std::cout << "\nLoading from JSON.\n";
//         loadSpectatorsFromJSON();
//         std::cout << "\n" << std::string(50, '=') << "\n";
//         std::cout << "Done. Use 'Display Queue'.\n";
//     } else {
//         std::cout << "\nAlready loaded. Use 'Display Queue'.\n";
//     }
// }

// void SpectatorRegistration::checkInSpectator() {
//     if (registrationQueue.isEmpty()) {
//         std::cout << "\nNo spectators waiting.\n";
//         return;
//     }
//     auto s = registrationQueue.dequeue();
//     std::cout << "\nChecked in: " << s.name
//               << " (ID " << s.id << ", " << toString(s.type) << ")\n";
// }

// void SpectatorRegistration::displayQueue() {
//     std::cout << "\n=== Live Stream & Spectator Management System ===\n";
//     if (!dataLoaded) {
//         std::cout << "No data loaded. Use 'Register Spectator'.\n";
//         return;
//     }
//     std::cout << "Registration queue: "
//               << (registrationQueue.isEmpty() ? "All seated"
//                                               : std::to_string(registrationQueue.size()) + " waiting")
//               << "\n\n";
//     seatingManager->displaySeatingStatus();
//     std::cout << "==========================================\n";
// }


// File: functions/SpectatorRegistration.cpp
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "functions/SpectatorRegistration.hpp"

using json = nlohmann::json;

SpectatorRegistration::SpectatorRegistration()
  : seatingManager(new SeatingManager(
        /* VIP rows */          1,  /* seats/VIP row */      10,  // 10 VIP seats total
        /* Infl rows */         2,  /* seats/Infl row */     5,   // 10 Influencer seats  
        /* Streaming rooms */   5,  /* seats/stream room */  10,  // 5 rooms × 10 = 50 streaming seats
        /* general capacity */  30  // 30 normal/general seats
    )),
    waitingList(),
    processed(false)
{
    // Load spectators from JSON in a separate method to avoid include conflicts
    loadSpectatorsFromJSON();
}

void SpectatorRegistration::loadSpectatorsFromJSON() {
    std::cout << "Loading spectator data from JSON...\n";
    
    try {
        // Load all spectators first
        std::ifstream spectatorFile("data/spectators.json");
        if (!spectatorFile.is_open()) {
            // Try with relative path from build directory
            spectatorFile.open("../data/spectators.json");
        }
        
        if (!spectatorFile.is_open()) {
            std::cout << "Error: Could not open spectators.json file.\n";
            std::cout << "Please ensure the file exists in the data/ directory.\n";
            std::cout << "No spectators loaded. You can add spectators manually.\n";
            return;
        }

        json spectatorArray;
        spectatorFile >> spectatorArray;
        spectatorFile.close();

        // Load all spectators into waiting list
        for (const auto& item : spectatorArray) {
            // Parse ID (remove 'S' prefix)
            std::string idStr = item["id"];
            int id = std::stoi(idStr.substr(1)); // Remove 'S' prefix and convert

            std::string name = item["name"];
            
            // Parse gender
            std::string genderStr = item["gender"];
            Gender gender = (genderStr == "Female") ? Gender::Female : Gender::Male;
            
            std::string email = item["email"];
            std::string phoneNum = item["phoneNum"];
            
            // Parse spectator type
            std::string typeStr = item["type"];
            SpectatorType type;
            if (typeStr == "VIP") type = SpectatorType::VIP;
            else if (typeStr == "Streamer") type = SpectatorType::Streamer;
            else if (typeStr == "Influencer") type = SpectatorType::Influencer;
            else if (typeStr == "Player") type = SpectatorType::Player;
            else type = SpectatorType::Normal;
            
            std::string affiliation = item["affiliation"];

            // Create and add spectator to waiting list (all registered spectators)
            Spectator spectator(id, name, gender, email, phoneNum, type, affiliation);
            waitingList.append(spectator);
        }

        std::cout << "Loaded " << waitingList.getSize() << " registered spectators from JSON file.\n";
        
        // Now load check-in data to determine who actually checked in
        loadCheckInData();
        
    } catch (const std::exception& e) {
        std::cout << "Error parsing spectator JSON file: " << e.what() << "\n";
        std::cout << "No spectators loaded. You can add spectators manually.\n";
    }
}

void SpectatorRegistration::loadCheckInData() {
    try {
        std::ifstream checkInFile("data/check_in.json");
        if (!checkInFile.is_open()) {
            // Try with relative path from build directory
            checkInFile.open("../data/check_in.json");
        }
        
        if (!checkInFile.is_open()) {
            std::cout << "Warning: Could not open check_in.json file.\n";
            std::cout << "All registered spectators will be considered for seating.\n";
            return;
        }

        json checkInArray;
        checkInFile >> checkInArray;
        checkInFile.close();

        // Create a map of checked-in spectators with their check-in times
        checkedInSpectators.clear();
        for (const auto& checkIn : checkInArray) {
            std::string spectatorId = checkIn["spectatorId"];
            std::string checkInDateTime = checkIn["checkInDateTime"];
            checkedInSpectators[spectatorId] = checkInDateTime;
        }

        std::cout << "Loaded check-in data for " << checkedInSpectators.size() << " spectators.\n";
        
    } catch (const std::exception& e) {
        std::cout << "Error parsing check-in JSON file: " << e.what() << "\n";
        std::cout << "All registered spectators will be considered for seating.\n";
    }
}

SpectatorRegistration::~SpectatorRegistration() {
    delete seatingManager;
}

void SpectatorRegistration::registerSpectator() {
    std::cout << "\n=== Spectator Registration ===\n";
    std::cout << "Data loaded: " << waitingList.getSize() << " spectators from JSON file.\n";
    
    std::cout << "\nDo you want to add a spectator manually? (y/n): ";
    char choice;
    std::cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        // Manual registration
        std::cout << "\n=== Manual Spectator Registration ===\n";
        
        std::string idStr;
        int id;
        std::string name, email, phone, affiliation;
        int genderChoice, typeChoice;
        
        std::cin.ignore(); // Clear the newline left by reading choice
        
        std::cout << "Enter Spectator ID (e.g., S00058): ";
        std::getline(std::cin, idStr);
        
        // Parse ID - if it starts with 'S', remove it and convert to int
        if (!idStr.empty() && idStr[0] == 'S') {
            try {
                id = std::stoi(idStr.substr(1));
            } catch (const std::exception& e) {
                std::cout << "Invalid ID format. Using default ID.\n";
                id = 9999; // Default fallback ID
            }
        } else {
            try {
                id = std::stoi(idStr);
            } catch (const std::exception& e) {
                std::cout << "Invalid ID format. Using default ID.\n";
                id = 9999; // Default fallback ID
            }
        }
        
        std::cout << "Enter Name: ";
        std::getline(std::cin, name);
        
        std::cout << "Enter Gender (1-Male, 2-Female): ";
        std::cin >> genderChoice;
        Gender gender = (genderChoice == 2) ? Gender::Female : Gender::Male;
        std::cin.ignore(); // Clear newline after reading integer
        
        std::cout << "Enter Email: ";
        std::getline(std::cin, email);
        
        std::cout << "Enter Phone Number: ";
        std::getline(std::cin, phone);
        
        std::cout << "Select Spectator Type:\n";
        std::cout << "1. Normal\n2. VIP\n3. Streamer\n4. Influencer\n5. Player\n";
        std::cout << "Choose (1-5): ";
        std::cin >> typeChoice;
        std::cin.ignore();
        
        SpectatorType type;
        switch(typeChoice) {
            case 2: type = SpectatorType::VIP; break;
            case 3: type = SpectatorType::Streamer; break;
            case 4: type = SpectatorType::Influencer; break;
            case 5: type = SpectatorType::Player; break;
            default: type = SpectatorType::Normal; break;
        }
        
        std::cout << "Enter Affiliation: ";
        std::getline(std::cin, affiliation);
        
        // Create new spectator
        Spectator newSpectator(id, name, gender, email, phone, type, affiliation);
        
        // Add to waiting list
        waitingList.append(newSpectator);
        
        std::cout << "\nSpectator " << name << " registered successfully!\n";
        std::cout << "Total spectators in system: " << waitingList.getSize() << "\n";
    } else {
        std::cout << "\nNo new spectators added.\n";
        std::cout << "Total spectators in system: " << waitingList.getSize() << "\n";
    }
    
    std::cout << "Use 'Display Queue' to process seating assignments.\n";
}

void SpectatorRegistration::displayQueue() {
    if (!processed) {
        std::cout << "\n=== Assigning Seats ===\n";
        while (waitingList.getSize() > 0) {
            Spectator* firstSpectator = waitingList.get(0);
            if (firstSpectator) {
                Spectator s = *firstSpectator;
                registrationQueue.enqueue(s);
                seatingManager->addToEntryQueue(s, /*quiet=*/true);
                waitingList.removeAt(0);
            } else {
                break;
            }
        }
        seatingManager->processEntryQueue(/*verbose=*/true);
        processed = true;
    }
    std::cout << "\n=== Current Seating Status ===\n";
    seatingManager->displaySeatingStatus();
}

void SpectatorRegistration::checkInSpectator() {
    if (registrationQueue.isEmpty()) {
        std::cout << "No spectators waiting.\n";
        return;
    }
    Spectator s = registrationQueue.dequeue();
    std::cout << "Checked in: " << s.name
              << " (ID " << s.id << ", " << toString(s.type) << ")\n";
}
