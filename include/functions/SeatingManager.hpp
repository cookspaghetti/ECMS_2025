#ifndef SEATINGMANAGER_HPP
#define SEATINGMANAGER_HPP

#include "../dto/Spectator.hpp"
#include "../structures/PriorityQueue.hpp"
#include "../structures/Queue.hpp"
#include "../structures/CircularQueue.hpp"
#include <iostream>

class SeatingManager {
private:
    // VIP Section
    Queue<Spectator> vipSeating;
    int vipCapacity;
    int vipOccupied;
    
    // Influencer Section
    Queue<Spectator> influencerSeating;
    int influencerCapacity;
    int influencerOccupied;
    
    // Streaming Setup Area
    Queue<Spectator> streamingArea;
    int streamingCapacity;
    int streamingOccupied;
    
    // General Admission with overflow management
    CircularQueue<Spectator> generalSeating;
    Queue<Spectator> overflowQueue;
    int generalCapacity;
    
    // Priority queue for managing entry based on spectator type
    PriorityQueue<Spectator> entryQueue;

public:
    SeatingManager(int vipCap, int influencerCap, int streamingCap, int generalCap);
    
    // Seating assignment methods
    bool assignSeat(const Spectator& spectator);
    bool assignVIPSeat(const Spectator& spectator);
    bool assignInfluencerSeat(const Spectator& spectator);
    bool assignStreamingSeat(const Spectator& spectator);
    bool assignGeneralSeat(const Spectator& spectator);
    
    // Queue management
    void addToEntryQueue(const Spectator& spectator);
    void processEntryQueue();
    void manageOverflow();
    
    // Utility methods
    void displaySeatingStatus() const;
    void displayOverflowStatus() const;
    int getAvailableSeats(SpectatorType type) const;
    bool hasAvailableSeats(SpectatorType type) const;
    void loadDummyData();  // Load dummy spectators for demonstration
    
    // Priority calculation for different spectator types
    int calculatePriority(SpectatorType type) const;
};

// Constructor
inline SeatingManager::SeatingManager(int vipCap, int influencerCap, int streamingCap, int generalCap) 
    : vipSeating(vipCap), vipCapacity(vipCap), vipOccupied(0),
      influencerSeating(influencerCap), influencerCapacity(influencerCap), influencerOccupied(0),
      streamingArea(streamingCap), streamingCapacity(streamingCap), streamingOccupied(0),
      generalSeating(generalCap), overflowQueue(generalCap * 2), generalCapacity(generalCap),
      entryQueue(generalCap + vipCap + influencerCap + streamingCap) {
    std::cout << "🏟️ SeatingManager initialized with:\n";
    std::cout << "   VIP: " << vipCapacity << " seats\n";
    std::cout << "   Influencer: " << influencerCapacity << " seats\n";
    std::cout << "   Streaming: " << streamingCapacity << " seats\n";
    std::cout << "   General: " << generalCapacity << " seats\n";
    std::cout << "   📝 Note: Use 'Display Queue' option to load dummy data for demonstration\n";
}

// Priority calculation for different spectator types
inline int SeatingManager::calculatePriority(SpectatorType type) const {
    switch (type) {
        case SpectatorType::VIP: return 10;        // Highest priority
        case SpectatorType::Player: return 9;     // Players get VIP treatment
        case SpectatorType::Influencer: return 8; // Influencers next
        case SpectatorType::Streamer: return 7;   // Streamers have priority
        case SpectatorType::Normal: return 1;     // Normal spectators lowest
        default: return 0;
    }
}

// Add spectator to entry queue with calculated priority
inline void SeatingManager::addToEntryQueue(const Spectator& spectator) {
    int priority = calculatePriority(spectator.type);
    entryQueue.enqueue(spectator, priority);
    std::cout << "➕ Added " << spectator.name << " (" << 
                 toString(spectator.type) << ") to entry queue with priority " << priority << "\n";
}

// Process the entry queue and assign seats
inline void SeatingManager::processEntryQueue() {
    std::cout << "\n Processing entry queue...\n";
    
    while (!entryQueue.isEmpty()) {
        Spectator spectator = entryQueue.dequeue();
        std::cout << "Processing: " << spectator.name << " (" << toString(spectator.type) << ")\n";
        
        if (!assignSeat(spectator)) {
            // If we can't assign a seat, add to overflow
            overflowQueue.enqueue(spectator);
            std::cout << "⚠️ " << spectator.name << " added to overflow queue\n";
        }
    }
    
    manageOverflow();
}

