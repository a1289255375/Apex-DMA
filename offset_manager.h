#pragma once
#include <SimpleIni.h>
#include "offsets.h"
using namespace offsets;
namespace offset_manager
{
    inline bool LoadOffsets();
    inline bool DateCheck(std::string date);
    inline bool LoadVars();
};

bool offset_manager::LoadOffsets()
{
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile("stdout.ini");

    // Get the offsets from the ini file
    // ...
    bool success = true;

    offsets::OFFSET_ENTITYLIST = ini.GetLongValue("Miscellaneous", "cl_entitylist", 0);
    printf("cl_entitylist: %#08x \n", offsets::OFFSET_ENTITYLIST);
    if (offsets::OFFSET_ENTITYLIST == 0)
    {
        printf("Failed to load cl_entitylist offset\n");
        success = false;
    }
    offsets::OFFSET_LOCAL_ENT = ini.GetLongValue("Miscellaneous", "LocalPlayer", 0);
    printf("LocalPlayer: %#08x \n", offsets::OFFSET_LOCAL_ENT);
    if (offsets::OFFSET_LOCAL_ENT == 0)
    {
        printf("Failed to load LocalPlayer offset\n");
        success = false;
    }
    offsets::OFFSET_NAME_LIST = ini.GetLongValue("Miscellaneous", "NameList", 0);
    printf("NameList: %#08x \n", offsets::OFFSET_NAME_LIST);
    if (offsets::OFFSET_NAME_LIST == 0)
    {
        printf("Failed to load NameList offset\n");
        success = false;
    }
    offsets::OFFSET_TIMESCALE = ini.GetLongValue("ConVars", "host_timescale", 0);
    printf("host_timescale: %#08x \n", offsets::OFFSET_TIMESCALE);
    if (offsets::OFFSET_TIMESCALE == 0)
    {
        printf("Failed to load host_timescale offset\n");
        success = false;
    }
    offsets::OFFSET_LEVELNAME = ini.GetLongValue("Miscellaneous", "LEVELNAME", 0);
    printf("LEVELNAME: %#08x \n", offsets::OFFSET_LEVELNAME);
    if (offsets::OFFSET_LEVELNAME == 0)
    {
        printf("Failed to load LEVELNAME offset\n");
        success = false;
    }
    offsets::OFFSET_TEAM = ini.GetLongValue("RecvTable.DT_BaseEntity", "m_iTeamNum", 0);
    printf("m_iTeamNum: %#08x \n", offsets::OFFSET_TEAM);
    if (offsets::OFFSET_TEAM == 0)
    {
        printf("Failed to load m_iTeamNum offset\n");
        success = false;
    }
    offsets::OFFSET_HEALTH = ini.GetLongValue("RecvTable.DT_Player", "m_iHealth", 0);
    printf("m_iHealth: %#08x \n", offsets::OFFSET_HEALTH);
    if (offsets::OFFSET_HEALTH == 0)
    {
        printf("Failed to load m_iHealth offset\n");
        success = false;
    }
    offsets::OFFSET_SHIELD = ini.GetLongValue("RecvTable.DT_BaseEntity", "m_shieldHealth", 0);
    printf("m_shieldHealth: %#08x \n", offsets::OFFSET_SHIELD);
    if (offsets::OFFSET_SHIELD == 0)
    {
        printf("Failed to load m_shieldHealth offset\n");
        success = false;
    }
    offsets::OFFSET_MAXSHIELD = ini.GetLongValue("RecvTable.DT_BaseEntity", "m_shieldHealthMax", 0);
    printf("m_shieldHealthMax: %#08x \n", offsets::OFFSET_MAXSHIELD);
    if (offsets::OFFSET_MAXSHIELD == 0)
    {
        printf("Failed to load m_shieldHealthMax offset\n");
        success = false;
    }
    offsets::OFFSET_NAME = ini.GetLongValue("RecvTable.DT_BaseEntity", "m_iName", 0);
    printf("m_iName: %#08x \n", offsets::OFFSET_NAME);
    if (offsets::OFFSET_NAME == 0)
    {
        printf("Failed to load m_iName offset\n");
        success = false;
    }
    offsets::OFFSET_SIGN_NAME = ini.GetLongValue("RecvTable.DT_BaseEntity", "m_iSignifierName", 0);
    printf("m_iSignifierName: %#08x \n", offsets::OFFSET_SIGN_NAME);
    if (offsets::OFFSET_SIGN_NAME == 0)
    {
        printf("Failed to load m_iSignifierName offset\n");
        success = false;
    }
    offsets::OFFSET_ABS_VELOCITY = ini.GetLongValue("RecvTable.DT_LocalPlayerExclusive", "m_vecAbsVelocity", 0);
    printf("m_vecAbsVelocity: %#08x \n", offsets::OFFSET_ABS_VELOCITY);
    if (offsets::OFFSET_ABS_VELOCITY == 0)
    {
        printf("Failed to load m_vecAbsVelocity offset\n");
        success = false;
    }
    offsets::OFFSET_VISIBLE_TIME = ini.GetLongValue("Miscellaneous", "CPlayer!lastVisibleTime", 0);
    printf("CPlayer!lastVisibleTime: %#08x \n", offsets::OFFSET_VISIBLE_TIME);
    if (offsets::OFFSET_VISIBLE_TIME == 0)
    {
        printf("Failed to load CPlayer!lastVisibleTime offset\n");
        success = false;
    }
    offsets::OFFSET_ZOOMING = ini.GetLongValue("RecvTable.DT_Player", "m_bZooming", 0);
    printf("m_bZooming: %#08x \n", offsets::OFFSET_ZOOMING);
    if (offsets::OFFSET_ZOOMING == 0)
    {
        printf("Failed to load m_bZooming offset\n");
        success = false;
    }
    offsets::OFFSET_LIFE_STATE = ini.GetLongValue("RecvTable.DT_Player", "m_lifeState", 0);
    printf("m_lifeState: %#08x \n", offsets::OFFSET_LIFE_STATE);
    if (offsets::OFFSET_LIFE_STATE == 0)
    {
        printf("Failed to load m_lifeState offset\n");
        success = false;
    }
    offsets::OFFSET_BLEED_OUT_STATE = ini.GetLongValue("RecvTable.DT_Player", "m_bleedoutState", 0);
    printf("m_bleedoutState: %#08x \n", offsets::OFFSET_BLEED_OUT_STATE);
    if (offsets::OFFSET_BLEED_OUT_STATE == 0)
    {
        printf("Failed to load m_bleedoutState offset\n");
        success = false;
    }
    offsets::OFFSET_ORIGIN = ini.GetLongValue("DataMap.C_BaseEntity", "m_vecAbsOrigin", 0);
    printf("m_vecAbsOrigin: %#08x \n", offsets::OFFSET_ORIGIN);
    if (offsets::OFFSET_ORIGIN == 0)
    {
        printf("Failed to load m_vecAbsOrigin offset\n");
        success = false;
    }
    offsets::OFFSET_AIMPUNCH = ini.GetLongValue("DataMap.C_Player", "m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle", 0);
    printf("m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle: %#08x \n", offsets::OFFSET_AIMPUNCH);
    if (offsets::OFFSET_AIMPUNCH == 0)
    {
        printf("Failed to load m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle offset\n");
        success = false;
    }
    offsets::OFFSET_CAMERAPOS = ini.GetLongValue("Miscellaneous", "CPlayer!camera_origin", 0);
    printf("CPlayer!camera_origin: %#08x \n", offsets::OFFSET_CAMERAPOS);
    if (offsets::OFFSET_CAMERAPOS == 0)
    {
        printf("Failed to load CPlayer!camera_origin offset\n");
        success = false;
    }
    offsets::OFFSET_CAMERAANGLES = ini.GetLongValue("Miscellaneous", "CPlayer!camera_angles", 0);
    printf("CPlayer!camera_angles: %#08x \n", offsets::OFFSET_CAMERAANGLES);
    if (offsets::OFFSET_CAMERAANGLES == 0)
    {
        printf("Failed to load CPlayer!camera_angles offset\n");
        success = false;
    }
    offsets::OFFSET_VIEWANGLES = ini.GetLongValue("RecvTable.DT_Player", "m_ammoPoolCapacity", 0)-0x14;
    printf("m_ammoPoolCapacity-0x14: %#08x  \n", offsets::OFFSET_VIEWANGLES);
    if (offsets::OFFSET_VIEWANGLES == -0x14)
    {
        printf("Failed to load m_ammoPoolCapacity offset\n");
        success = false;
    }
    offsets::OFFSET_ARMORTYPE = ini.GetLongValue("RecvTable.DT_Player", "m_armorType", 0);
    printf("m_armorType: %#08x \n", offsets::OFFSET_ARMORTYPE);
    if (offsets::OFFSET_ARMORTYPE == 0)
    {
        printf("Failed to load m_armorType offset\n");
        success = false;
    }
    offsets::OFFSET_MATRIX = ini.GetLongValue("Miscellaneous", "ViewMatrix", 0);
    printf("ViewMatrix: %#08x \n", offsets::OFFSET_MATRIX);
    if (offsets::OFFSET_MATRIX == 0)
    {
        printf("Failed to load ViewMatrix offset\n");
        success = false;
    }
    offsets::OFFSET_RENDER = ini.GetLongValue("Miscellaneous", "ViewRender", 0);
    printf("ViewRender: %#08x \n", offsets::OFFSET_RENDER);
    if (offsets::OFFSET_RENDER == 0)
    {
        printf("Failed to load ViewRender offset\n");
        success = false;
    }
    offsets::OFFSET_WEAPON = ini.GetLongValue("RecvTable.DT_BaseCombatCharacter", "m_latestPrimaryWeapons", 0);
    printf("m_latestPrimaryWeapons: %#08x \n", offsets::OFFSET_WEAPON);
    if (offsets::OFFSET_WEAPON == 0)
    {
        printf("Failed to load m_latestPrimaryWeapons offset\n");
        success = false;
    }
    offsets::OFFSET_BULLET_SPEED = ini.GetLongValue("Miscellaneous", "CWeaponX!m_flProjectileSpeed", 0);
    printf("CWeaponX!m_flProjectileSpeed: %#08x \n", offsets::OFFSET_BULLET_SPEED);
    if (offsets::OFFSET_BULLET_SPEED == 0)
    {
        printf("Failed to load CWeaponX!m_flProjectileSpeed offset\n");
        success = false;
    }
    offsets::OFFSET_BULLET_SCALE = ini.GetLongValue("Miscellaneous", "CWeaponX!m_flProjectileScale", 0);
    printf("CWeaponX!m_flProjectileScale: %#08x \n", offsets::OFFSET_BULLET_SCALE);
    if (offsets::OFFSET_BULLET_SCALE == 0)
    {
        printf("Failed to load CWeaponX!m_flProjectileScale offset\n");
        success = false;
    }
    offsets::OFFSET_ZOOM_FOV = ini.GetLongValue("RecvTable.DT_WeaponX", "m_playerData", 0) + ini.GetLongValue("DataMap.WeaponPlayerData", "m_curZoomFOV", 0); 
    printf("m_playerData + m_curZoomFOV: %#08x \n", offsets::OFFSET_ZOOM_FOV);
    if (offsets::OFFSET_ZOOM_FOV == 0)
    {
        printf("Failed to load m_curZoomFOV offset\n");
        success = false;
    }
    offsets::OFFSET_AMMO = ini.GetLongValue("RecvTable.DT_WeaponX_LocalWeaponData", "m_ammoInClip", 0);
    printf("m_ammoInClip: %#08x \n", offsets::OFFSET_AMMO);
    if (offsets::OFFSET_AMMO == 0)
    {
        printf("Failed to load m_ammoInClip offset\n");
        success = false;
    }
    offsets::OFFSET_WEAPONID = ini.GetLongValue("RecvTable.DT_PropSurvival", "m_weaponNameIndex", 0);
    printf("m_weaponNameIndex: %#08x \n", offsets::OFFSET_WEAPONID);
    if (offsets::OFFSET_WEAPONID == 0)
    {
        printf("Failed to load m_weaponNameIndex offset\n");
        success = false;
    }
    offsets::OFFSET_CHARGE_LEVEL = ini.GetLongValue("DataMap.CWeaponX", "m_lastChargeLevel", 0);
    printf("m_lastChargeLevel: %#08x \n", offsets::OFFSET_CHARGE_LEVEL);
    if (offsets::OFFSET_CHARGE_LEVEL == 0)
    {
        printf("Failed to load m_lastChargeLevel offset\n");
        success = false;
    }
    offsets::OFFSET_IN_ATTACK = ini.GetLongValue("Buttons", "in_attack", 0);
    printf("in_attack: %#08x \n", offsets::OFFSET_IN_ATTACK);
    if (offsets::OFFSET_IN_ATTACK == 0)
    {
        printf("Failed to load in_attack offset\n");
        success = false;
    }
    offsets::OFFSET_FLAGS = ini.GetLongValue("RecvTable.DT_Player", "m_fFlags", 0);
    printf("m_fFlags: %#08x \n", offsets::OFFSET_FLAGS);
    if (offsets::OFFSET_FLAGS == 0)
    {
        printf("Failed to load m_fFlags offset\n");
        success = false;
    }
    offsets::OFFSET_STUDIOHDR = ini.GetLongValue("Miscellaneous", "CBaseAnimating!m_pStudioHdr", 0);
    printf("CBaseAnimating!m_pStudioHdr: %#08x \n", offsets::OFFSET_STUDIOHDR);
    if (offsets::OFFSET_STUDIOHDR == 0)
    {
        printf("Failed to load CBaseAnimating!m_pStudioHdr offset\n");
        success = false;
    }
    offsets::OFFSET_FYAW = ini.GetLongValue("DataMap.C_Player", "m_currentFramePlayer.m_ammoPoolCount", 0)- 0x8;
    printf("m_currentFramePlayer.m_ammoPoolCount - 0x8: %#08x \n", offsets::OFFSET_FYAW);
    if (offsets::OFFSET_FYAW == -0x8)
    {
        printf("Failed to load m_currentFramePlayer.m_ammoPoolCount offset\n");
        success = false;
    }
    offsets::OFFSET_INPUT_SYSTEM = ini.GetLongValue("Miscellaneous", "InputSystem", 0);
    printf("InputSystem: %#08x \n", offsets::OFFSET_INPUT_SYSTEM);
    if (offsets::OFFSET_INPUT_SYSTEM == 0)
    {
        printf("Failed to load InputSystem offset\n");
        success = false;
    }
    // offsets::OFFSET_CLIENT_NAME = ini.GetStringValue("Miscellaneous", "ClientName", "");
    // printf("ClientName: %s \n", offsets::OFFSET_CLIENT_NAME);
    // if (offsets::OFFSET_CLIENT_NAME == "")
    // {
    //     printf("Failed to load ClientName offset\n");
    //     success = false;
    // }
    OFFSET_FPITCH = OFFSET_FYAW - 0x4;
    OFFSET_FROLL = OFFSET_FYAW + 0x4;
    OFFSET_BREATH_ANGLES = OFFSET_VIEWANGLES - 0x10;
    return true;
}
bool offset_manager::DateCheck(std::string date)
{
    //get the current date
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int year = 1900 + ltm->tm_year;
    int month = 1 + ltm->tm_mon;
    int day = ltm->tm_mday;
    //compare the current date to the date in the argument if the current date is pass the date in the argument return true
    if(year > std::stoi(date.substr(0, 4)))
    {
        return true;
    }
    else if(year == std::stoi(date.substr(0, 4)))
    {
        if(month > std::stoi(date.substr(5, 2)))
        {
            return true;
        }
        else if(month == std::stoi(date.substr(5, 2)))
        {
            if(day >= std::stoi(date.substr(8, 2)))
            {
                return true;
            }
        }
    }
    // Add a return statement to cover all possible paths
    return false;
}
bool offset_manager::LoadVars()
{
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile("config.ini");
    bool success = true;
    offsets::rcs = ini.GetDoubleValue("Settings", "rcs", 0.0);
    offsets::aim = ini.GetBoolValue("Settings", "aim", false);
    offsets::strigger = ini.GetBoolValue("Settings", "strigger", false);
    offsets::item_glow = ini.GetBoolValue("Settings", "item_glow", false);
    offsets::player_glow = ini.GetBoolValue("Settings", "player_glow", true);
    offsets::no_recoil = ini.GetBoolValue("Settings", "no_recoil", true);
    offsets::armorbaseglow = ini.GetBoolValue("Settings", "armorbaseglow", false);
    offsets::vischeck_glow = ini.GetBoolValue("Settings", "vischeck_glow", true);
    offsets::bone = ini.GetLongValue("Settings", "bone", 2);
    offsets::smooth = ini.GetDoubleValue("Settings", "smooth", 150.0);
    offsets::max_fov = ini.GetDoubleValue("Settings", "max_fov", 3.0);
    printf ("rcs: %f \n", offsets::rcs);
    printf ("aim: %d \n", offsets::aim);
    printf ("trigger: %d \n", offsets::strigger);
    printf ("item_glow: %d \n", offsets::item_glow);
    printf ("player_glow: %d \n", offsets::player_glow);
    printf ("no_recoil: %d \n", offsets::no_recoil);
    printf ("armorbaseglow: %d \n", offsets::armorbaseglow);
    printf ("vischeck_glow: %d \n", offsets::vischeck_glow);
    printf ("bone: %d \n", offsets::bone);
    printf ("smooth: %f \n", offsets::smooth);
    printf ("max_fov: %f \n", offsets::max_fov);
    return success;
}