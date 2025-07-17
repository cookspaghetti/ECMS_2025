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
#include "SeatingManager.hpp"

int main() {
    // === Test DTOs ===
    std::cout << "=== DTO Test ===\n";

    Performance perf1(1, 20, Champion::Yasuo, "2025-07-01", 75.0f);
    Player player1(1, "Alex", 21, Gender::Male, "alex@apu.edu.my", "0123456789", 5, "2023-03-14", 1);
    Tournament tour1(1, "APUEC 2025", "2025-08-01", "2025-08-15", "KL Campus", TournamentCategory::Regional);
    Match match1(1, 1, MatchType::BestOf3, "2025-08-03", "14:00", 1, 2, 1);
    
    Champion champs1[Result::TEAM_SIZE] = { Champion::Yasuo, Champion::LeeSin, Champion::Ahri, Champion::Lux, Champion::Ezreal };
    Champion champs2[Result::TEAM_SIZE] = { Champion::Zed, Champion::Katarina, Champion::Teemo, Champion::Morgana, Champion::Vayne };
    Result result1(1, 1, 3, "2-1", champs1, champs2, 1);

    Spectator spectator1(1, "Jamie", Gender::Female, "jamie@stream.com", "0198765432", SpectatorType::Streamer, "Twitch");

    std::cout << "Player: " << player1.name << ", Rank: " << player1.rank << ", Favourite Champ: Yasuo\n";
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
    std::cout << "Top Result ID: " << resultStack.peek().id << ", Score: " << resultStack.peek().score << "\n\n";

    // === TASK 3: LIVE STREAM & SPECTATOR QUEUE MANAGEMENT ===
    std::cout << "=== TASK 3: SEATING & SPECTATOR MANAGEMENT ===\n";

    // Create seating manager with capacity: VIP(5), Influencer(3), Streaming(4), General(20)
    SeatingManager seatingManager(5, 3, 4, 20);

    // Create various types of spectators
    Spectator vip1(101, "VIP Alice", Gender::Female, "alice@vip.com", "0123456789", SpectatorType::VIP, "Premium Sponsor");
    Spectator vip2(102, "VIP Bob", Gender::Male, "bob@vip.com", "0123456790", SpectatorType::VIP, "Tournament Sponsor");
    Spectator vip3(103, "VIP Charlie", Gender::Male, "charlie@vip.com", "0123456791", SpectatorType::VIP, "Major Sponsor");

    Spectator influencer1(201, "Influencer Dana", Gender::Female, "dana@youtube.com", "0134567890", SpectatorType::Influencer, "YouTube Gaming");
    Spectator influencer2(202, "Influencer Eve", Gender::Female, "eve@tiktok.com", "0134567891", SpectatorType::Influencer, "TikTok Gaming");

    Spectator streamer1(301, "Streamer Felix", Gender::Male, "felix@twitch.tv", "0145678901", SpectatorType::Streamer, "Twitch");
    Spectator streamer2(302, "Streamer Grace", Gender::Female, "grace@youtube.com", "0145678902", SpectatorType::Streamer, "YouTube");
    Spectator streamer3(303, "Streamer Henry", Gender::Male, "henry@facebook.com", "0145678903", SpectatorType::Streamer, "Facebook Gaming");

    Spectator normal1(401, "John Normal", Gender::Male, "john@email.com", "0156789012", SpectatorType::Normal, "General Public");
    Spectator normal2(402, "Jane Normal", Gender::Female, "jane@email.com", "0156789013", SpectatorType::Normal, "General Public");
    Spectator normal3(403, "Jack Normal", Gender::Male, "jack@email.com", "0156789014", SpectatorType::Normal, "General Public");

    // Add spectators to entry queue (they'll be processed by priority)
    std::cout << "\n--- Adding Spectators to Entry Queue ---\n";
    seatingManager.addToEntryQueue(normal1);     // Priority 1
    seatingManager.addToEntryQueue(vip1);        // Priority 10
    seatingManager.addToEntryQueue(streamer1);   // Priority 7
    seatingManager.addToEntryQueue(influencer1); // Priority 8
    seatingManager.addToEntryQueue(normal2);     // Priority 1
    seatingManager.addToEntryQueue(vip2);        // Priority 10
    seatingManager.addToEntryQueue(streamer2);   // Priority 7

    // Process the entry queue (higher priority spectators get seated first)
    seatingManager.processEntryQueue();

    // Display current status
    seatingManager.displaySeatingStatus();

    // Add more spectators to test overflow management
    std::cout << "\n--- Testing Overflow Management ---\n";
    seatingManager.addToEntryQueue(vip3);
    seatingManager.addToEntryQueue(influencer2);
    seatingManager.addToEntryQueue(streamer3);
    seatingManager.addToEntryQueue(normal3);

    seatingManager.processEntryQueue();
    seatingManager.displaySeatingStatus();
    seatingManager.displayOverflowStatus();

    // Demonstrate overflow management (simulating some spectators leaving)
    std::cout << "\n--- Simulating Spectator Departure and Overflow Processing ---\n";
    std::cout << "Some VIPs left early, checking overflow...\n";
    seatingManager.manageOverflow();
    seatingManager.displaySeatingStatus();

    std::cout << "\nAll structures and DTOs instantiated successfully.\n";

    return 0;
}