// Assign seat based on spectator type and availability
inline bool SeatingManager::assignSeat(const Spectator& spectator) {
    switch (spectator.type) {
        case SpectatorType::VIP:
        case SpectatorType::Player:  // Players compete for VIP seats
            return assignVIPSeat(spectator);
            
        case SpectatorType::Influencer:
            return assignInfluencerSeat(spectator);
            
        case SpectatorType::Streamer:
            return assignStreamingSeat(spectator);
            
        case SpectatorType::Normal:
            return assignGeneralSeat(spectator);
            
        default:
            return false;
    }
}

// Assign VIP seat
inline bool SeatingManager::assignVIPSeat(const Spectator& spectator) {
    if (vipOccupied < vipCapacity) {
        vipSeating.enqueue(spectator);
        vipOccupied++;
        std::cout << " " << spectator.name << " assigned to VIP seat (" << 
                     vipOccupied << "/" << vipCapacity << ")\n";
        return true;
    }
    std::cout << " VIP section full for " << spectator.name << "\n";
    return false;
}

// Assign Influencer seat
inline bool SeatingManager::assignInfluencerSeat(const Spectator& spectator) {
    if (influencerOccupied < influencerCapacity) {
        influencerSeating.enqueue(spectator);
        influencerOccupied++;
        std::cout << " " << spectator.name << " assigned to Influencer seat (" << 
                     influencerOccupied << "/" << influencerCapacity << ")\n";
        return true;
    }
    
    // Try to assign to general seating if influencer section is full
    std::cout << "⚠️ Influencer section full for " << spectator.name << ", trying general seating...\n";
    return assignGeneralSeat(spectator);
}

// Assign Streaming seat
inline bool SeatingManager::assignStreamingSeat(const Spectator& spectator) {
    if (streamingOccupied < streamingCapacity) {
        streamingArea.enqueue(spectator);
        streamingOccupied++;
        std::cout << " " << spectator.name << " assigned to Streaming area (" << 
                     streamingOccupied << "/" << streamingCapacity << ")\n";
        return true;
    }
    
    // Try to assign to general seating if streaming area is full
    std::cout << "⚠️ Streaming area full for " << spectator.name << ", trying general seating...\n";
    return assignGeneralSeat(spectator);
}

// Assign General seat
inline bool SeatingManager::assignGeneralSeat(const Spectator& spectator) {
    if (!generalSeating.isFull()) {
        generalSeating.enqueue(spectator);
        std::cout << " " << spectator.name << " assigned to General seating\n";
        return true;
    }
    std::cout << " General seating full for " << spectator.name << "\n";
    return false;
}

// Manage overflow queue
inline void SeatingManager::manageOverflow() {
    if (!overflowQueue.isEmpty()) {
        std::cout << "\n⚠️ OVERFLOW MANAGEMENT ACTIVATED\n";
        displayOverflowStatus();
    }
}

// Display current seating status
inline void SeatingManager::displaySeatingStatus() const {
    std::cout << "\n🎪 === SEATING STATUS ===\n";
    
    std::cout << "🌟 VIP Section: " << vipOccupied << "/" << vipCapacity << " occupied\n";
    if (vipOccupied > 0) {
        std::cout << "   VIP Guests: ";
        Queue<Spectator> tempVip = vipSeating;
        bool first = true;
        while (!tempVip.isEmpty()) {
            if (!first) std::cout << ", ";
            std::cout << tempVip.dequeue().name;
            first = false;
        }
        std::cout << "\n";
    }
    
    std::cout << "📢 Influencer Section: " << influencerOccupied << "/" << influencerCapacity << " occupied\n";
    if (influencerOccupied > 0) {
        std::cout << "   Influencers: ";
        Queue<Spectator> tempInf = influencerSeating;
        bool first = true;
        while (!tempInf.isEmpty()) {
            if (!first) std::cout << ", ";
            std::cout << tempInf.dequeue().name;
            first = false;
        }
        std::cout << "\n";
    }
    
    std::cout << "📹 Streaming Area: " << streamingOccupied << "/" << streamingCapacity << " occupied\n";
    if (streamingOccupied > 0) {
        std::cout << "   Streamers: ";
        Queue<Spectator> tempStream = streamingArea;
        bool first = true;
        while (!tempStream.isEmpty()) {
            if (!first) std::cout << ", ";
            std::cout << tempStream.dequeue().name;
            first = false;
        }
        std::cout << "\n";
    }
    
    std::cout << "👥 General Seating: " << generalSeating.size() << "/" << generalCapacity;
    if (generalSeating.isFull()) {
        std::cout << " (FULL)";
    }
    std::cout << "\n";
    
    if (!overflowQueue.isEmpty()) {
        displayOverflowStatus();
    }
    
    std::cout << "========================\n";
}

