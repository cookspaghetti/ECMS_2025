#ifndef TOURNAMENT_MANAGER_HPP
#define TOURNAMENT_MANAGER_HPP

#include <string>
#include "dto/Tournament.hpp"
#include "structures/DoublyLinkedList.hpp"
#include "helper/JsonLoader.hpp"
#include "helper/JsonWriter.hpp"

class TournamentManager {
private:
    DoublyLinkedList<Tournament> tournaments;
    JsonLoader jsonLoader;
    JsonWriter jsonWriter;
    Tournament* registeringTournament;
    Tournament* ongoingTournament;
    
    // Helper methods
    void loadTournamentsFromFile();
    bool hasDuplicateTournament(const std::string& name, const std::string& startDate, const std::string& endDate) const;
    std::string formatDate(const std::string& date) const;
    bool isValidDate(const std::string& date) const;

public:
    // Constructor and destructor
    TournamentManager();
    ~TournamentManager();
    
    // Core tournament management functions
    void createTournament();
    void displayTournamentDetails();
    void displayAllTournaments() const;
    
    // Tournament status management
    bool hasRegisteringTournament();
    Tournament* getRegisteringTournament();
    bool hasOngoingTournament();
    Tournament* getOngoingTournament();
    void endCurrentTournament();
    bool endRegisteringTournament(const int currentParticipants);
    
    // Participant count management
    bool incrementParticipantCount();
    bool decrementParticipantCount();
    
    // Tournament search and retrieval
    Tournament* findTournamentById(const std::string& id) const;
    Tournament* findTournamentByName(const std::string& name) const;
    DoublyLinkedList<Tournament> getTournamentsByCategory(TournamentCategory category) const;
    DoublyLinkedList<Tournament> getTournamentsByStage(TournamentStage stage) const;
    
    // Data persistence
    void saveTournaments();
    void reloadData();
    
    // Utility functions
    int getTournamentCount() const;
    void displayTournamentStatistics() const;
    bool deleteTournament(const std::string& tournamentId);
    bool isValidParticipantCount(int count) const;
};

#endif
