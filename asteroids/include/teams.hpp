#ifndef TEAMS_HPP
#define TEAMS_HPP

#include <cstdint>
#include <entt/entt.hpp>

using entt::operator""_hs;

static const std::uint32_t team_player_tag = "TEAM_1"_hs;
static const std::uint32_t team_enemy_tag  = "TEAM_2"_hs;

#endif // TEAMS_HPP
