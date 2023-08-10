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
#include <termios.h>
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
int wp_skin_id;
int skin_id;

bool firing_range = false;
bool control_mode = false;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float max = 999.0f;
float max_dist = 200.0f * 40.0f;
float map_dist = 2000.0f * 40.0f;
int team_player = 0;

const int toRead = 100;
extern bool aim_no_recoil;
extern float smooth;
extern int bone;
extern bool bestbone_bool;
bool iszoom = false;
// make std vector call teamsquad
std::vector<int> teamsquad;
std::vector<int> teamsquad_tmp;
int teamsquad_size = 0;
bool active = false;
uint64_t g_Base;
uint64_t c_Base;
bool next = false;
bool valid = false;
bool lock = false;
int totalEntityCount = 0;
int totalSquadCount = 0;
bool isShooting = false;
char map_name[32] = {0};
uint32_t keys[4] = {0};
char last_map_name[32] = {0};
float lastvis_esp[toRead];
float lastvis_aim[toRead];
int tmp_spec = 0, spectators = 0;
int tmp_all_spec = 0, allied_spectators = 0;
void algsMapRadar()
{
	uintptr_t pLocal = apex_mem.Read<uintptr_t>(g_Base + OFFSET_LOCAL_ENT); // pointer to the local player
	int dt = apex_mem.Read<int>(pLocal + OFFSET_TEAM);						// original team id of the local player
	for (int i; i < 60000; i++)
	{
		apex_mem.Write<int>(pLocal + OFFSET_TEAM, 1);
	}
	for (int j; j < 60000; j++)
	{
		apex_mem.Write<int>(pLocal + OFFSET_TEAM, dt);
	}
}
inline bool bittest(uint32_t data, unsigned char index)
{
	return (data & (1 << index)) != 0;
}
bool getkeyState(int key_state)
{
	int v2 = apex_mem.Read<int>(g_Base + OFFSET_INPUT_SYSTEM + 4 * (key_state >> 5) + 0xB0);
	return bittest(v2, key_state & 0x1F);
}

