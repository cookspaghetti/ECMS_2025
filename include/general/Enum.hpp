#ifndef ENUM_HPP
#define ENUM_HPP

enum class TournamentStage
{
    Registration,    // Added for tournament creation
    Qualifiers,
    Tiebreakers,
    Quarterfinals,
    Semifinals,
    Finals,
    Completed        // Added for completed tournaments
};

enum class TournamentCategory
{
    Local,
    Regional,
    National,        // Added for national tournaments
    International
};

enum class Gender {
    Male,
    Female
};

enum class SpectatorType
{
    Normal,
    VIP,
    Streamer,
    Influencer,
    Player
};

enum class Champion
{
    Akali,
    Ezreal,
    Fiora,
    Fizz,
    Garen,
    Jax,
    Katarina,
    LeeSin,
    Riven,
    Yasuo,
    Zed,
    NoChampion // Placeholder for no champion selected
};

#endif