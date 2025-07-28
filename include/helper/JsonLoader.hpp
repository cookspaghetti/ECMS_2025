#ifndef JSONLOADER_HPP
#define JSONLOADER_HPP

#include <string>
#include "structures/DoublyLinkedList.hpp"
#include "dto/Player.hpp"
#include "dto/Match.hpp"
#include "dto/Performance.hpp"
#include "dto/Result.hpp"
#include "dto/Spectator.hpp"
#include "dto/Tournament.hpp"

class JsonLoader {
public:
    static DoublyLinkedList<Player> loadPlayers(const std::string& filename);
    static DoublyLinkedList<Match> loadMatches(const std::string& filename);
    static DoublyLinkedList<Performance> loadPerformances(const std::string& filename);
    static DoublyLinkedList<Result> loadResults(const std::string& filename);
    static DoublyLinkedList<Spectator> loadSpectators(const std::string& filename);
    static DoublyLinkedList<Tournament> loadTournaments(const std::string& filename);
};

#endif
