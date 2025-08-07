#include "functions/TournamentManager.hpp"
#include "general/Enum.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <regex>
#include <array>

TournamentManager::TournamentManager() : registeringTournament(nullptr), ongoingTournament(nullptr) {
    loadTournamentsFromFile();
}

TournamentManager::~TournamentManager() {
    // Clean up any dynamically allocated memory if needed
    delete registeringTournament;
    delete ongoingTournament;
}

void TournamentManager::loadTournamentsFromFile() {
    tournaments.clear();
    
    try {
        DoublyLinkedList<Tournament> loadedTournaments = jsonLoader.loadTournaments("data/tournaments.json");

        for (int i = 0; i < loadedTournaments.getSize(); i++) {
            Tournament* tournament = loadedTournaments.get(i);
            if (tournament) {
                if (tournament->stage == TournamentStage::Registration) {
                    if (registeringTournament) delete registeringTournament;
                    registeringTournament = new Tournament(*tournament);
                }
                if (tournament->stage == TournamentStage::Qualifiers || 
                    tournament->stage == TournamentStage::Quarterfinals || 
                    tournament->stage == TournamentStage::Semifinals || 
                    tournament->stage == TournamentStage::Finals) {
                    if (ongoingTournament) delete ongoingTournament;
                    ongoingTournament = new Tournament(*tournament);
                }
                tournaments.append(*tournament);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading tournaments: " << e.what() << std::endl;
        std::cout << "No tournaments loaded.\n";
    }
}

bool TournamentManager::hasRegisteringTournament() {
    loadTournamentsFromFile(); // Ensure we have the latest tournament data
    return registeringTournament != nullptr;
}

Tournament* TournamentManager::getRegisteringTournament() {
    loadTournamentsFromFile(); // Ensure we have the latest tournament data
    return registeringTournament;
}

bool TournamentManager::hasOngoingTournament() {
    loadTournamentsFromFile(); // Ensure we have the latest tournament data
    return ongoingTournament != nullptr;
}

Tournament* TournamentManager::getOngoingTournament() {
    loadTournamentsFromFile(); // Ensure we have the latest tournament data
    return ongoingTournament;
}

bool TournamentManager::incrementParticipantCount() {
    if (!hasRegisteringTournament()) {
        std::cout << "No registering tournament found to update participant count.\n";
        return false;
    }
    
    if (registeringTournament->currentParticipants >= registeringTournament->maxParticipants) {
        std::cout << "Tournament is already at maximum capacity (" << registeringTournament->maxParticipants << " participants).\n";
        return false;
    }
    
    registeringTournament->currentParticipants++;
    
    // Update the tournament in the tournaments list as well
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->id == registeringTournament->id) {
            tournament->currentParticipants = registeringTournament->currentParticipants;
            break;
        }
    }
    
    // Auto-save the updated tournament data
    saveTournaments();
    
    std::cout << "Tournament participant count updated: " << registeringTournament->currentParticipants 
              << "/" << registeringTournament->maxParticipants << std::endl;
    
    return true;
}

bool TournamentManager::decrementParticipantCount() {
    if (!hasRegisteringTournament()) {
        std::cout << "No registering tournament found to update participant count.\n";
        return false;
    }
    
    if (registeringTournament->currentParticipants <= 0) {
        std::cout << "Tournament participant count is already at zero.\n";
        return false;
    }
    
    registeringTournament->currentParticipants--;
    
    // Update the tournament in the tournaments list as well
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->id == registeringTournament->id) {
            tournament->currentParticipants = registeringTournament->currentParticipants;
            break;
        }
    }
    
    // Auto-save the updated tournament data
    saveTournaments();
    
    std::cout << "Tournament participant count updated: " << registeringTournament->currentParticipants 
              << "/" << registeringTournament->maxParticipants << std::endl;
    
    return true;
}

bool TournamentManager::hasDuplicateTournament(const std::string& name, const std::string& startDate, const std::string& endDate) const {
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->name == name && tournament->startDate == startDate && tournament->endDate == endDate) {
            return true;
        }
    }
    return false;
}

std::string TournamentManager::formatDate(const std::string& date) const {
    // Basic date formatting - expects DD/MM/YYYY format
    if (date.length() == 10 && date[2] == '/' && date[5] == '/') {
        return date;
    }
    return date; // Return as-is if not in expected format
}

bool TournamentManager::isValidDate(const std::string& date) const {
    // Basic date validation using regex for DD/MM/YYYY format
    std::regex datePattern(R"(\d{2}/\d{2}/\d{4})");
    return std::regex_match(date, datePattern);
}

