#include "Tournament.hpp"

Tournament::Tournament() : id(0), date(""), category("") {}

Tournament::Tournament(int id, const std::string& date, const std::string& category)
    : id(id), date(date), category(category) {}
