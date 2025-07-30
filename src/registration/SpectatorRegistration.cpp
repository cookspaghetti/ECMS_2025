// File: functions/SpectatorRegistration.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <nlohmann/json.hpp>
#include "functions/SpectatorRegistration.hpp"

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
    dataLoaded(false)
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
        std::cerr << "▶ Could not open " << filename << "\n";
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
        std::cout << "▶ Loaded " << allSpectators.getSize() << " spectators.\n";
    }
    catch (std::exception &e) {
        std::cerr << "▶ Error parsing " << filename << ": " << e.what() << "\n";
    }
}

void SpectatorRegistration::loadCheckIns(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "▶ Could not open " << filename << "; skipping auto-enqueue.\n";
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
        std::cout << "▶ Enqueued " << count << " checked-in spectators.\n";
        dataLoaded = true;
    }
    catch (std::exception &e) {
        std::cerr << "▶ Error parsing " << filename << ": " << e.what() << "\n";
    }
}

void SpectatorRegistration::registerSpectator() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string idStr, name, email, phone, affiliation;
    int         genderChoice, typeChoice;

    std::cout << "\nID (S#####): ";
    std::getline(std::cin, idStr);
    int id = (idStr.size()>1 && idStr[0]=='S')
             ? std::stoi(idStr.substr(1))
             : std::stoi(idStr);

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
    std::cout << "▶ Registered “" << name << "” (ID " << idStr << ").\n";
}

void SpectatorRegistration::checkInSpectator() {
    if (!dataLoaded) {
        std::cerr << "▶ Data not loaded yet.\n";
        return;
    }
    std::cout << "\n=== Available Spectators ===\n";
    for (int i = 0; i < allSpectators.getSize(); ++i) {
        auto* p = allSpectators.get(i);
        std::cout << i+1 << ") " << p->name << " (ID S"
                  << std::setw(5) << std::setfill('0') << p->id << ")\n";
    }
    std::cout << "Select # to check in (0 to cancel): ";
    int choice; std::cin >> choice;
    if (choice<1 || choice>allSpectators.getSize()) {
        std::cout << "▶ Cancelled.\n";
        return;
    }
    Spectator s = *allSpectators.get(choice-1);
    seatingManager->addToEntryQueue(s, false);
    std::cout << "▶ \"" << s.name << "\" enqueued for seating.\n";
    std::cout << "Seat now? (y/n): ";
    char c; std::cin >> c;
    if (c=='y'||c=='Y') displayQueue();
}

void SpectatorRegistration::displayQueue() {
    if (!dataLoaded) {
        std::cerr << "▶ No data loaded; aborting seating pass.\n";
        return;
    }
    seatingManager->processEntryQueue(true);
    seatingManager->displaySeatingStatus();
    if (seatingManager->hasOverflow()) {
        seatingManager->handleOverflow();
        seatingManager->displaySeatingStatus();
    }
}