void TournamentManager::createTournament() {
    std::cout << "\n=== CREATE NEW TOURNAMENT ===\n";

    // Check if there's already a registering tournament
    if (hasRegisteringTournament()) {
        std::cout << "Warning: There is already a registering tournament!\n";
        std::cout << "Registering Tournament: " << registeringTournament->name << " (ID: " << registeringTournament->id << ")\n";
        
        char choice;
        std::cout << "Do you want to end the current tournament and create a new one? (y/n): ";
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == 'y' || choice == 'Y') {
            endCurrentTournament();
        } else {
            std::cout << "Tournament creation cancelled.\n";
            return;
        }
    }
    
    Tournament newTournament;
    
    // Generate new tournament ID
    int maxId = 0;
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && !tournament->id.empty() && tournament->id.length() >= 2) {
            // Extract numeric part from "T00000" format
            std::string numericPart = tournament->id.substr(1);
            try {
                int id = std::stoi(numericPart);
                if (id > maxId) {
                    maxId = id;
                }
            } catch (const std::exception&) {
                // Invalid ID format, skip
            }
        }
    }
    // Generate new tournament ID in T00000 format
    std::string counterStr = std::to_string(maxId + 1);
    newTournament.id = "T" + std::string(5 - counterStr.length(), '0') + counterStr;
    
    // Get tournament details from user
    std::cout << "Enter tournament name: ";
    std::getline(std::cin, newTournament.name);

    // Validate tournament name
    while (newTournament.name.empty()) {
        std::cout << "Tournament name cannot be empty. Please enter a valid name: ";
        std::getline(std::cin, newTournament.name);
    }
    
    std::cout << "Enter tournament start date (DD/MM/YYYY): ";
    std::getline(std::cin, newTournament.startDate);
    std::cout << "Enter tournament end date (DD/MM/YYYY): ";
    std::getline(std::cin, newTournament.endDate);

    // Validate date formats
    while (!isValidDate(newTournament.startDate)) {
        std::cout << "Warning: Date format may be invalid. Expected format: DD/MM/YYYY\n";
        std::cout << "Enter tournament start date (DD/MM/YYYY): ";
        std::getline(std::cin, newTournament.startDate);
    }

    while (!isValidDate(newTournament.endDate)) {
        std::cout << "Warning: Date format may be invalid. Expected format: DD/MM/YYYY\n";
        std::cout << "Enter tournament end date (DD/MM/YYYY): ";
        std::getline(std::cin, newTournament.endDate);
    }

    // Check for duplicates
    if (hasDuplicateTournament(newTournament.name, newTournament.startDate, newTournament.endDate)) {
        std::cout << "Error: A tournament with this name and date already exists!\n";
        return;
    }
    
    std::cout << "Enter tournament location: ";
    std::getline(std::cin, newTournament.location);

    // Validate tournament location
    while (newTournament.location.empty()) {
        std::cout << "Tournament location cannot be empty. Please enter a valid location: ";
        std::getline(std::cin, newTournament.location);
    }
    
    // Tournament category selection
    std::cout << "\nSelect tournament category:\n";
    std::cout << "1. Local\n";
    std::cout << "2. Regional\n";
    std::cout << "3. National\n";
    std::cout << "4. International\n";
    std::cout << "Enter choice (1-4): ";

    int categoryChoice;
    std::cin >> categoryChoice;
    std::cin.ignore();

    while (categoryChoice < 1 || categoryChoice > 4) {
        std::cout << "Invalid choice. Please select a valid category (1-4): ";
        std::cin >> categoryChoice;
        std::cin.ignore();
    }

    switch (categoryChoice) {
        case 1: newTournament.category = TournamentCategory::Local; break;
        case 2: newTournament.category = TournamentCategory::Regional; break;
        case 3: newTournament.category = TournamentCategory::National; break;
        case 4: newTournament.category = TournamentCategory::International; break;
        default: 
            std::cout << "Invalid choice. Setting to Regional.\n";
            newTournament.category = TournamentCategory::Regional;
            break;
    }
    
    newTournament.stage = TournamentStage::Registration;
    
    int maxParticipants;
    std::cout << "Enter maximum participants: ";
    std::cin >> maxParticipants;
    std::cin.ignore();

    while (maxParticipants <= 0 || maxParticipants > 1000 || maxParticipants % 2 != 0 ||
           !isValidParticipantCount(maxParticipants)) {
        std::cout << "Invalid number of participants.\n";
        std::cout << "Enter maximum participants: ";
        std::cin >> maxParticipants;
        std::cin.ignore();
    }

    newTournament.maxParticipants = maxParticipants;

    std::cout << "Enter prize pool: $";
    std::cin >> newTournament.prizePool;
    std::cin.ignore();

    while (newTournament.prizePool < 0) {
        std::cout << "Prize pool cannot be negative. Please enter a valid amount: $";
        std::cin >> newTournament.prizePool;
        std::cin.ignore();
    }
    
    // Set tournament as active by default
    newTournament.currentParticipants = 0;
    
    // Add to tournaments list
    tournaments.append(newTournament);
    if (registeringTournament) delete registeringTournament;
    registeringTournament = new Tournament(newTournament);

    // Save to file
    saveTournaments();
    
    std::cout << "\nTournament created successfully!\n";
    std::cout << "Tournament ID: " << newTournament.id << std::endl;
    std::cout << "Tournament Name: " << newTournament.name << std::endl;
    std::cout << "Stage: " << JsonWriter::tournamentStageToString(newTournament.stage) << std::endl;
}

