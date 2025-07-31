// File: functions/SpectatorRegistration.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>
#include <nlohmann/json.hpp>
#include "functions/SpectatorRegistration.hpp"
#include "helper/JsonWriter.hpp"

using json = nlohmann::json;

SpectatorRegistration::SpectatorRegistration()
  : allSpectators(),
    seatingManager(new SeatingManager(
        /*vipCapacity*/       50,  // 50 VIP seats total (5 rooms × 10 seats each)
        /*influencerCapacity*/ 30,  // 30 influencer seats (accommodate 25 + some extra)
        /*numStreamRooms*/     5,   // 5 streaming rooms (as per original design)
        /*seatsPerStreamRoom*/ 1,   // 1 seat per streaming room (1 streamer per room)
        /*generalCapacity*/  150    // 150 general seats
    )),
    dataLoaded(false),
    nextSpectatorId(1)
{
    loadSpectatorsFromJSON("../data/spectators.json");
    loadCheckIns("../data/check_in.json");
}

SpectatorRegistration::~SpectatorRegistration() {
    delete seatingManager;
}

void SpectatorRegistration::loadSpectatorsFromJSON(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << " Could not open " << filename << "\n";
        return;
    }
    try {
        json arr; in >> arr; in.close();
        for (auto &item : arr) {
            std::string idStr      = item["id"];
            int         id         = std::stoi(idStr.substr(1));
            std::string name       = item["name"];
            Gender      gender     = (item["gender"]=="Female" ? Gender::Female : Gender::Male);
            std::string email      = item["email"];
            std::string phone      = item["phoneNum"];
            std::string typeStr    = item["type"];
            SpectatorType type      = SpectatorType::Normal;
            if (typeStr=="VIP")         type = SpectatorType::VIP;
            else if (typeStr=="Streamer")   type = SpectatorType::Streamer;
            else if (typeStr=="Influencer") type = SpectatorType::Influencer;
            else if (typeStr=="Player")     type = SpectatorType::Player;
            std::string affiliation = item["affiliation"];

            allSpectators.append({ id, name, gender, email, phone, type, affiliation });
        }
        // std::cout << "[INFO] Loaded " << allSpectators.getSize() << " spectators.\n";
        
        // Update nextSpectatorId to be one more than the highest existing ID
        int maxId = 0;
        for (int i = 0; i < allSpectators.getSize(); ++i) {
            auto* s = allSpectators.get(i);
            if (s && s->id > maxId) {
                maxId = s->id;
            }
        }
        nextSpectatorId = maxId + 1;
    }
    catch (std::exception &e) {
        std::cerr << " Error parsing " << filename << ": " << e.what() << "\n";
    }
}

void SpectatorRegistration::loadCheckIns(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << " Could not open " << filename << "; skipping auto-enqueue.\n";
        dataLoaded = true;
        return;
    }
    try {
        json arr; in >> arr; in.close();
        int count = 0;
        for (auto &obj : arr) {
            if (!obj["checkedIn"].get<bool>()) continue;
            std::string sid = obj["spectatorId"];
            
            // Find spectator by ID
            Spectator* spectatorPtr = nullptr;
            for (int i = 0; i < allSpectators.getSize(); ++i) {
                auto* s = allSpectators.get(i);
                if (s) {
                    // Format spectator ID with leading zeros to match check-in format
                    char formattedId[10];
                    sprintf(formattedId, "S%05d", s->id);
                    if (std::string(formattedId) == sid) {
                        spectatorPtr = s;
                        break;
                    }
                }
            }
            
            if (spectatorPtr) {
                seatingManager->addToEntryQueue(*spectatorPtr, false);
                ++count;
            }
        }
        // std::cout << "[INFO] Enqueued " << count << " checked-in spectators.\n";
        dataLoaded = true;
    }
    catch (std::exception &e) {
        std::cerr << " Error parsing " << filename << ": " << e.what() << "\n";
    }
}

