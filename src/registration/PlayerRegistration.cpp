#include <iostream>
#include <string>
#include "functions/PlayerRegistration.hpp"

PlayerRegistration::PlayerRegistration() : nextPlayerId(1) {}

void PlayerRegistration::registerPlayer() {
    std::string name, email, phone, dateJoined;
    int age, rank;
    char genderChar;
    Gender gender;
    bool isWildcard;

    std::cout << "Enter player name: ";
    std::cin.ignore();
    std::getline(std::cin, name);

    std::cout << "Enter age: ";
    std::cin >> age;

    std::cout << "Enter gender (M/F): ";
    std::cin >> genderChar;
    gender = (genderChar == 'M' || genderChar == 'm') ? Gender::Male : Gender::Female;

    std::cout << "Enter email: ";
    std::cin >> email;

    std::cout << "Enter phone number: ";
    std::cin >> phone;

    rank = 0;

    std::cout << "Is this a wildcard entry? (1 for Yes, 0 for No): ";
    std::cin >> isWildcard;

    std::cout << "Enter date joined (YYYY-MM-DD): ";
    std::cin >> dateJoined;

    Player p(nextPlayerId++, name, age, gender, email, phone, rank, isWildcard, dateJoined, -1);
    registrationQueue.enqueue(p);

    std::cout << "Player registered and added to the queue.\n";
}

void PlayerRegistration::checkInPlayer() {
    if (registrationQueue.isEmpty()) {
        std::cout << "No players in the queue.\n";
        return;
    }

    Player p = registrationQueue.dequeue();
    std::cout << "Checked in Player: " << p.name << " (ID: " << p.id << ")\n";
}

void PlayerRegistration::displayQueue() {
    std::cout << "\nCurrent Registration Queue:\n";
    registrationQueue.display();
}
