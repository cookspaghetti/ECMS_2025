#ifndef RESULT_HPP
#define RESULT_HPP

#include <string>
#include "general/Enum.hpp"

class Result {
public:
    std::string id;
    std::string matchId;                            // ID of the associated match
    Champion championsP1;
    Champion championsP2;
    std::string winnerId;                           // ID of the winning player

    Result() : id(""), matchId(""), winnerId("") {
        championsP1 = Champion::NoChampion;
        championsP2 = Champion::NoChampion;
    }

    Result(const std::string& id, const std::string& matchId, Champion championsP1, Champion championsP2, const std::string& winnerId)
        : id(id), matchId(matchId), championsP1(championsP1), championsP2(championsP2), winnerId(winnerId) {
    }
};

inline Champion championFromString(const std::string& str) {
    if (str == "Akali") return Champion::Akali;
    if (str == "Ezreal") return Champion::Ezreal;
    if (str == "Fiora") return Champion::Fiora;
    if (str == "Fizz") return Champion::Fizz;
    if (str == "Garen") return Champion::Garen;
    if (str == "Jax") return Champion::Jax;
    if (str == "Katarina") return Champion::Katarina;
    if (str == "LeeSin") return Champion::LeeSin;
    if (str == "Riven") return Champion::Riven;
    if (str == "Yasuo") return Champion::Yasuo;
    if (str == "Zed") return Champion::Zed;
    return Champion::NoChampion;
}

inline std::string championToString(Champion champion) {
    switch (champion) {
        case Champion::Akali: return "Akali";
        case Champion::Ezreal: return "Ezreal";
        case Champion::Fiora: return "Fiora";
        case Champion::Fizz: return "Fizz";
        case Champion::Garen: return "Garen";
        case Champion::Jax: return "Jax";
        case Champion::Katarina: return "Katarina";
        case Champion::LeeSin: return "LeeSin";
        case Champion::Riven: return "Riven";
        case Champion::Yasuo: return "Yasuo";
        case Champion::Zed: return "Zed";
        case Champion::NoChampion: return "NoChamp";
        default: return "Unknown";
    }
}

#endif