// Display overflow status
inline void SeatingManager::displayOverflowStatus() const {
    if (!overflowQueue.isEmpty()) {
        std::cout << "\n🚨 OVERFLOW QUEUE: " << overflowQueue.size() << " spectators waiting\n";
        std::cout << "   Waiting list: ";
        Queue<Spectator> tempOverflow = overflowQueue;
        bool first = true;
        while (!tempOverflow.isEmpty()) {
            if (!first) std::cout << ", ";
            Spectator spectator = tempOverflow.dequeue();
            std::cout << spectator.name << " (" << toString(spectator.type) << ")";
            first = false;
        }
        std::cout << "\n";
        std::cout << "   💡 These spectators can be accommodated if seats become available\n";
    } else {
        std::cout << " No overflow - all sections within capacity\n";
    }
}

// Get available seats for a specific type
inline int SeatingManager::getAvailableSeats(SpectatorType type) const {
    switch (type) {
        case SpectatorType::VIP:
        case SpectatorType::Player:
            return vipCapacity - vipOccupied;
        case SpectatorType::Influencer:
            return influencerCapacity - influencerOccupied;
        case SpectatorType::Streamer:
            return streamingCapacity - streamingOccupied;
        case SpectatorType::Normal:
            return generalCapacity - generalSeating.size();
        default:
            return 0;
    }
}

// Check if seats are available for a specific type
inline bool SeatingManager::hasAvailableSeats(SpectatorType type) const {
    return getAvailableSeats(type) > 0;
}

