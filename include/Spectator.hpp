#ifndef SPECTATOR_HPP
#define SPECTATOR_HPP

#include <string>

class Spectator {
public:
    int id;
    std::string name;
    char gender;
    std::string email;
    std::string phoneNum;
    std::string type; // Normal, VIP, or Streamer
    std::string affiliation; // Straming Platform, Team, or Organization

    Spectator();
    Spectator(int id, const std::string& name, char gender, const std::string& email,
              const std::string& phoneNum, const std::string& type, const std::string& affiliation);
};

#endif
