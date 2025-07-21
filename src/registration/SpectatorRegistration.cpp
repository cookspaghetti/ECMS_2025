#include <iostream>
#include <string>
#include "functions/SpectatorRegistration.hpp"
#include "functions/SeatingManager.hpp"
#include "dto/Spectator.hpp"

SpectatorRegistration::SpectatorRegistration() 
    : nextSpectatorId(1001), seatingManager(new SeatingManager(5, 3, 4, 20)) {
    // Initialize with starting ID and seating capacities:
    // VIP: 5 seats, Influencer: 3 seats, Streaming: 4 seats, General: 20 seats
    // Dummy data will be loaded when user selects "Display Queue"
}

SpectatorRegistration::~SpectatorRegistration() {
    delete seatingManager;
}

void SpectatorRegistration::loadDummyData() {
    // Create some dummy spectators to demonstrate the system
    Spectator vip1(1001, "Alice Johnson", Gender::Female, "alice@email.com", "123-456-7890", SpectatorType::VIP, "Premium Gaming Club");
    Spectator streamer1(1002, "StreamerPro", Gender::Male, "streamer@twitch.tv", "123-456-7891", SpectatorType::Streamer, "Twitch");
    Spectator influencer1(1003, "GameInfluencer", Gender::Female, "influence@youtube.com", "123-456-7892", SpectatorType::Influencer, "YouTube Gaming");
    Spectator player1(1004, "ProGamer123", Gender::Male, "progamer@esports.com", "123-456-7893", SpectatorType::Player, "Team Alpha");
    Spectator normal1(1005, "John Doe", Gender::Male, "john@email.com", "123-456-7894", SpectatorType::Normal, "General Public");
    Spectator normal2(1006, "Jane Smith", Gender::Female, "jane@email.com", "123-456-7895", SpectatorType::Normal, "General Public");
    
    // Add them to the registration queue
    registrationQueue.enqueue(vip1);
    registrationQueue.enqueue(streamer1);
    registrationQueue.enqueue(influencer1);
    registrationQueue.enqueue(player1);
    registrationQueue.enqueue(normal1);
    registrationQueue.enqueue(normal2);
    
    // Update the next ID counter
    nextSpectatorId = 1007;
    
    std::cout << " Loaded 6 dummy spectators for demonstration\n";
}

void SpectatorRegistration::registerSpectator() {
    std::string name, email, phone, affiliation;
    int genderChoice, typeChoice;
    Gender gender;
    SpectatorType type;

    std::cout << "\n=== Register New Spectator ===\n";
    
    std::cout << "Enter spectator name: ";
    std::cin.ignore();
    std::getline(std::cin, name);

    std::cout << "Enter email: ";
    std::getline(std::cin, email);

    std::cout << "Enter phone number: ";
    std::getline(std::cin, phone);

    std::cout << "Enter affiliation (Platform/Organization): ";
    std::getline(std::cin, affiliation);

    std::cout << "Select gender:\n";
    std::cout << "1. Male\n";
    std::cout << "2. Female\n";
    std::cout << "Choice: ";
    std::cin >> genderChoice;
    gender = (genderChoice == 2) ? Gender::Female : Gender::Male;

    std::cout << "\nSelect spectator type:\n";
    std::cout << "1. Normal Spectator\n";
    std::cout << "2. VIP Guest\n";
    std::cout << "3. Live Streamer\n";
    std::cout << "4. Influencer\n";
    std::cout << "5. Tournament Player\n";
    std::cout << "Choice: ";
    std::cin >> typeChoice;

    switch (typeChoice) {
        case 2: 
            type = SpectatorType::VIP; 
            std::cout << "VIP privileges: Priority seating, exclusive access\n";
            break;
        case 3: 
            type = SpectatorType::Streamer; 
            std::cout << "Streamer setup: Dedicated streaming area with equipment support\n";
            break;
        case 4: 
            type = SpectatorType::Influencer; 
            std::cout << "Influencer access: Content creation facilities\n";
            break;
        case 5: 
            type = SpectatorType::Player; 
            std::cout << "Player privileges: High priority seating\n";
            break;
        default: 
            type = SpectatorType::Normal; 
            std::cout << "General admission seating\n";
            break;
    }

    Spectator newSpectator(nextSpectatorId++, name, gender, email, phone, type, affiliation);
    registrationQueue.enqueue(newSpectator);
    
    // Also add to seating manager entry queue
    seatingManager->addToEntryQueue(newSpectator);

    std::cout << "\n Spectator '" << name << "' (ID: " << newSpectator.id 
              << ", Type: " << toString(type) << ") registered successfully!\n";
    std::cout << "Added to registration queue and seating system (runtime storage only).\n";
}

void SpectatorRegistration::checkInSpectator() {
    if (registrationQueue.isEmpty()) {
        std::cout << "\n No spectators in the registration queue.\n";
        return;
    }

    Spectator spectator = registrationQueue.dequeue();
    std::cout << "\n Checked in Spectator: " << spectator.name 
              << " (ID: " << spectator.id 
              << ", Type: " << toString(spectator.type) << ")\n";
    
    std::cout << "Spectator is ready for seating assignment.\n";
}

void SpectatorRegistration::displayQueue() {
    std::cout << "\n=== Live Stream & Spectator Queue ===\n";
    
    // Load dummy data for overflow demonstration
    seatingManager->loadDummyData();
    
    if (registrationQueue.isEmpty()) {
        std::cout << "Queue is empty - no spectators waiting.\n";
    } else {
        std::cout << "Current spectators in registration queue:\n";
        std::cout << "Queue contains " << registrationQueue.size() << " spectators waiting to be processed.\n";
        std::cout << "Use 'Check-In Spectator' to process them one by one.\n";
        
        // Show queue summary by spectator type
        std::cout << "\n Queue Summary by Type:\n";
        std::cout << "Note: Use Check-In to process spectators and see individual details\n";
    }
    
    // Display actual seating information from SeatingManager
    seatingManager->displaySeatingStatus();
    seatingManager->displayOverflowStatus();
}

void SpectatorRegistration::displaySeatingStatus() {
    std::cout << "\n=== Live Stream & Spectator Seating Status ===\n";
    seatingManager->displaySeatingStatus();
    seatingManager->displayOverflowStatus();
    
    std::cout << "\n Seating System Features:\n";
    std::cout << "Priority-based assignment (VIP > Players > Influencers > Streamers > Normal)\n";
    std::cout << "Dedicated areas for content creators and streamers\n";
    std::cout << "Automatic overflow management for capacity control\n";
    std::cout << "Real-time seat availability tracking\n";
}

bool SpectatorRegistration::hasWaitingSpectators() const {
    return !registrationQueue.isEmpty();
}

Spectator SpectatorRegistration::getNextSpectator() {
    if (registrationQueue.isEmpty()) {
        return Spectator(); // Return empty spectator if queue is empty
    }
    return registrationQueue.dequeue();
}