void TournamentManager::displayTournamentDetails() {
    if (!hasRegisteringTournament()) {
        std::cout << "No registering tournament found.\n";
        std::cout << "Please create a tournament or activate an existing one.\n";
        return;
    }

    std::cout << "\n=== REGISTERING TOURNAMENT DETAILS ===\n";
    std::cout << std::left << std::setw(20) << "Tournament ID:" << registeringTournament->id << std::endl;
    std::cout << std::left << std::setw(20) << "Name:" << registeringTournament->name << std::endl;
    std::cout << std::left << std::setw(20) << "Start Date:" << registeringTournament->startDate << std::endl;
    std::cout << std::left << std::setw(20) << "End Date:" << registeringTournament->endDate << std::endl;
    std::cout << std::left << std::setw(20) << "Location:" << registeringTournament->location << std::endl;

    // Convert enums to strings for display
    std::string categoryStr = JsonWriter::tournamentCategoryToString(registeringTournament->category);
    std::string stageStr = JsonWriter::tournamentStageToString(registeringTournament->stage);

    std::cout << std::left << std::setw(20) << "Category:" << categoryStr << std::endl;
    std::cout << std::left << std::setw(20) << "Stage:" << stageStr << std::endl;
    std::cout << std::left << std::setw(20) << "Participants:" << registeringTournament->currentParticipants 
              << "/" << registeringTournament->maxParticipants << std::endl;
    std::cout << std::left << std::setw(20) << "Prize Pool:" << "$" << std::fixed << std::setprecision(2) 
              << registeringTournament->prizePool << std::endl;
    std::cout << std::left << std::setw(20) << "Status:" << "Active" << std::endl;
    std::cout << "================================ END =================================\n";
}

void TournamentManager::displayAllTournaments() const {
    if (tournaments.getSize() == 0) {
        std::cout << "No tournaments found.\n";
        return;
    }
    
    std::cout << "\n=== ALL TOURNAMENTS ===\n";
    std::cout << std::left << std::setw(5) << "ID" 
              << std::setw(25) << "Name" 
              << std::setw(12) << "Start Date"
              << std::setw(15) << "End Date"
              << std::setw(15) << "Location"
              << std::setw(15) << "Category"
              << std::setw(15) << "Stage" << std::endl;
    std::cout << std::string(107, '-') << std::endl;
    
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament) {
            std::string categoryStr = JsonWriter::tournamentCategoryToString(tournament->category);
            std::string stageStr = JsonWriter::tournamentStageToString(tournament->stage);

            std::cout << std::left << std::setw(5) << tournament->id
                      << std::setw(25) << tournament->name.substr(0, 24)
                      << std::setw(12) << tournament->startDate
                      << std::setw(15) << tournament->endDate
                      << std::setw(15) << tournament->location.substr(0, 14)
                      << std::setw(15) << categoryStr
                      << std::setw(15) << stageStr << std::endl;
        }
    }
    std::cout << "======================== END ========================\n";
}

void TournamentManager::endCurrentTournament() {
    if (!hasRegisteringTournament()) {
        std::cout << "No registering tournament to cancel.\n";
        return;
    }

    std::cout << "Cancelling current tournament: " << registeringTournament->name << " (ID: " << registeringTournament->id << ")\n";

    // Remove the registering tournament from the list
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->id == registeringTournament->id) {
            tournaments.removeAt(i);
            break;
        }
    }

    // Save changes to file
    saveTournaments();
    
    // Clear the active tournament pointer
    delete registeringTournament;
    registeringTournament = nullptr;
    
    std::cout << "Tournament cancelled successfully.\n";
}

