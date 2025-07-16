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
    Ahri,
    Akali,
    Alistar,
    Ashe,
    Azir,
    Braum,
    Cassiopeia,
    Darius,
    Draven,
    Ezreal,
    Fiora,
    Fizz,
    Garen,
    Graves,
    Janna,
    Jax,
    Jhin,
    Jinx,
    Karma,
    Karthus,
    Katarina,
    KhaZix,
    LeeSin,
    Leona,
    Lissandra,
    Lulu,
    Lux,
    Malphite,
    MissFortune,
    Morgana,
    Nami,
    Nautilus,
    Orianna,
    Rengar,
    Riven,
    Ryze,
    Sona,
    Soraka,
    Syndra,
    Taric,
    Teemo,
    Thresh,
    TwistedFate,
    Twitch,
    Vayne,
    VelKoz,
    Viktor,
    Xerath,
    Yasuo,
    Zed,
    Zyra,
    NoChampion // Placeholder for no champion selected
};

#endif