// Load dummy data for demonstration with overflow scenarios
inline void SeatingManager::loadDummyData() {
    // Check if data is already loaded to prevent duplicates
    if (vipOccupied > 0 || influencerOccupied > 0 || streamingOccupied > 0 || !generalSeating.isEmpty()) {
        std::cout << "⚠️ Dummy data already loaded. Current seating status:\n";
        return;
    }
    
    std::cout << "\n🎭 Loading comprehensive dummy data for overflow demonstration...\n";
    std::cout << "📊 Target: Exceed all capacity limits to trigger overflow management\n";

    // Create VIP spectators (capacity: 5 - adding 8 to trigger overflow)
    Spectator vip1(101, "VIP Alice", Gender::Female, "alice@vip.com", "0123456789", SpectatorType::VIP, "Premium Sponsor");
    Spectator vip2(102, "VIP Bob", Gender::Male, "bob@vip.com", "0123456790", SpectatorType::VIP, "Tournament Sponsor");
    Spectator vip3(103, "VIP Charlie", Gender::Male, "charlie@vip.com", "0123456791", SpectatorType::VIP, "Major Sponsor");
    Spectator vip4(104, "VIP Diana", Gender::Female, "diana@vip.com", "0123456792", SpectatorType::VIP, "Diamond Sponsor");
    Spectator vip5(105, "VIP Edward", Gender::Male, "edward@vip.com", "0123456793", SpectatorType::VIP, "Elite Sponsor");
    // These should overflow VIP section (3 overflow spectators)
    Spectator vip6(106, "VIP Frank", Gender::Male, "frank@vip.com", "0123456794", SpectatorType::VIP, "Gold Sponsor");
    Spectator vip7(107, "VIP Grace", Gender::Female, "grace@vip.com", "0123456795", SpectatorType::VIP, "Platinum Sponsor");
    Spectator vip8(108, "VIP Henry", Gender::Male, "henry@vip.com", "0123456796", SpectatorType::VIP, "Silver Sponsor");

    // Create Tournament Players (capacity: compete for VIP - adding 3 more)
    Spectator player1(501, "ProPlayer Alpha", Gender::Male, "alpha@esports.com", "0167890123", SpectatorType::Player, "Team Alpha");
    Spectator player2(502, "ProPlayer Beta", Gender::Female, "beta@esports.com", "0167890124", SpectatorType::Player, "Team Beta");
    Spectator player3(503, "ProPlayer Gamma", Gender::Male, "gamma@esports.com", "0167890125", SpectatorType::Player, "Team Gamma");

    // Create Influencer spectators (capacity: 3 - adding 6 to trigger overflow)
    Spectator influencer1(201, "Influencer Dana", Gender::Female, "dana@youtube.com", "0134567890", SpectatorType::Influencer, "YouTube Gaming");
    Spectator influencer2(202, "Influencer Eve", Gender::Female, "eve@tiktok.com", "0134567891", SpectatorType::Influencer, "TikTok Gaming");
    Spectator influencer3(203, "Influencer Sam", Gender::Male, "sam@instagram.com", "0134567892", SpectatorType::Influencer, "Instagram Gaming");
    // These should overflow Influencer section (3 overflow influencers)
    Spectator influencer4(204, "Influencer Luna", Gender::Female, "luna@twitter.com", "0134567893", SpectatorType::Influencer, "Twitter Gaming");
    Spectator influencer5(205, "Influencer Max", Gender::Male, "max@discord.com", "0134567894", SpectatorType::Influencer, "Discord Content");
    Spectator influencer6(206, "Influencer Nova", Gender::Female, "nova@reddit.com", "0134567895", SpectatorType::Influencer, "Reddit Gaming");

    // Create Streamer spectators (capacity: 4 - adding 7 to trigger overflow)
    Spectator streamer1(301, "Streamer Felix", Gender::Male, "felix@twitch.tv", "0145678901", SpectatorType::Streamer, "Twitch");
    Spectator streamer2(302, "Streamer Grace", Gender::Female, "grace@youtube.com", "0145678902", SpectatorType::Streamer, "YouTube");
    Spectator streamer3(303, "Streamer Henry", Gender::Male, "henry@facebook.com", "0145678903", SpectatorType::Streamer, "Facebook Gaming");
    Spectator streamer4(304, "Streamer Ivy", Gender::Female, "ivy@mixer.com", "0145678904", SpectatorType::Streamer, "Mixer");
    // These should overflow Streamer section (3 overflow streamers)
    Spectator streamer5(305, "Streamer Jake", Gender::Male, "jake@kick.com", "0145678905", SpectatorType::Streamer, "Kick");
    Spectator streamer6(306, "Streamer Kate", Gender::Female, "kate@trovo.live", "0145678906", SpectatorType::Streamer, "Trovo");
    Spectator streamer7(307, "Streamer Leo", Gender::Male, "leo@streamlabs.com", "0145678907", SpectatorType::Streamer, "StreamLabs");

    // Create Normal spectators (capacity: 20 - adding 25 to trigger general overflow)
    Spectator normal1(401, "John Normal", Gender::Male, "john@email.com", "0156789012", SpectatorType::Normal, "General Public");
    Spectator normal2(402, "Jane Normal", Gender::Female, "jane@email.com", "0156789013", SpectatorType::Normal, "General Public");
    Spectator normal3(403, "Jack Normal", Gender::Male, "jack@email.com", "0156789014", SpectatorType::Normal, "General Public");
    Spectator normal4(404, "Lisa Normal", Gender::Female, "lisa@email.com", "0156789015", SpectatorType::Normal, "General Public");
    Spectator normal5(405, "Mike Normal", Gender::Male, "mike@email.com", "0156789016", SpectatorType::Normal, "General Public");
    Spectator normal6(406, "Nina Normal", Gender::Female, "nina@email.com", "0156789017", SpectatorType::Normal, "General Public");
    Spectator normal7(407, "Oliver Normal", Gender::Male, "oliver@email.com", "0156789018", SpectatorType::Normal, "General Public");
    Spectator normal8(408, "Paula Normal", Gender::Female, "paula@email.com", "0156789019", SpectatorType::Normal, "General Public");
    Spectator normal9(409, "Quinn Normal", Gender::Male, "quinn@email.com", "0156789020", SpectatorType::Normal, "General Public");
    Spectator normal10(410, "Rachel Normal", Gender::Female, "rachel@email.com", "0156789021", SpectatorType::Normal, "General Public");
    Spectator normal11(411, "Steve Normal", Gender::Male, "steve@email.com", "0156789022", SpectatorType::Normal, "General Public");
    Spectator normal12(412, "Tina Normal", Gender::Female, "tina@email.com", "0156789023", SpectatorType::Normal, "General Public");
    Spectator normal13(413, "Uma Normal", Gender::Female, "uma@email.com", "0156789024", SpectatorType::Normal, "General Public");
    Spectator normal14(414, "Victor Normal", Gender::Male, "victor@email.com", "0156789025", SpectatorType::Normal, "General Public");
    Spectator normal15(415, "Wendy Normal", Gender::Female, "wendy@email.com", "0156789026", SpectatorType::Normal, "General Public");
    Spectator normal16(416, "Xavier Normal", Gender::Male, "xavier@email.com", "0156789027", SpectatorType::Normal, "General Public");
    Spectator normal17(417, "Yara Normal", Gender::Female, "yara@email.com", "0156789028", SpectatorType::Normal, "General Public");
    Spectator normal18(418, "Zack Normal", Gender::Male, "zack@email.com", "0156789029", SpectatorType::Normal, "General Public");
    Spectator normal19(419, "Amy Normal", Gender::Female, "amy@email.com", "0156789030", SpectatorType::Normal, "General Public");
    Spectator normal20(420, "Ben Normal", Gender::Male, "ben@email.com", "0156789031", SpectatorType::Normal, "General Public");
    // These should overflow General section (5 overflow)
    Spectator normal21(421, "Cathy Normal", Gender::Female, "cathy@email.com", "0156789032", SpectatorType::Normal, "General Public");
    Spectator normal22(422, "David Normal", Gender::Male, "david@email.com", "0156789033", SpectatorType::Normal, "General Public");
    Spectator normal23(423, "Emma Normal", Gender::Female, "emma@email.com", "0156789034", SpectatorType::Normal, "General Public");
    Spectator normal24(424, "Frank Normal", Gender::Male, "frank@email.com", "0156789035", SpectatorType::Normal, "General Public");
    Spectator normal25(425, "Grace Normal", Gender::Female, "grace@email.com", "0156789036", SpectatorType::Normal, "General Public");

    std::cout << "\n📋 Adding spectators to entry queue (priority-based processing)...\n";

    // Add all spectators to entry queue (they'll be processed by priority)
    addToEntryQueue(vip1);
    addToEntryQueue(vip2);
    addToEntryQueue(vip3);
    addToEntryQueue(vip4);
    addToEntryQueue(vip5);
    addToEntryQueue(vip6);  // Overflow
    addToEntryQueue(vip7);  // Overflow
    addToEntryQueue(vip8);  // Overflow
    
    addToEntryQueue(player1);  // High priority, competes for VIP seats
    addToEntryQueue(player2);  // High priority, competes for VIP seats
    addToEntryQueue(player3);  // High priority, competes for VIP seats

    addToEntryQueue(influencer1);
    addToEntryQueue(influencer2);
    addToEntryQueue(influencer3);
    addToEntryQueue(influencer4);  // Overflow
    addToEntryQueue(influencer5);  // Overflow
    addToEntryQueue(influencer6);  // Overflow

    addToEntryQueue(streamer1);
    addToEntryQueue(streamer2);
    addToEntryQueue(streamer3);
    addToEntryQueue(streamer4);
    addToEntryQueue(streamer5);   // Overflow
    addToEntryQueue(streamer6);   // Overflow
    addToEntryQueue(streamer7);   // Overflow

    // Add normal spectators
    for (int i = 1; i <= 25; i++) {
        if (i == 1) addToEntryQueue(normal1);
        else if (i == 2) addToEntryQueue(normal2);
        else if (i == 3) addToEntryQueue(normal3);
        else if (i == 4) addToEntryQueue(normal4);
        else if (i == 5) addToEntryQueue(normal5);
        else if (i == 6) addToEntryQueue(normal6);
        else if (i == 7) addToEntryQueue(normal7);
        else if (i == 8) addToEntryQueue(normal8);
        else if (i == 9) addToEntryQueue(normal9);
        else if (i == 10) addToEntryQueue(normal10);
        else if (i == 11) addToEntryQueue(normal11);
        else if (i == 12) addToEntryQueue(normal12);
        else if (i == 13) addToEntryQueue(normal13);
        else if (i == 14) addToEntryQueue(normal14);
        else if (i == 15) addToEntryQueue(normal15);
        else if (i == 16) addToEntryQueue(normal16);
        else if (i == 17) addToEntryQueue(normal17);
        else if (i == 18) addToEntryQueue(normal18);
        else if (i == 19) addToEntryQueue(normal19);
        else if (i == 20) addToEntryQueue(normal20);
        else if (i == 21) addToEntryQueue(normal21);  // Overflow
        else if (i == 22) addToEntryQueue(normal22);  // Overflow
        else if (i == 23) addToEntryQueue(normal23);  // Overflow
        else if (i == 24) addToEntryQueue(normal24);  // Overflow
        else if (i == 25) addToEntryQueue(normal25);  // Overflow
    }

    // Process the entry queue to assign seats
    processEntryQueue();
    
    std::cout << "\n COMPREHENSIVE dummy data loaded with overflow scenarios!\n";
    std::cout << " Expected overflow: VIP(3), Influencer(3), Streamer(3), General(5)\n";
}

#endif
