#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>

class Match {
public:
    int id;
    std::string matchType; // Best of 1, Best of 3, Best of 5
    std::string date;
    std::string time;
    int player1;
    int player2;
    int resultId; // pointer to Result

    Match();
    Match(int id, const std::string& matchType, const std::string& date, const std::string& time,
          int player1, int player2, int resultId);
};

#endif
