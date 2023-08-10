#pragma once
#include <cstdint>
namespace offsets
{
    inline uint64_t OFFSET_ENTITYLIST = 0x0;          // cl_entitylist
    inline uint64_t OFFSET_LOCAL_ENT = 0x0;           // LocalPlayer
    inline uint64_t OFFSET_NAME_LIST = 0x0;           // NameList
    inline uint64_t OFFSET_THIRDPERSON = 0x01e03040 + 0x6c; // thirdperson_override + 0x6c
    inline uint64_t OFFSET_TIMESCALE = 0x0;          // host_timescale
    inline uint64_t OFFSET_LEVELNAME = 0x0;           // LEVELNAME

    inline uint64_t OFFSET_TEAM = 0x0;         // m_iTeamNum
    inline uint64_t OFFSET_HEALTH = 0x0;       // m_iHealth
    inline uint64_t OFFSET_SHIELD = 0x0;       // m_shieldHealth
    inline uint64_t OFFSET_MAXSHIELD = 0x0;    // m_shieldHealthMax
    inline uint64_t OFFSET_NAME = 0x0;         // m_iName
    inline uint64_t OFFSET_SIGN_NAME = 0x0;    // m_iSignifierName
    inline uint64_t OFFSET_ABS_VELOCITY = 0x0; // m_vecAbsVelocity
    inline uint64_t OFFSET_VISIBLE_TIME = 0x0; // CPlayer!lastVisibleTime
    inline uint64_t OFFSET_ZOOMING = 0x0;      // m_bZooming

    inline uint64_t OFFSET_LIFE_STATE = 0x0;      // m_lifeState, >0 = dead
    inline uint64_t OFFSET_BLEED_OUT_STATE = 0x0; // m_bleedoutState, >0 = knocked

    inline uint64_t OFFSET_ORIGIN = 0x0;            // m_vecAbsOrigin
    inline uint64_t OFFSET_BONES = 0x0ec8 + 0x48;         // m_bConstrainBetweenEndpoints
    inline uint64_t OFFSET_AIMPUNCH = 0x0;          // m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
    inline uint64_t OFFSET_CAMERAPOS = 0x0;         // CPlayer!camera_origin
    inline uint64_t OFFSET_CAMERAANGLES = 0x0;      // CPlayer!camera_angles
    inline uint64_t OFFSET_VIEWANGLES = 0x0; // m_ammoPoolCapacity - 0x14
    inline uint64_t OFFSET_BREATH_ANGLES = OFFSET_VIEWANGLES - 0x10;
    inline uint64_t OFFSET_OBSERVER_MODE = 0x3534;    // m_iObserverMode
    inline uint64_t OFFSET_OBSERVING_TARGET = 0x3540; // m_hObserverTarget
    inline uint64_t OFFSET_ARMORTYPE = 0x0;        // m_armorType
    inline uint64_t OFFSET_MATRIX = 0x0;         // ViewMatrix
    inline uint64_t OFFSET_RENDER = 0x0;        // ViewRender

    inline uint64_t OFFSET_WEAPON = 0x0;            // m_latestPrimaryWeapons
    inline uint64_t OFFSET_BULLET_SPEED = 0x0;      // CWeaponX!m_flProjectileSpeed
    inline uint64_t OFFSET_BULLET_SCALE = 0x0;      // CWeaponX!m_flProjectileScale
    inline uint64_t OFFSET_ZOOM_FOV = 0x0 + 0x0; // m_playerData + m_curZoomFOV
    inline uint64_t OFFSET_AMMO = 0x0;              // m_ammoInClip
    inline uint64_t OFFSET_WEAPONID = 0x0;          // m_weaponNameIndex
    inline uint64_t OFFSET_CHARGE_LEVEL = 0x0;      // m_lastChargeLevel
    inline uint64_t OFFSET_ITEM_GLOW = 0x02f0 ;          // m_highlightFunctionBits
    inline uint64_t OFFSET_GLOW_FADE = 0x388;
    inline uint64_t OFFSET_GLOW_T1 = 0x292 ;            // 16256 = enabled, 0 = disabled
    inline uint64_t OFFSET_GLOW_T2 = 0x30c ;            // 1193322764 = enabled, 0 = disabled
    inline uint64_t OFFSET_GLOW_ENABLE = 0x3f8 ;        // 7 = enabled, 2 = disabled
    inline uint64_t OFFSET_GLOW_THROUGH_WALLS = 0x400 ; // 2 = enabled, 5 = disabled
    inline uint64_t GLOW_TYPE = 0x2C0;                 // 0x2C4 //OK Script_Highlight_GetState + 4 / m_highlightFunctionBits  + 4?
    inline uint64_t GLOW_COLOR_R = 0x200;              // OK Script_CopyHighlightState mov tcx nº7  / m_highlightParams + 24 (0x18)
    inline uint64_t GLOW_COLOR_G = 0x204;              // OK Script_CopyHighlightState mov tcx nº7  / m_highlightParams + 24 (0x18)
    inline uint64_t GLOW_COLOR_B = 0x208;              // OK Script_CopyHighlightState mov tcx nº7  / m_highlightParams + 24 (0x18)
    inline uint64_t GLOW_DISTANCE = 0x3B4;             // OK Script_Highlight_SetFarFadeDist / m_highlightServerFadeEndTimes + 52(0x34)
    inline uint64_t OFFSET_IN_ATTACK = 0x0;     // in_attack=0x07472e98
    inline uint64_t OFFSET_IN_FORWARD = 0x0759c0f8;
    inline uint64_t OFFSET_FLAGS = 0x0;            // m_fFlags
    inline uint64_t OFFSET_MODEL = 0x0030 ;            // TO BE RESEARCHED
    inline uint64_t OFFSET_STUDIOHDR = 0x0;        // CBaseAnimating!m_pStudioHdr
    inline uint64_t OFFSET_BONECLASS = 0x0ec8 + 0x48; ////m_nForceBone + 0x48
    inline uint64_t OFFSET_FLOCALTIME = 0x04BC;
    inline uint64_t OFFSET_CROSSHAIR_TARGET_START_TIME = 0x1B18;
    inline uint64_t OFFSET_CROSSHAIR_TARGET_END_TIME = 0x1B1C;
    inline uint64_t OFFSET_SKIN = 0x0e84;       /// m_nSkin
    inline uint64_t OFFSET_FYAW = 0x0; // m_currentFramePlayer.m_ammoPoolCount - 0x8
    inline uint64_t OFFSET_FPITCH = OFFSET_FYAW - 0x4;
    inline uint64_t OFFSET_FROLL = OFFSET_FYAW + 0x4;
    inline uint64_t OFFSET_IS_ZOOM = 0x0; // in_zoom=0x07473028
    inline uint64_t OFFSET_INPUT_SYSTEM = 0x0; // InputSystem
    inline float rcs = 30.0;
    inline bool aim = true;
    inline bool trigger = false;
    inline bool strigger = false;
    inline bool item_glow = false;
    inline bool player_glow = true;
    inline bool no_recoil = true;
    inline bool armorbaseglow = false;
    inline bool vischeck_glow = true;
    inline int bone = 2;
    inline float smooth = 150.0;
    inline float max_fov = 3.0;
};