void ProcessPlayer(Entity &LPlayer, Entity &target, uint64_t entitylist, int index)
{
	int entity_team = target.getTeamId();
	int lplayer_team = LPlayer.getTeamId();
	Vector EntityPosition = target.getPosition();
	Vector LocalPlayerPosition = LPlayer.getPosition();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);
	QAngle LPlayerAngle = QAngle(LPlayer.getFPitch(), LPlayer.getFYaw(), 0);
	QAngle TargetAngle = QAngle(target.getFPitch(), target.getFYaw(), 0);
	Math::NormalizeAngles(LPlayerAngle);
	Math::NormalizeAngles(TargetAngle);
	if (dist > max_dist)
		return;
	if (!firing_range)
		if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
			return;

	if (aim)
	{
		if ((target.lastVisTime() > lastvis_aim[index]))
		{
			float fov = CalculateFov(LPlayer, target);
			if (fov < max)
			{
				max = fov;
				tmp_aimentity = target.ptr;
			}
		}
		else
		{
			if (aimentity == target.ptr)
			{
				aimentity = tmp_aimentity = lastaimentity = 0;
			}
		}
	}
	else
	{
		float fov = CalculateFov(LPlayer, target);
		if (fov < max)
		{
			max = fov;
			tmp_aimentity = target.ptr;
		}
	}
	lastvis_aim[index] = target.lastVisTime();
}
namespace globals
{
	uint64_t LocalPlayer = 0;
}
using namespace globals;
void DoActions()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// uint32_t counter = 0;
		while (g_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));

			
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
			if (LocalPlayer == 0)
				continue;

			Entity LPlayer = getEntity(LocalPlayer);

			team_player = LPlayer.getTeamId();
			if (team_player < 0 || team_player > 50)
			{
				continue;
			}
			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

			uint64_t baseent = 0;
			apex_mem.Read<uint64_t>(entitylist, baseent);
			if (baseent == 0)
			{
				continue;
			}
			max = 999.0f;
			tmp_aimentity = 0;
			iszoom = LPlayer.isZooming();
			isShooting = (apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK + 0x8) == 5);
			apex_mem.ReadArray<char>(g_Base + OFFSET_LEVELNAME, map_name, 32);
			if (getkeyState((int)ButtonCode::KEY_HOME))
			{
				printf("pressed \n");
				algsMapRadar();
			}
			for (int i = 0; i < toRead; i++)
			{
				uint64_t centity = 0;
				apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
				if (centity == 0)
					continue;
				if (LocalPlayer == centity)
				{
					continue;
				}
				Entity Target = getEntity(centity);
				if (!Target.isPlayer())
				{
					continue;
				}
				ProcessPlayer(LPlayer, Target, entitylist, i);
				if (map_name == "mp_lobby")
					continue;
				int entity_team = Target.getTeamId();
				int health = Target.getHealth();
				int shield = Target.getShield();
				if (entity_team == team_player)
				{

					continue;
				}

				if (player_glow && !Target.isGlowing())
				{

					float currentEntityTime = 60000.f;	// ADDED currentEntityTime
					GlowMode mode = {101, 102, 50, 75}; // { 101,102,50,75 };
					GColor color;

					if ((int)Target.buffer[OFFSET_GLOW_ENABLE] != 1 || (int)Target.buffer[OFFSET_GLOW_THROUGH_WALLS] != 1 || (int)Target.buffer[OFFSET_GLOW_FADE] != 872415232)
					{
						// float currentEntityTime = 60000.f;//(fhloaht)targhset->buhsffer[0xhhsEEhs4];
						if (!isnan(currentEntityTime) && currentEntityTime > 0.f)
						{

							if (armorbaseglow)
							{
								if (Target.isBleedOut() || !Target.isAlive())
								{
									color = {5, 5, 0}; // Downed enemy - Yellow (15/3 = 5)
								}
								else
								{
									if (shield > 100)
									{					   // Heirloom armor - Red
										color = {5, 0, 0}; // (15/3 = 5)
									}
									else if (shield > 75)
									{					   // Purple armor - Purple
										color = {1, 0, 3}; // (5/3 = 1.666 => 1)
									}
									else if (shield > 50)
									{					   // Blue armor - Light blue
										color = {0, 1, 3}; // (5/3 = 1.666 => 1)
									}
									else if (shield > 0)
									{					   // White armor - White
										color = {5, 5, 5}; // (15/3 = 5)
									}
									else if (health < 50)
									{					   // Above 50% HP - Orange
										color = {5, 3, 0}; // (15/3 = 5)
									}
									else
									{					   // Below 50% HP - Green
										color = {0, 5, 0}; // (15/3 = 5)
									}
								}
							}
							if (vischeck_glow)
							{
								if (Target.lastVisTime() > lastvis_esp[i])
								{
									color = {0, 5, 0}; // Green (15/3 = 5)
								}
								else
								{
									color = {5, 0, 15}; // Red (15/3 = 5)
								}
								lastvis_esp[i] = Target.lastVisTime();
							}
						}
					}

					Target.enableGlow();
					apex_mem.Write<GlowMode>(Target.ptr + GLOW_TYPE, mode); // GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel
					apex_mem.Write<GColor>(Target.ptr + GLOW_COLOR_R, color);
					// printf("isGlowing");
				}
				else if (!player_glow && Target.isGlowing())
				{
					Target.disableGlow();
				}
			}
			if (!lock)
				aimentity = tmp_aimentity;
			else
				aimentity = lastaimentity;
		}
		while (next && g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
static void sTriggerbotThread()
{

	while (g_Base != 0)
	{
		if (strigger)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (!getkeyState((int)ButtonCode::MOUSE_4))
			{
				continue;
			}
			if (LocalPlayer == 0)
				continue;
			Entity LPlayer = getEntity(LocalPlayer);
			if (aimentity == 0)
				continue;
			Entity target = getEntity(aimentity);
			Vector LPlayerpos = LPlayer.getPosition();

			uint64_t viewrender = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewrender);
			if (viewrender == 0)
				continue;
			uint64_t viewmatrix = apex_mem.Read<uint64_t>(viewrender + OFFSET_MATRIX);
			if (viewmatrix == 0)
				continue;
			Matrix m = {};
			apex_mem.Read<Matrix>(viewmatrix, m);
			// get skeletonpositions
			Vector Head = target.getstudiohdr(0);
			Vector Neck = target.getstudiohdr(1);
			Vector Chest = target.getstudiohdr(2);
			Vector Stomach = target.getstudiohdr(3);
			Vector Bottom = target.getstudiohdr(4);
			Vector HeadHigh = Head + Vector(0, 0, 5);
			Vector NeckHigh = Neck + Vector(0, 0, 5);
			Vector ChestHigh = Chest + Vector(0, 0, 8.5);
			Vector StomachHigh = Stomach + Vector(0, 0, 8.5);
			Vector BottomHigh = Bottom + Vector(0, 0, 8.5);
			Vector Head2D, HeadHigh2D;
			Vector Neck2D, NeckHigh2D;
			Vector Chest2D, ChestHigh2D;
			Vector Stomach2D, StomachHigh2D;
			Vector Bottom2D, BottomHigh2D;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (WorldToScreen(Head, m.matrix, Head2D) && WorldToScreen(HeadHigh, m.matrix, HeadHigh2D) &&
				WorldToScreen(Neck, m.matrix, Neck2D) && WorldToScreen(NeckHigh, m.matrix, NeckHigh2D) &&
				WorldToScreen(Chest, m.matrix, Chest2D) && WorldToScreen(ChestHigh, m.matrix, ChestHigh2D) &&
				WorldToScreen(Stomach, m.matrix, Stomach2D) && WorldToScreen(StomachHigh, m.matrix, StomachHigh2D) &&
				WorldToScreen(Bottom, m.matrix, Bottom2D) && WorldToScreen(BottomHigh, m.matrix, BottomHigh2D))

			{
				float HeadRadius = Head2D.y - HeadHigh2D.y;
				float NeckRadius = Neck2D.y - NeckHigh2D.y;
				float ChestRadius = Chest2D.y - ChestHigh2D.y;
				float StomachRadius = Stomach2D.y - StomachHigh2D.y;
				float BottomRadius = Bottom2D.y - BottomHigh2D.y;
				/*3440x1440*/
				// float sX = 3440 / 2;
				// float sY = 1440 / 2;
				/*1920x1080*/
				float sX = 1920 / 2;
				float sY = 1080 / 2;
				/*2560x1440*/
				// float sX = 2560 / 2;
				// float sY = 1440 / 2;

				if (((sX - Head2D.x) * (sX - Head2D.x) + (sY - Head2D.y) * (sY - Head2D.y) <= (HeadRadius * HeadRadius)) ||
					((sX - Chest2D.x) * (sX - Chest2D.x) + (sY - Chest2D.y) * (sY - Chest2D.y) <= (ChestRadius * ChestRadius)) ||
					((sX - Stomach2D.x) * (sX - Stomach2D.x) + (sY - Stomach2D.y) * (sY - Stomach2D.y) <= (StomachRadius * StomachRadius)) ||
					((sX - Bottom2D.x) * (sX - Bottom2D.x) + (sY - Bottom2D.y) * (sY - Bottom2D.y) <= (BottomRadius * BottomRadius)))
				{
					if (apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK + 0x8) != 5)
					{
						apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
						std::this_thread::sleep_for(std::chrono::milliseconds(6));
						apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
					}
				}
			}
		}
	}
}
// /////////////////////////////////////////////////////////////////////////////////////////////////////
static void AimbotLoop()
{

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	while (g_Base != 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (aim)
		{
			if (aimentity == 0 || !isShooting)
			{
				lock = false;
				lastaimentity = 0;
				continue;
			}
			lock = true;
			lastaimentity = aimentity;
			if (LocalPlayer == 0)
				continue;
			Entity LPlayer = getEntity(LocalPlayer);
			QAngle Angles = CalculateBestBoneAim(LPlayer, aimentity, max_fov);
			if (Angles.x == 0 && Angles.y == 0)
			{
				lock = false;
				lastaimentity = 0;
				continue;
			}
			LPlayer.SetViewAngles(Angles);
		}
	}
}
static void NoRecoilThread()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	while (g_Base != 0)
	{
		if (no_recoil)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (LocalPlayer == 0)
				continue;
			QAngle oldRecoilAngle;
			Entity LPlayer = getEntity(LocalPlayer);
			QAngle newAngle;
			// get recoil angle
			QAngle recoilAngles = LPlayer.GetRecoil();
			// get original angles
			QAngle oldVAngles = LPlayer.GetViewAngles();
			newAngle = oldVAngles;
			// removing recoil angles from player view angles
			newAngle.x = newAngle.x + (oldRecoilAngle.x - recoilAngles.x) * (rcs / 100.f);
			newAngle.y = newAngle.y + (oldRecoilAngle.y - recoilAngles.y) * (rcs / 100.f);
			// setting viewangles to new angles
			LPlayer.SetViewAngles(newAngle);
			// setting old recoil angles to current recoil angles
			oldRecoilAngle = recoilAngles;
			// normalize view angles
			Math::NormalizeAngles(oldRecoilAngle);
		}
	}
}
static void item_glow_t()
{
	while (item_glow)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		int k = 0;
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			uint64_t entitylist = g_Base + offsets::OFFSET_ENTITYLIST;
			if (item_glow)
			{
				for (int i = 0; i < 15000; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					Item item = getItem(centity);

					if (item.isBox())
					{
						apex_mem.Write<int>(centity + 0x262, 16256);
						apex_mem.Write<int>(centity + 0x2dc, 1193322764);
						apex_mem.Write<int>(centity + 0x3c8, 7);
						apex_mem.Write<int>(centity + 0x3d0, 2);
					}

					if (item.isTrap())
					{
						apex_mem.Write<int>(centity + 0x262, 16256);
						apex_mem.Write<int>(centity + 0x2dc, 1193322764);
						apex_mem.Write<int>(centity + 0x3c8, 7);
						apex_mem.Write<int>(centity + 0x3d0, 2);
					}

					if (item.isItem() && !item.isGlowing())
					{
						item.enableGlow();
					}
				}
				k = 1;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else
			{
				if (k == 1)
				{
					for (int i = 0; i < 15000; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
							continue;

						Item item = getItem(centity);

						if (item.isItem() && item.isGlowing())
						{
							item.disableGlow();
						}
					}
					k = 0;
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (geteuid() != 0)
	{
		printf("Error: %s is not running as root\n", argv[0]);
		return 0;
	}
	const char *ap_proc = "r5apex.exe";
	// check offset is loaded
	std::string date = "2024-06-10";
	if (offset_manager::DateCheck(date))
	{
		printf("Error: Offset is outdated\n");
		return 0;
	}
	if (offset_manager::LoadVars() == false)
	{
		printf("Error: Failed to load vars\n");
		return 0;
		// exit(0);
	}
	if (offset_manager::LoadOffsets() == false)
	{
		printf("Error: Failed to load offsets\n");
		return 0;
		// exit(0);
	}
	while (true)
	{
		if (apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for apex process...\n");

			apex_mem.open_proc(ap_proc);

			if (apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				printf("\nApex process found\n");
				printf("Base: %lx\n", g_Base);
				// printf("Done\n");
				std::thread(DoActions).detach();
				std::thread(AimbotLoop).detach();
				std::thread(NoRecoilThread).detach();
				std::thread(sTriggerbotThread).detach();
				std::thread(item_glow_t).detach();
				printf("thread started\n");
			}
		}
		else
		{
			apex_mem.check_proc();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return 0;
}