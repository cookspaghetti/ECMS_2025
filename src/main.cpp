#include <iostream>
#include <string>

#include "Performance.hpp"
#include "Player.hpp"
#include "Tournament.hpp"
#include "Match.hpp"
#include "Result.hpp"
#include "Spectator.hpp"
#include "Queue.hpp"
#include "CircularQueue.hpp"
#include "PriorityQueue.hpp"
#include "Stack.hpp"

int main() {
    // === Test DTOs ===
    std::cout << "=== DTO Test ===\n";

    Performance perf1(1, 20, Champion::Yasuo, "2025-07-01", 75.0f);
    Player player1(1, "Alex", 21, Gender::Male, "alex@apu.edu.my", "0123456789", 5, false, "2023-03-14", 1);
    Tournament tour1(1, "APUEC 2025", "2025-08-01", "2025-08-15", "KL Campus", TournamentStage::Qualifiers, TournamentCategory::Regional);
    Match match1(1, 1, TournamentStage::Qualifiers, MatchType::BestOf3, "2025-08-03", "14:00", 1, 2, 1);
    
    Champion champs1[Result::TEAM_SIZE] = { Champion::Yasuo, Champion::LeeSin, Champion::Ahri, Champion::Lux, Champion::Ezreal };
    Champion champs2[Result::TEAM_SIZE] = { Champion::Zed, Champion::Katarina, Champion::Teemo, Champion::Morgana, Champion::Vayne };
    Result result1(1, 1, 3, "2-1", champs1, champs2, 1);

    Spectator spectator1(1, "Jamie", Gender::Female, "jamie@stream.com", "0198765432", SpectatorType::Streamer, "Twitch");

    std::cout << "Player: " << player1.name << ", Rank: " << player1.rank << ", Favourite Champ: Yasuo\n";
    std::cout << "Tournament: " << tour1.name << ", Category: Regional, Location: " << tour1.location << "\n";
    std::cout << "Result: " << result1.score << ", Winner ID: " << result1.winnerId << "\n";
    std::cout << "Spectator: " << spectator1.name << ", Type: " << (spectator1.type == SpectatorType::Streamer ? "Streamer" : "Normal") << "\n\n";
    std::cout << "Match: " << match1.date << " " << match1.time << ", Type: BestOf3\n";
    std::cout << "Spectator: " << spectator1.name << ", Type: Streamer\n\n";

    // === Test Queue ===
    std::cout << "=== Queue Test ===\n";
    Queue<Player> playerQueue(3);
    playerQueue.enqueue(player1);
    std::cout << "Queued Player: " << playerQueue.peek().name << "\n\n";

    // === Test CircularQueue ===
    std::cout << "=== CircularQueue Test ===\n";
    CircularQueue<Spectator> specQueue(2);
    specQueue.enqueue(spectator1);
    std::cout << "First Spectator: " << specQueue.peek().name << "\n\n";

    // === Test PriorityQueue ===
    std::cout << "=== PriorityQueue Test ===\n";
    PriorityQueue<Spectator> vipQueue(3);
    Spectator vip(2, "VIP John", Gender::Male, "john@vip.com", "0111222333", SpectatorType::VIP, "RedBull");
    Spectator normal(3, "Norma", Gender::Female, "norma@xyz.com", "0133334444", SpectatorType::Normal, "None");

    vipQueue.enqueue(spectator1, 2);  // Streamer
    vipQueue.enqueue(vip, 5);         // VIP
    vipQueue.enqueue(normal, 1);      // Normal

    std::cout << "Highest Priority Spectator: " << vipQueue.peek().name << "\n\n";

    // === Test Stack ===
    std::cout << "=== Stack Test ===\n";
    Stack<Result> resultStack(2);
    resultStack.push(result1);
    std::cout << "Top Result ID: " << resultStack.peek().id << ", Score: " << resultStack.peek().score << "\n";

    std::cout << "\nAll structures and DTOs instantiated successfully.\n";

    return 0;
}
