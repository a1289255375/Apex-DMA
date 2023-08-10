#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>
#include <iostream>
#include <cfloat>
#include "Game.h"
#include <thread>
#include <stdlib.h>
#include <algorithm>
extern float smooth;
extern int bone;
extern bool aim_no_recoil;
bool firing_range = false;
bool control_mode = false;
Memory apex_mem;
Memory client_mem;
struct Box
{
    Vector2D topLeft;
    Vector2D bottomRight;
};

const Vector2D crosshairPos = Vector2D(1920 / 2, 1080 / 2);
inline bool is_point_in_box(Box &box)
{
    return (crosshairPos.x >= box.topLeft.x && crosshairPos.x <= box.bottomRight.x && crosshairPos.y >= box.topLeft.y && crosshairPos.y <= box.bottomRight.y);
}
const int toRead = 100;
namespace config
{
    int wp_skin_id;
    int skin_id;
    float rcs = 40.f;
    bool active = true;
    uintptr_t aimentity = 0;
    uintptr_t tmp_aimentity = 0;
    uintptr_t lastaimentity = 0;
    float max = 999.0f;
    float max_dist = 200.0f * 40.0f;
    float map_dist = 2000.0f * 40.0f;
    int team_player = 0;
    float max_fov = 15;
    int aim = false;
    bool trigger = false;
    bool strigger = false;
    bool esp = false;
    bool radar = false;
    bool item_glow = false;
    bool player_glow = false;
    bool isAlive = false;
    bool no_recoil = false;
    bool isNotShooting = false;
    bool aiming = false;
    bool triggering = false;
    bool thirdperson = false;
    bool skinEnable = false;
    char map_name[32] = {0};
    bool armorbaseglow = false;
    bool vischeck_glow = false;
};

namespace ThreadsManager
{
    bool actions_t = false;
    bool esp_t = false;
    bool radar_t = false;
    bool spectators_t = false;
    bool spectatorlist_t = false;
    bool aim_t = false;
    bool trigger_t = false;
    bool strigger_t = false;
    bool vars_t = false;
    bool item_t = false;
    bool no_recoil_t = false;
};

uint64_t g_Base;
uint64_t c_Base;
bool next = false;
bool valid = false;
bool lock = false;

typedef struct player
{
    float dist = 0;
    float boxMiddle = 0;
    float h_y = 0;
    float width = 0;
    float height = 0;
    float b_x = 0;
    float b_y = 0;
    float HeadRadius = 0;
    float yaw = 0;
    int entity_team = 0;
    int health = 0;
    int shield = 0;
    int armorType = 0;
    int MaxShield = 0;
    Vector origin = Vector(0, 0, 0);
    HitBoxManager HitBox;
    bool knocked = false;
    bool visible = false;
    bool isAlive = false;
    char name[33] = {0};
} player;
player players[toRead];
float lastvis_esp[toRead];
float lastvis_aim[toRead];

namespace spectatorlist
{
    bool spectator_list = false;
    int tmp_spec = 0, spectators = 0;
    int tmp_all_spec = 0, allied_spectators = 0;
    int totalEntityCount = 0;
    int totalSquadCount = 0;
    // make std vector call teamsquad
    std::vector<int> teamsquad;
    std::vector<int> teamsquad_tmp;
    int teamsquad_size = 0;
};