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
        std::cout << "Total registered spectators: " << waitingList.getSize() << "\n";
        std::cout << "Total checked-in spectators: " << checkedInSpectators.getSize() << "\n";
        
        // First, let's find and show spectators who registered but did NOT check in
        // Using the search function to find non-checked-in spectators
        std::cout << "\n=== Spectators who registered but did NOT check in (will be dequeued) ===\n";
        int notCheckedInCount = 0;
        
        for (int i = 0; i < waitingList.getSize(); i++) {
            Spectator* spectator = waitingList.get(i);
            if (spectator) {
                // Convert spectator ID to string format (e.g., "S00001")
                char spectatorIdStr[10];
                sprintf(spectatorIdStr, "S%05d", spectator->id);
                
                // Use search function to check if this spectator is NOT in check-in list
                CheckInInfo* checkInInfo = checkedInSpectators.search([spectatorIdStr](const CheckInInfo& info) {
                    return strcmp(info.spectatorId, spectatorIdStr) == 0;
                });
                
                if (checkInInfo == nullptr) {
                    // This spectator registered but did NOT check in - they get "dequeued" (not processed)
                    std::cout << "- DEQUEUED: " << spectator->name << " (ID: " << spectatorIdStr 
                              << ", Type: " << toString(spectator->type) << ") - NOT CHECKED IN\n";
                    notCheckedInCount++;
                }
            }
        }
        
        std::cout << "Total spectators dequeued (not checked in): " << notCheckedInCount << "\n";
        
        // Now process only the checked-in spectators for seating (these get enqueued)
        std::cout << "\n=== Processing checked-in spectators for seating (enqueuing) ===\n";
        
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
                
                // Use search function to check if this spectator checked in
                CheckInInfo* checkInInfo = checkedInSpectators.search([spectatorIdStr](const CheckInInfo& info) {
                    return strcmp(info.spectatorId, spectatorIdStr) == 0;
                });
                
                if (checkInInfo != nullptr) {
                    // This spectator has checked in - they get ENQUEUED for seating
                    std::cout << "✓ ENQUEUED: " << spectator->name << " (ID: " << spectatorIdStr 
                              << ", Type: " << toString(spectator->type) << ") - CHECKED IN\n";
                    
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
        
        std::cout << "\n=== Adding checked-in spectators to CircularQueue for seating ===\n";
        std::cout << "Total spectators to enqueue: " << totalCheckedIn << "\n";
        
        // Process in priority order: VIP → Influencer → Normal → Streamer → Player
        // Each of these spectators gets ENQUEUED into the CircularQueue
        
        // Process VIP spectators (highest priority)
        std::cout << "Enqueuing " << vipSpectators.getSize() << " VIP spectators...\n";
        for (int i = 0; i < vipSpectators.getSize(); i++) {
            Spectator* spectator = vipSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);  // Add to registration queue
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);  // Add to seating queue
            }
        }
        
        // Process Influencer spectators
        std::cout << "Enqueuing " << influencerSpectators.getSize() << " Influencer spectators...\n";
        for (int i = 0; i < influencerSpectators.getSize(); i++) {
            Spectator* spectator = influencerSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);  // Add to registration queue
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);  // Add to seating queue
            }
        }
        
        // Process Normal spectators
        std::cout << "Enqueuing " << normalSpectators.getSize() << " Normal spectators...\n";
        for (int i = 0; i < normalSpectators.getSize(); i++) {
            Spectator* spectator = normalSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);
            }
        }
        
        // Process Streamer spectators
        std::cout << "Enqueuing " << streamerSpectators.getSize() << " Streamer spectators...\n";
        for (int i = 0; i < streamerSpectators.getSize(); i++) {
            Spectator* spectator = streamerSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);  // Add to registration queue
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);  // Add to seating queue
            }
        }
        
        // Process Player spectators
        std::cout << "Enqueuing " << playerSpectators.getSize() << " Player spectators...\n";
        for (int i = 0; i < playerSpectators.getSize(); i++) {
            Spectator* spectator = playerSpectators.get(i);
            if (spectator) {
                registrationQueue.enqueue(*spectator);  // Add to registration queue
                seatingManager->addToEntryQueue(*spectator, /*quiet=*/true);  // Add to seating queue
            }
        }
        
        std::cout << "\n=== Processing CircularQueue (SeatingManager EntryQueue) ===\n";
        
        try {
            seatingManager->processEntryQueue(/*verbose=*/false);  // Use false for production
            std::cout << "Processing completed successfully.\n";
        } catch (const std::exception& e) {
            std::cout << "[ERROR] Exception in processEntryQueue(): " << e.what() << "\n";
        } catch (...) {
            std::cout << "[ERROR] Unknown exception in processEntryQueue()\n";
        }
        
        std::cout << "Setting processed = true and continuing...\n";
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
    std::cout << "\n=== Manual Spectator Check-In ===\n";
    std::cout << "This function allows you to manually check in a registered spectator.\n";
    std::cout << "Total registered spectators: " << waitingList.getSize() << "\n";
    std::cout << "Already checked in: " << checkedInSpectators.getSize() << "\n";
    
    if (waitingList.getSize() == 0) {
        std::cout << "No spectators registered. Please register spectators first.\n";
        return;
    }
    
    // Show available spectators (those who haven't checked in yet)
    std::cout << "\n=== Available Spectators for Check-In ===\n";
    DoublyLinkedList<Spectator> availableSpectators;
    
    for (int i = 0; i < waitingList.getSize(); i++) {
        Spectator* spectator = waitingList.get(i);
        if (spectator) {
            // Convert spectator ID to string format (e.g., "S00001")
            char spectatorIdStr[10];
            sprintf(spectatorIdStr, "S%05d", spectator->id);
            
            // Use search function to check if this spectator has already checked in
            CheckInInfo* checkInInfo = checkedInSpectators.search([spectatorIdStr](const CheckInInfo& info) {
                return strcmp(info.spectatorId, spectatorIdStr) == 0;
            });
            
            if (checkInInfo == nullptr) {
                // This spectator has NOT checked in yet
                availableSpectators.append(*spectator);
                std::cout << availableSpectators.getSize() << ". " << spectator->name 
                          << " (ID: " << spectatorIdStr << ", Type: " << toString(spectator->type) << ")\n";
            }
        }
    }
    
    if (availableSpectators.getSize() == 0) {
        std::cout << "All registered spectators have already checked in!\n";
        return;
    }
    
    // Get user choice
    std::cout << "\nSelect spectator to check in (1-" << availableSpectators.getSize() << "), or 0 to cancel: ";
    int choice;
    std::cin >> choice;
    
    if (choice <= 0 || choice > availableSpectators.getSize()) {
        std::cout << "Check-in cancelled.\n";
        return;
    }
    
    // Get the selected spectator
    Spectator* selectedSpectator = availableSpectators.get(choice - 1);
    if (selectedSpectator) {
        // Create check-in info
        char spectatorIdStr[10];
        sprintf(spectatorIdStr, "S%05d", selectedSpectator->id);
        
        // Use current time as check-in time (simplified)
        char currentTime[25];
        strcpy(currentTime, "2025-01-15 10:00:00");  // You could use actual time here
        
        // Add to check-in list (using DoublyLinkedList)
        CheckInInfo newCheckIn(spectatorIdStr, currentTime);
        checkedInSpectators.append(newCheckIn);
        
        // Also add to registration queue (this represents "enqueuing" for seating)
        registrationQueue.enqueue(*selectedSpectator);
        
        // Add to seating manager queue
        seatingManager->addToEntryQueue(*selectedSpectator, false);
        
        std::cout << "\n✓ SUCCESS: " << selectedSpectator->name << " (ID: " << spectatorIdStr 
                  << ") has been manually checked in!\n";
        std::cout << "- Added to check-in list (DoublyLinkedList)\n";
        std::cout << "- ENQUEUED to registration queue (CircularQueue)\n";
        std::cout << "- ENQUEUED to seating queue (CircularQueue)\n";
        
        std::cout << "\nUpdated totals:\n";
        std::cout << "- Checked-in spectators: " << checkedInSpectators.getSize() << "\n";
        std::cout << "- Spectators in seating queue: " << (registrationQueue.size()) << "\n";
        
        // Ask if they want to process seating immediately
        std::cout << "\nDo you want to process seating for this spectator immediately? (y/n): ";
        char processChoice;
        std::cin >> processChoice;
        
        if (processChoice == 'y' || processChoice == 'Y') {
            std::cout << "\n=== Processing Seating for Checked-In Spectator ===\n";
            seatingManager->processEntryQueue(false);  // verbose = false for cleaner output
            seatingManager->displaySeatingStatus();
        }
    }
}