bool TournamentManager::endRegisteringTournament(const int currentParticipants) {
    if (!hasRegisteringTournament()) {
        std::cout << "No registering tournament to end.\n";
        return false;
    }

    if (currentParticipants < 2 || currentParticipants > registeringTournament->maxParticipants) {
        std::cout << "Invalid number of participants. Must be between 2 and " 
                  << registeringTournament->maxParticipants << ".\n";
        return false;
    }

    if (currentParticipants % 2 != 0) {
        std::cout << "Number of participants must be even.\n";
        return false;
    }

    if (ongoingTournament) {
        std::cout << "Warning: There is already an ongoing tournament!\n";
        std::cout << "Ongoing Tournament: " << ongoingTournament->name << " (ID: " << ongoingTournament->id << ")\n";
        return false;
    }

    std::cout << "Ending registration for tournament: " << registeringTournament->name << " (ID: " << registeringTournament->id << ")\n";

    // Set the tournament stage to Qualifiers
    registeringTournament->stage = TournamentStage::Qualifiers;
    registeringTournament->currentParticipants = currentParticipants;

    // Update the tournament in the list
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->id == registeringTournament->id) {
            *tournament = *registeringTournament; // Update the existing tournament
            break;
        }
    }

    // Save changes to file
    saveTournaments();

    // Set the ongoing tournament pointer
    if (ongoingTournament) delete ongoingTournament;
    ongoingTournament = new Tournament(*registeringTournament);

    // Clear the registering tournament pointer
    delete registeringTournament;
    registeringTournament = nullptr;

    std::cout << "Tournament registration ended. Tournament is now in Qualifiers stage.\n";
    return true;
}

Tournament* TournamentManager::findTournamentById(const std::string& id) const {
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->id == id) {
            return tournament;
        }
    }
    return nullptr;
}

Tournament* TournamentManager::findTournamentByName(const std::string& name) const {
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->name == name) {
            return tournament;
        }
    }
    return nullptr;
}

DoublyLinkedList<Tournament> TournamentManager::getTournamentsByCategory(TournamentCategory category) const {
    DoublyLinkedList<Tournament> result;
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->category == category) {
            result.append(*tournament);
        }
    }
    return result;
}

DoublyLinkedList<Tournament> TournamentManager::getTournamentsByStage(TournamentStage stage) const {
    DoublyLinkedList<Tournament> result;
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->stage == stage) {
            result.append(*tournament);
        }
    }
    return result;
}

void TournamentManager::saveTournaments() {
    try {
        jsonWriter.writeAllTournaments(tournaments, "data/tournaments.json");
        std::cout << "Tournaments saved successfully.\n";
    } catch (const std::exception& e) {
        std::cout << "Error saving tournaments: " << e.what() << std::endl;
    }
}

void TournamentManager::reloadData() {
    loadTournamentsFromFile();
}

int TournamentManager::getTournamentCount() const {
    return tournaments.getSize();
}

void TournamentManager::displayTournamentStatistics() const {
    if (tournaments.getSize() == 0) {
        std::cout << "No tournaments to display statistics for.\n";
        return;
    }
    
    std::cout << "\n=== TOURNAMENT STATISTICS ===\n";
    std::cout << "Total Tournaments: " << tournaments.getSize() << std::endl;
    
    int activeCount = 0;
    int completedCount = 0;
    double totalPrizePool = 0.0;
    
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament) {
            if (tournament->stage == TournamentStage::Registration) activeCount++;
            if (tournament->stage == TournamentStage::Completed) completedCount++;
            totalPrizePool += tournament->prizePool;
        }
    }
    
    std::cout << "Active Tournaments: " << activeCount << std::endl;
    std::cout << "Completed Tournaments: " << completedCount << std::endl;
    std::cout << "Total Prize Pool: $" << std::fixed << std::setprecision(2) << totalPrizePool << std::endl;
    std::cout << "==============================\n";
}

bool TournamentManager::deleteTournament(const std::string& tournamentId) {
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament && tournament->id == tournamentId) {
            std::string tournamentName = tournament->name;
            
            // If this is the active tournament, clear the active pointer
            if (registeringTournament && registeringTournament->id == tournamentId) {
                delete registeringTournament;
                registeringTournament = nullptr;
            }
            if (ongoingTournament && ongoingTournament->id == tournamentId) {
                delete ongoingTournament;
                ongoingTournament = nullptr;
            }
            
            tournaments.removeAt(i);
            saveTournaments();
            
            std::cout << "Tournament '" << tournamentName << "' has been deleted.\n";
            return true;
        }
    }
    
    std::cout << "Tournament with ID " << tournamentId << " not found.\n";
    return false;
}

bool TournamentManager::isValidParticipantCount(int count) const {
    // create array of valid participant counts
    std::array<int, 4> validCounts = {16, 32, 64, 128};
    return std::find(validCounts.begin(), validCounts.end(), count) != validCounts.end();
}