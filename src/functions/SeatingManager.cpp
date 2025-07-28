// #include "functions/SeatingManager.hpp"

// //------------------------------------------------------------------------------
// // Constructor
// SeatingManager::SeatingManager(int /*vipCap*/, 
//                                int inflCap,
//                                int streamCapPerRoom,
//                                int genCap)
//   : influencerCapacity(inflCap),
//     influencerOccupied(0),
//     influencerSeating(inflCap),
//     streamerOccupied(0),
//     generalSeating(genCap),
//     generalCapacity(genCap),
//     entryQueue( VIP_ROWS*SEATS_PER_ROW + inflCap +
//                 MAX_STREAMING_ROOMS*streamCapPerRoom + genCap ),
//     overflowQueue( VIP_ROWS*SEATS_PER_ROW + inflCap +
//                    MAX_STREAMING_ROOMS*streamCapPerRoom + genCap )
// {
//   // initialize VIP grid
//   for(int r = 0; r < VIP_ROWS; ++r)
//     for(int s = 0; s < SEATS_PER_ROW; ++s)
//       vipSeats[r][s] = SeatPosition(r+1, s+1);

//   // initialize streaming rooms
//   for(int i = 0; i < MAX_STREAMING_ROOMS; ++i)
//     streamingRooms[i] = StreamingRoom(i+1, streamCapPerRoom);
// }

// //------------------------------------------------------------------------------
// // Queue up for seating
// void SeatingManager::addToEntryQueue(const Spectator &s, bool /*quiet*/) {
//     entryQueue.enqueue(s);
// }

// //------------------------------------------------------------------------------
// // Process everyone, seating or overflowing
// void SeatingManager::processEntryQueue(bool verbose) {
//     if (verbose) std::cout << "Processing entry queue...\n";
//     while (!entryQueue.isEmpty()) {
//         Spectator sp = entryQueue.dequeue();
//         bool seated = false;

//         switch (sp.type) {
//           case SpectatorType::VIP:
//             seated = assignVIPSeatPosition(sp, !verbose);
//             break;
//           case SpectatorType::Influencer:
//             seated = assignInfluencerSeat(sp, !verbose);
//             break;
//           case SpectatorType::Streamer:
//             seated = assignStreamingSeat(sp, !verbose);
//             break;
//           default:
//             seated = assignGeneralSeat(sp, !verbose);
//         }

//         if (!seated) {
//             overflowQueue.enqueue(sp);
//             if (verbose)
//               std::cout << "[Overflow] " << sp.name
//                         << " (" << toString(sp.type) << ")\n";
//         }
//     }
// }

// //------------------------------------------------------------------------------
// // Display all bins + overflow
// void SeatingManager::displaySeatingStatus() const {
//     std::cout << "\n=== SEATING STATUS ===\n";

//     // VIP
//     std::cout << "VIP Seating:\n";
//     for (int r = 0; r < VIP_ROWS; ++r) {
//       int count=0;
//       for (int s=0; s<SEATS_PER_ROW; ++s)
//         if (vipSeats[r][s].occupied) ++count;
//       std::cout << "  Row " << (r+1)
//                 << ": " << count
//                 << "/" << SEATS_PER_ROW << "\n";
//     }

//     // Influencers
//     std::cout << "Influencer Seating: "
//               << influencerOccupied
//               << "/" << influencerCapacity << "\n";

//     // Streaming
//     std::cout << "Streaming Rooms:\n";
//     for (int i = 0; i < MAX_STREAMING_ROOMS; ++i) {
//       int cnt = streamingRooms[i].viewers.size();
//       std::cout << "  [Room " << (i+1) << "] "
//                 << cnt << "/" << streamingRooms[i].capacity
//                 << " viewers\n";
//     }

//     // General
//     std::cout << "General Admission: "
//               << generalSeating.size()
//               << "/" << generalCapacity << "\n";

//     // Overflow
//     std::cout << "Overflow Queue: "
//               << overflowQueue.size()
//               << " waiting\n\n";
// }

// //------------------------------------------------------------------------------
// // Helpers
// SeatPosition SeatingManager::findNextVIPSeat() const {
//     for(int r = 0; r < VIP_ROWS; ++r)
//       for(int s = 0; s < SEATS_PER_ROW; ++s)
//         if (!vipSeats[r][s].occupied)
//           return SeatPosition(r+1, s+1);
//     return SeatPosition(); // no free seat
// }

// bool SeatingManager::assignVIPSeatPosition(const Spectator &s, bool quiet) {
//     SeatPosition p = findNextVIPSeat();
//     if (p.row != -1) {
//       auto &slot = vipSeats[p.row-1][p.seat-1];
//       slot.occupied = true;
//       slot.spectatorName = s.name;
//       if (!quiet) {
//         std::cout << s.name
//                   << " seated VIP at R" << p.row
//                   << " S" << p.seat << "\n";
//       }
//       return true;
//     }
//     return false;  // no VIP seat left → overflow
// }

// bool SeatingManager::assignInfluencerSeat(const Spectator &s, bool quiet) {
//     if (influencerOccupied < influencerCapacity) {
//       influencerSeating.enqueue(s);
//       ++influencerOccupied;
//       if (!quiet)
//         std::cout << s.name << " seated Influencer\n";
//       return true;
//     }
//     return false;  // overflow
// }

// bool SeatingManager::assignStreamingSeat(const Spectator &s, bool quiet) {
//     // Try rooms 1→2→…→MAX_STREAMING_ROOMS
//     for (int i = 0; i < MAX_STREAMING_ROOMS; ++i) {
//       if (!streamingRooms[i].viewers.isFull()) {
//         streamingRooms[i].viewers.enqueue(s);
//         if (!quiet)
//           std::cout << s.name
//                     << " seated in Stream Room " << (i+1)
//                     << " (" << streamingRooms[i].viewers.size()
//                     << "/" << streamingRooms[i].capacity << ")\n";
//         return true;
//       }
//     }
//     return false;  // overflow
// }

// bool SeatingManager::assignGeneralSeat(const Spectator &s, bool quiet) {
//     if (!generalSeating.isFull()) {
//       generalSeating.enqueue(s);
//       if (!quiet)
//         std::cout << s.name
//                   << " seated General ("
//                   << generalSeating.size()
//                   << "/" << generalCapacity << ")\n";
//       return true;
//     }
//     return false;  // overflow
// }