std::string SpectatorRegistration::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void SpectatorRegistration::updateCheckInFile(const Spectator& spectator) {
    // Load existing check-in data
    json checkInArray = json::array();
    std::ifstream inFile("../data/check_in.json");
    if (inFile.is_open()) {
        try {
            inFile >> checkInArray;
        } catch (...) {
            checkInArray = json::array();
        }
        inFile.close();
    }
    
    // Format spectator ID
    char formattedId[10];
    sprintf(formattedId, "S%05d", spectator.id);
    
    // Check if spectator already has a check-in record
    bool found = false;
    for (auto& entry : checkInArray) {
        if (entry["spectatorId"] == std::string(formattedId)) {
            entry["checkedIn"] = true;
            entry["checkInDateTime"] = getCurrentDateTime();
            auto dateTime = getCurrentDateTime();
            entry["checkInDate"] = dateTime.substr(0, 10);  // Extract date part
            entry["checkInTime"] = dateTime.substr(11);     // Extract time part
            found = true;
            break;
        }
    }
    
    // If not found, create new entry
    if (!found) {
        json newEntry;
        newEntry["spectatorId"] = std::string(formattedId);
        newEntry["name"] = spectator.name;
        newEntry["type"] = (spectator.type == SpectatorType::VIP) ? "VIP" :
                          (spectator.type == SpectatorType::Streamer) ? "Streamer" :
                          (spectator.type == SpectatorType::Influencer) ? "Influencer" :
                          (spectator.type == SpectatorType::Player) ? "Player" : "Normal";
        newEntry["affiliation"] = spectator.affiliation;
        newEntry["checkedIn"] = true;
        auto dateTime = getCurrentDateTime();
        newEntry["checkInDateTime"] = dateTime;
        newEntry["checkInDate"] = dateTime.substr(0, 10);
        newEntry["checkInTime"] = dateTime.substr(11);
        
        checkInArray.push_back(newEntry);
    }
    
    // Write back to file
    std::ofstream outFile("../data/check_in.json");
    if (outFile.is_open()) {
        outFile << checkInArray.dump(4);
        outFile.close();
        // std::cout << "[INFO] Check-in data saved to file.\n";
    } else {
        std::cerr << " Failed to save check-in data to file.\n";
    }
}

void SpectatorRegistration::registerSpectator() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string name, email, phone, affiliation;
    int         genderChoice, typeChoice;

    // Auto-generate ID - no need for user input
    int id = nextSpectatorId++;
    std::cout << "\nAuto-generated ID: S" << std::setfill('0') << std::setw(5) << id << "\n";

    std::cout << "Name: ";
    std::getline(std::cin, name);

    std::cout << "Gender (1=Male,2=Female): ";
    std::cin >> genderChoice; std::cin.ignore();
    Gender gender = (genderChoice==2) ? Gender::Female : Gender::Male;

    std::cout << "Email: ";
    std::getline(std::cin, email);

    std::cout << "Phone: ";
    std::getline(std::cin, phone);

    std::cout << "Type (1=Normal,2=VIP,3=Streamer,4=Influencer,5=Player): ";
    std::cin >> typeChoice; std::cin.ignore();
    SpectatorType type = SpectatorType::Normal;
    if (typeChoice==2) type = SpectatorType::VIP;
    else if (typeChoice==3) type = SpectatorType::Streamer;
    else if (typeChoice==4) type = SpectatorType::Influencer;
    else if (typeChoice==5) type = SpectatorType::Player;

    std::cout << "Affiliation: ";
    std::getline(std::cin, affiliation);

    allSpectators.append({ id, name, gender, email, phone, type, affiliation });
    
    // Save the new spectator to JSON file
    Spectator newSpectator = { id, name, gender, email, phone, type, affiliation };
    if (JsonWriter::appendSpectator(newSpectator, "../data/spectators.json")) {
        std::cout << " Registered \"" << name << "\" (ID S" << std::setfill('0') << std::setw(5) << id << ") and saved to file.\n";
    } else {
        std::cout << " Registered \"" << name << "\" (ID S" << std::setfill('0') << std::setw(5) << id << ") but failed to save to file.\n";
    }
}

void SpectatorRegistration::checkInSpectator() {
    if (!dataLoaded) {
        std::cerr << " Data not loaded yet.\n";
        return;
    }
    std::cout << "\n=== Spectator Check-In ===\n";
    std::cout << "Total spectators available: " << allSpectators.getSize() << "\n";
    std::cout << "Select spectator # to check in (1-" << allSpectators.getSize() << ", 0 to cancel): ";
    int choice; std::cin >> choice;
    if (choice<1 || choice>allSpectators.getSize()) {
        std::cout << " Cancelled.\n";
        return;
    }
    Spectator s = *allSpectators.get(choice-1);
    
    // Add to seating queue
    seatingManager->addToEntryQueue(s, false);
    
    // Save check-in data to file
    updateCheckInFile(s);
    
    std::cout << "Check-in successful.\n";
    std::cout << "Seat now? (y/n): ";
    char c; std::cin >> c;
    if (c=='y'||c=='Y') displayQueue();
}

void SpectatorRegistration::displayQueue() {
    if (!dataLoaded) {
        std::cerr << " No data loaded; aborting seating pass.\n";
        return;
    }
    seatingManager->processEntryQueue(true);
    seatingManager->displaySeatingStatus();
    if (seatingManager->hasOverflow()) {
        seatingManager->handleOverflow();
        seatingManager->displaySeatingStatus();
    }
}
