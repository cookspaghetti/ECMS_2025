// File: functions/SpectatorRegistration.cpp
#include <iostream>
#include <fstream>
#include <cstdio>
#include <nlohmann/json.hpp>
#include "functions/SpectatorRegistration.hpp"

using json = nlohmann::json;

SpectatorRegistration::SpectatorRegistration()
  : seatingManager(new SeatingManager(
        /* VIP rows */          1,  /* seats/VIP row */      10,  // 10 VIP seats total per room
        /* Infl rows */         2,  /* seats/Infl row */     5,   // 10 Influencer seats per room  
        /* Streaming rooms */   5,  /* seats/stream room */  1,   // 5 streaming rooms × 1 streamer each = 5 streaming seats total
        /* general capacity */  150 // 30 normal/general seats per room × 5 rooms = 150 total general seats
    )),
    waitingList(),
    processed(false),
    dataLoaded(false)
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
        
        // Mark that data has been loaded successfully
        dataLoaded = true;
        
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

        // Load check-in data into our DoublyLinkedList
        for (const auto& checkIn : checkInArray) {
            std::string spectatorId = checkIn["spectatorId"];
            std::string checkInDateTime = checkIn["checkInDateTime"];
            
            CheckInInfo info(spectatorId.c_str(), checkInDateTime.c_str());
            checkedInSpectators.append(info);
        }

        std::cout << "Loaded check-in data for " << checkedInSpectators.getSize() << " spectators.\n";
        dataLoaded = true;

        
    } catch (const std::exception& e) {
        std::cout << "Error parsing check-in JSON file: " << e.what() << "\n";
        std::cout << "All registered spectators will be considered for seating.\n";
    }
}

CheckInInfo* SpectatorRegistration::findCheckInInfo(const char* spectatorId) {
    for (int i = 0; i < checkedInSpectators.getSize(); i++) {
        CheckInInfo* info = checkedInSpectators.get(i);
        if (info && strcmp(info->spectatorId, spectatorId) == 0) {
            return info;
        }
    }
    return nullptr;
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
    if (!dataLoaded) {
        std::cout << "Error: Spectator data not loaded yet. Please ensure JSON files are accessible.\n";
        return;
    }
    
    if (!processed) {
        std::cout << "\n=== Assigning Seats Based on Check-in Data ===\n";
        
        // Create separate lists for each spectator type
        DoublyLinkedList<Spectator> vipSpectators;
        DoublyLinkedList<Spectator> influencerSpectators;
        DoublyLinkedList<Spectator> normalSpectators;
        DoublyLinkedList<Spectator> streamerSpectators;
        DoublyLinkedList<Spectator> playerSpectators;
        
        // Go through all registered spectators and separate those who checked in
        for (int i = 0; i < waitingList.getSize(); i++) {
            Spectator* spectator = waitingList.get(i);
            if (spectator) {
                // Convert spectator ID to string format (e.g., "S00001")
                char spectatorIdStr[10];
                sprintf(spectatorIdStr, "S%05d", spectator->id);
                
                // Check if this spectator has checked in
                CheckInInfo* checkInInfo = findCheckInInfo(spectatorIdStr);
                if (checkInInfo != nullptr) {
                    // This spectator has checked in, add to appropriate priority list
                    switch (spectator->type) {
                        case SpectatorType::VIP:
                            vipSpectators.append(*spectator);
                            break;
                        case SpectatorType::Influencer:
                            influencerSpectators.append(*spectator);
                            break;
                        case SpectatorType::Streamer:
                            streamerSpectators.append(*spectator);
                            break;
                        case SpectatorType::Player:
                            playerSpectators.append(*spectator);
                            break;
                        case SpectatorType::Normal:
                        default:
                            normalSpectators.append(*spectator);
                            break;
                    }
                }
            }
        }
        
        int totalCheckedIn = vipSpectators.getSize() + influencerSpectators.getSize() + 
                           normalSpectators.getSize() + streamerSpectators.getSize() + 
                           playerSpectators.getSize();
        
        std::cout << "Processing " << totalCheckedIn << " checked-in spectators for seating...\n";
        
        // Process in priority order: VIP → Influencer → Normal → Streamer
        // (Removed check-in time sorting to avoid CircularQueue issues)
        
        // Process VIP spectators
        for (int i = 0; i < vipSpectators.getSize(); i++) {
            Spectator* spectator = vipSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);
            }
        }
        
        // Process Influencer spectators
        for (int i = 0; i < influencerSpectators.getSize(); i++) {
            Spectator* spectator = influencerSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);
            }
        }
        
        // Process Normal spectators
        for (int i = 0; i < normalSpectators.getSize(); i++) {
            Spectator* spectator = normalSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);
            }
        }
        
        // Process Streamer spectators
        for (int i = 0; i < streamerSpectators.getSize(); i++) {
            Spectator* spectator = streamerSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);
            }
        }
        
        // Process Player spectators
        for (int i = 0; i < playerSpectators.getSize(); i++) {
            Spectator* spectator = playerSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);
            }
        }
        
        seatingManager->processEntryQueue(/*verbose=*/false);
        processed = true;
    }
    
    std::cout << "\n=== Current Seating Status ===\n";
    seatingManager->displaySeatingStatus();
    
    // Check if there's overflow and handle it
    if (seatingManager->hasOverflow()) {
        bool hadOverflowBefore = true;
        seatingManager->handleOverflow();
        
        // Only show updated seating status if there's no more overflow (seats were added)
        // If overflow still exists, it means users were rejected
        if (!seatingManager->hasOverflow() && hadOverflowBefore) {
            std::cout << "\n=== Updated Seating Status ===\n";
            seatingManager->displaySeatingStatus();
        }
    }
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
