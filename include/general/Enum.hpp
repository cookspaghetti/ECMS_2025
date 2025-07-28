#ifndef ENUM_HPP
#define ENUM_HPP

enum class MatchType
{
    BestOf1,
    BestOf3,
    BestOf5
};

enum class TournamentStage
{
    Qualifiers,
    GroupStage,
    KnockoutStage,
    Quarterfinals,
    Semifinals,
    Finals
};

enum class TournamentCategory
{
    Local,
    Regional,
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