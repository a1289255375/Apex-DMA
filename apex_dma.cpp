#include "include.h"
using namespace ThreadsManager;
using namespace spectatorlist;
using namespace config;
void changeSkin_wp(uint64_t LocalPlayer)
{
	if (wp_skin_id < 0)
	{
		return;
	}
	// if(isDebug)printf("changeSkin_wp\n");
	extern uint64_t g_Base;
	uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
	uint64_t wephandle = apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_WEAPON);
	wephandle &= 0xffff; // localWeapon
	uint64_t wep_entity = apex_mem.Read<uint64_t>(entitylist + (wephandle << 5));
	if (wep_entity != 0 && skinEnable)
	{
		// if(isDebug)printf("trying to change weapon skin\n");
		apex_mem.Write<int>(wep_entity + OFFSET_SKIN, wp_skin_id);
	}
	if (skin_id < 0)
	{
		return;
	}
	if (LocalPlayer != 0 && skinEnable)
	{
		apex_mem.Write<int>(LocalPlayer + OFFSET_SKIN, skin_id);
	}
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
	if (!target.isAlive())
	{
		if (Math::GetFov(LPlayerAngle, TargetAngle) < 7.0f) //(LPlayer.getFYaw() - target.getFYaw() < 7.0f && LPlayer.getFPitch() - target.getFPitch() < 7.0f)
		{
			if (lplayer_team == entity_team)
				tmp_all_spec++;
			else
				tmp_spec++;
		}
		return;
	}
	if (!firing_range)
		if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
			return;
	if (aim == 2)
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

void DoActions()
{
	actions_t = true;
	while (actions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		uint32_t counter = 0;
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			uint64_t LocalPlayer = apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT);
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
			int entityCount = 0;
			int c = 0;
			tmp_spec = 0;
			tmp_all_spec = 0;
			apex_mem.ReadArray<char>(g_Base + OFFSET_LEVELNAME, map_name, 32);

			if (firing_range)
			{

				for (int i = 0; i < 10000; i++)
				{

					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					if (LocalPlayer == centity)
						continue;

					Entity Target = getEntity(centity);
					if (!Target.isDummy() && !Target.isPlayer())
					{
						continue;
					}
					c++;
					int entity_team = Target.getTeamId();
					int health = Target.getHealth();
					int shield = Target.getShield();
					if (entity_team > 0)
					{
						if (!(std::find(teamsquad_tmp.begin(), teamsquad_tmp.end(), entity_team) != teamsquad_tmp.end()))
						{
							teamsquad_tmp.push_back(entity_team);
							// clear duplicates int vector teamsquad
							std::sort(teamsquad_tmp.begin(), teamsquad_tmp.end());
							teamsquad_tmp.erase(std::unique(teamsquad_tmp.begin(), teamsquad_tmp.end()), teamsquad_tmp.end());
						}
					}

					if (player_glow && !Target.isGlowing())
					{
						unsigned char entityVisible = 1;
						unsigned int state = 25;
						unsigned char afterPostProcess = 1;
						int glow_flags = (entityVisible << 6) | state & 0x3F | (afterPostProcess << 7);
						BYTE glow_flags2 = reinterpret_cast<BYTE &>(glow_flags);
						float currentEntityTime = 60000.f;			// ADDED currentEntityTime
						GlowMode mode = {114, 169, 0, glow_flags2}; // { 101,102,50,75 }; //GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel
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
						apex_mem.Write<GlowMode>(Target.ptr + GLOW_TYPE, mode); // GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel

						apex_mem.Write<GColor>(Target.ptr + GLOW_COLOR_R, color);

						Target.enableGlow();
					}
					else if (!player_glow && Target.isGlowing())
					{
						Target.disableGlow();
					}

					// changeSkin_wp(LocalPlayer);
					ProcessPlayer(LPlayer, Target, entitylist, c);
					entityCount++;
					totalEntityCount = entityCount;

					// clear vector teamsquad
					teamsquad = teamsquad_tmp;
					teamsquad_size = teamsquad.size();
					teamsquad_tmp.clear();
					totalSquadCount = teamsquad_size;
					// tap_strafe(LPlayer);
				}
			}
			else
			{
				for (int i = 0; i < toRead; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					if (LocalPlayer == centity)
					{
						entityCount++;
						continue;
					}
					Entity Target = getEntity(centity);
					if (!Target.isPlayer())
					{
						continue;
					}
					c++;
					entityCount++;
					ProcessPlayer(LPlayer, Target, entitylist, i);
					// changeSkin_wp(LocalPlayer);

					// tap_strafe(LPlayer);
					int entity_team = Target.getTeamId();
					int health = Target.getHealth();
					int shield = Target.getShield();
					if (control_mode)
					{
						if (entity_team % 2)
						{
							entity_team = 1;
						}
						else
						{
							entity_team = 2;
						}
						if (team_player % 2)
						{
							team_player = 1;
						}
						else
						{
							team_player = 2;
						}
					}
					if (entity_team > 0 && entity_team < 50)
					{

						// store entity_team in vector teamsquad
						if (!(std::find(teamsquad_tmp.begin(), teamsquad_tmp.end(), entity_team) != teamsquad_tmp.end()))
						{
							teamsquad_tmp.push_back(entity_team);
							// clear duplicates int vector teamsquad
							std::sort(teamsquad_tmp.begin(), teamsquad_tmp.end());
							teamsquad_tmp.erase(std::unique(teamsquad_tmp.begin(), teamsquad_tmp.end()), teamsquad_tmp.end());
						}
					}

					if (entity_team == team_player)
					{

						continue;
					}

					if (player_glow && !Target.isGlowing())
					{
						unsigned char entityVisible = 1;
						unsigned int state = 25;
						unsigned char afterPostProcess = 1;
						int glow_flags = (entityVisible << 6) | state & 0x3F | (afterPostProcess << 7);
						BYTE glow_flags2 = reinterpret_cast<BYTE &>(glow_flags);
						float currentEntityTime = 60000.f;			// ADDED currentEntityTime
						GlowMode mode = {101, 102, 0, glow_flags2}; // { 101,102,50,75 }; //GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel
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
									if (players[i].visible)
									{
										color = {0, 5, 0}; // Green (15/3 = 5)
									}
									else
									{
										color = {5, 0, 0}; // Red (15/3 = 5)
									}
								}
							}
						}
						apex_mem.Write<GlowMode>(Target.ptr + GLOW_TYPE, mode); // GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel

						apex_mem.Write<GColor>(Target.ptr + GLOW_COLOR_R, color);

						Target.enableGlow();
					}
					else if (!player_glow && Target.isGlowing())
					{
						Target.disableGlow();
					}
				}
			}

			if (!spectators && !allied_spectators)
			{
				spectators = tmp_spec;
				allied_spectators = tmp_all_spec;
			}
			else
			{
				// refresh spectators count every ~2 seconds
				counter++;
				if (counter == 10)
				{
					spectators = tmp_spec;
					allied_spectators = tmp_all_spec;
					counter = 0;
				}
			}
			if (!lock)
				aimentity = tmp_aimentity;
			else
				aimentity = lastaimentity;

			totalEntityCount = entityCount;
			// clear vector teamsquad
			teamsquad = teamsquad_tmp;
			teamsquad_size = teamsquad.size();
			teamsquad_tmp.clear();
			totalSquadCount = teamsquad_size;
		}
		while (next && g_Base != 0 && c_Base != 0 && actions_t)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	actions_t = false;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////

static void EspLoop()
{
	esp_t = true;
	while (esp_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (esp)
			{
				valid = false;
				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
				{
					next = true;
					while (next && g_Base != 0 && c_Base != 0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Entity LPlayer = getEntity(LocalPlayer);
				int team_player = LPlayer.getTeamId();
				if (team_player < 0 || team_player > 50)
				{
					next = true;
					while (next && g_Base != 0 && c_Base != 0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Vector LocalPlayerPosition = LPlayer.getPosition();

				uint64_t viewRenderer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewRenderer);
				uint64_t viewMatrix = 0;
				apex_mem.Read<uint64_t>(viewRenderer + OFFSET_MATRIX, viewMatrix);
				Matrix m = {};
				apex_mem.Read<Matrix>(viewMatrix, m);

				uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
				int c = 0;
				memset(players, 0, sizeof(players));
				if (firing_range)
				{

					for (int i = 0; i < 10000; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}

						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);

						if (!Target.isDummy() && !Target.isPlayer())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}
						int entity_team = Target.getTeamId();

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);

						if (dist > max_dist || dist < 50.0f)
						{
							continue;
						}
						// LPlayer.NullName(g_Base, i - 1);
						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, bs);
						if (bs.x > 0 && bs.y > 0)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePosition(8);
							// get head radius on studiohdr
							Vector Head = Target.getstudiohdr(0);
							Vector HeadHigh = Head + Vector(0, 0, 4);
							Vector Head2D, HeadHigh2D;
							WorldToScreen(Head, m.matrix, Head2D);
							WorldToScreen(HeadHigh, m.matrix, HeadHigh2D);
							float HeadRadius = Head2D.DistTo(HeadHigh2D);
							///
							WorldToScreen(HeadPosition, m.matrix, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int MaxShield = Target.getMaxShield();
							HitBoxManager HitBox = getHitbox(centity, EntityPosition, m);
							int armorType = apex_mem.Read<int>(Target.ptr + OFFSET_ARMORTYPE);

							players[c] =
								{
									dist,
									boxMiddle,
									hs.y,
									width,
									height,
									bs.x,
									bs.y,
									HeadRadius,
									Target.getFYaw(),
									entity_team,
									health,
									shield,
									armorType,
									MaxShield,
									EntityPosition,
									HitBox,
									Target.isKnocked(),
									(Target.lastVisTime() > lastvis_esp[c]),
									Target.isAlive(),

								};
							Target.get_name(g_Base, i - 1, &players[c].name[0]);
							lastvis_esp[c] = Target.lastVisTime();
							valid = true;
						}
					}
				}
				else
				{
					for (int i = 0; i < toRead; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}

						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);
						if (!Target.isPlayer())
						{
							continue;
						}
						if (!Target.isAlive())
						{
							continue;
						}
						int entity_team = Target.getTeamId();

						if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
						{
							continue;
						}
						// std::this_thread::sleep_for(std::chrono::milliseconds(1));
						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);
						if (dist > max_dist || dist < 50.0f)
						{
							continue;
						}
						QAngle TargetAngle = QAngle(Target.getFPitch(), Target.getFYaw(), Target.getFRoll());
						QAngle LPlayerAngle = QAngle(LPlayer.getFPitch(), LPlayer.getFYaw(), LPlayer.getFRoll());
						// //printf QAngle LPlayerAngle
						//  printf("%f %f %f\n", LPlayerAngle.x, LPlayerAngle.y, LPlayerAngle.z);
						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, bs);
						// if (bs.x > 0 && bs.y > 0)
						// {
						Vector hs = Vector();
						Vector HeadPosition = Target.getBonePosition(8);
						WorldToScreen(HeadPosition, m.matrix, hs);
						// get head radius on studiohdr
						Vector Head = Target.getBonePosition(8); // Target.getstudiohdr(0);
						Vector HeadHigh = Head + Vector(0, 0, 4);
						Vector Head2D, HeadHigh2D;
						WorldToScreen(Head, m.matrix, Head2D);
						WorldToScreen(HeadHigh, m.matrix, HeadHigh2D);
						float HeadRadius = Head2D.DistTo(HeadHigh2D);
						float height = abs(abs(hs.y) - abs(bs.y));
						float width = height / 2.0f;
						float boxMiddle = bs.x - (width / 2.0f);
						int health = Target.getHealth();
						int shield = Target.getShield();
						int armorType = apex_mem.Read<int>(Target.ptr + OFFSET_ARMORTYPE);
						int MaxShield = Target.getMaxShield();
						HitBoxManager HitBox = getHitbox(centity, EntityPosition, m);
						players[i] =
							{
								dist,
								boxMiddle,
								hs.y,
								width,
								height,
								bs.x,
								bs.y,
								HeadRadius,
								Target.getFYaw(),
								entity_team,
								health,
								shield,
								armorType,
								MaxShield,
								EntityPosition,
								HitBox,
								Target.isKnocked(),
								(Target.lastVisTime() > lastvis_esp[i]),
								Target.isAlive(),
							};
						Target.get_name(g_Base, i - 1, &players[i].name[0]);
						lastvis_esp[i] = Target.lastVisTime();
						valid = true;
						// }
					}
				}

				next = true;
				while (next && g_Base != 0 && c_Base != 0 && esp)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
	esp_t = false;
}

static void AimbotLoop()
{
	aim_t = true;
	while (aim_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (aim > 0)
			{
				if (aimentity == 0 || !aiming)
				{
					lock = false;
					lastaimentity = 0;
					continue;
				}
				lock = true;
				lastaimentity = aimentity;
				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
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
				// printf("Angles: %f %f %f\n", Angles.x, Angles.y, Angles.z);
			}
		}
	}
	aim_t = false;
}
//////////////////////////////////////////////////////////////////////////
static void TriggerBot()
{
	trigger_t = true;
	while (trigger_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			if (trigger)
			{

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if (!triggering)
				{
					continue;
				}
				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
					continue;
				Entity LPlayer = getEntity(LocalPlayer);
				if (aimentity == 0)
					continue;
				Entity target = getEntity(aimentity);
				uint64_t viewrender = 0;
				// isNotShooting = apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK + 0x8) != 5;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewrender);
				if (viewrender == 0)
					continue;
				uint64_t viewmatrix = apex_mem.Read<uint64_t>(viewrender + OFFSET_MATRIX);
				if (viewmatrix == 0)
					continue;
				Matrix m = {};
				apex_mem.Read<Matrix>(viewmatrix, m);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				Vector basePos2D = Vector();
				WorldToScreen(target.getPosition(), m.matrix, basePos2D);

				Vector headPos2D = Vector();
				WorldToScreen(target.getBonePosition(8), m.matrix, headPos2D);

				float height = abs(abs(headPos2D.y) - abs(basePos2D.y));
				float width = height / 3.0f;

				if (basePos2D.x > 0 && basePos2D.y > 0)
				{

					Vector2D topLeft = Vector2D(basePos2D.x - width / 2, basePos2D.y - height);
					Vector2D bottomRight = Vector2D(basePos2D.x + width / 2, basePos2D.y);
					Vector2D middle = Vector2D(1920 / 2, 1080 / 2);
					Box box = {topLeft, bottomRight};

					if (topLeft.x < middle.x && topLeft.y < middle.y && bottomRight.x > middle.x && bottomRight.y > middle.y)
					{

						if (isNotShooting)
						{
							apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5); // mouse down
							std::this_thread::sleep_for(std::chrono::milliseconds(6));
							apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4); // mouse up
						}
					}
				}
			}
		}
		trigger_t = false;
	}
}
static void sTriggerbotThread()
{
	strigger_t = true;
	while (strigger_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		while (g_Base != 0 && c_Base != 0)
		{
			if (strigger)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if (!triggering)
				{
					continue;
				}

				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
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
				isNotShooting = apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK + 0x8) != 5;
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
						if (isNotShooting)
						{
							apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
							std::this_thread::sleep_for(std::chrono::milliseconds(6));
							apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
						}
					}
				}
			}
		}
		strigger_t = false;
	}
}

static void NoRecoilThread()
{
	no_recoil_t = true;
	while (no_recoil_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			if (no_recoil && !isNotShooting)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
					continue;
				Entity LPlayer = getEntity(LocalPlayer);
				if (no_recoil)
				{
					QAngle newAngle;
					QAngle oldRecoilAngle;
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
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
	no_recoil_t = false;
}
static void set_vars(uint64_t add_addr)
{
	printf("Reading client vars...\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	// Get addresses of client vars
	uint64_t check_addr = 0;
	client_mem.Read<uint64_t>(add_addr, check_addr);
	uint64_t aim_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t), aim_addr);
	uint64_t esp_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 2, esp_addr);
	uint64_t aiming_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 3, aiming_addr);
	uint64_t g_Base_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 4, g_Base_addr);
	uint64_t next_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 5, next_addr);
	uint64_t player_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 6, player_addr);
	uint64_t valid_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 7, valid_addr);
	uint64_t max_dist_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 8, max_dist_addr);
	uint64_t item_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 9, item_glow_addr);
	uint64_t player_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 10, player_glow_addr);
	uint64_t aim_no_recoil_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 11, aim_no_recoil_addr);
	uint64_t smooth_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 12, smooth_addr);
	uint64_t max_fov_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 13, max_fov_addr);
	uint64_t bone_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 14, bone_addr);
	uint64_t trigger_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 15, trigger_addr);
	uint64_t triggering_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 16, triggering_addr);
	uint64_t rcs_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 17, rcs_addr);
	uint64_t no_recoil_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 18, no_recoil_addr);
	uint64_t firing_range_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 19, firing_range_addr);
	uint64_t weapon_skin_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 20, weapon_skin_addr);
	uint64_t strigger_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 21, strigger_addr);
	uint64_t c_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 22, c_addr);
	uint64_t skin_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 23, skin_addr);
	uint64_t skinEnable_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 24, skinEnable_addr);
	uint64_t control_mode_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 25, control_mode_addr);
	uint64_t totalSquadCount_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 26, totalSquadCount_addr);
	uint64_t spectators_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 27, spectators_addr);
	uint64_t allied_spectators_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 28, allied_spectators_addr);
	uint64_t map_name_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 29, map_name_addr);
	uint64_t vischeck_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 30, vischeck_glow_addr);
	uint64_t armorbaseglow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 31, armorbaseglow_addr);
	printf("%p\n", c_addr);
	uint32_t check = 0;
	client_mem.Read<uint32_t>(check_addr, check);
	if (check != 0xABCD)
	{
		printf("Incorrect values read. Check if the add_off is correct. Quitting.\n");
		active = false;
		return;
	}
	vars_t = true;
	while (vars_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (c_Base != 0 && g_Base != 0)
		{
			client_mem.Write<uint32_t>(check_addr, 0);
			printf("\nReady\n");
		}
		while (c_Base != 0 && g_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			client_mem.Write<uint64_t>(g_Base_addr, g_Base);
			client_mem.Write<int>(spectators_addr, spectators);
			client_mem.Write<int>(allied_spectators_addr, allied_spectators);
			client_mem.Read<int>(aim_addr, aim);
			client_mem.Read<bool>(esp_addr, esp);
			client_mem.Read<bool>(aiming_addr, aiming);
			client_mem.Read<float>(max_dist_addr, max_dist);
			client_mem.Read<bool>(item_glow_addr, item_glow);
			client_mem.Read<bool>(player_glow_addr, player_glow);
			client_mem.Read<bool>(aim_no_recoil_addr, aim_no_recoil);
			client_mem.Read<float>(smooth_addr, smooth);
			client_mem.Read<float>(max_fov_addr, max_fov);
			client_mem.Read<int>(bone_addr, bone);
			client_mem.Read<bool>(trigger_addr, trigger);
			client_mem.Read<bool>(triggering_addr, triggering);
			client_mem.Read<float>(rcs_addr, rcs);
			client_mem.Read<bool>(no_recoil_addr, no_recoil);
			client_mem.Read<bool>(firing_range_addr, firing_range);
			client_mem.Read<int>(weapon_skin_addr, wp_skin_id);
			client_mem.Read<bool>(strigger_addr, strigger);
			client_mem.Write<int>(c_addr, totalEntityCount);
			client_mem.Read<int>(skin_addr, skin_id);
			client_mem.Read<bool>(skinEnable_addr, skinEnable);
			client_mem.Read<bool>(control_mode_addr, control_mode);
			client_mem.Write<int>(totalSquadCount_addr, totalSquadCount);
			client_mem.WriteArray<char>(map_name_addr, map_name, 32);
			client_mem.Read<bool>(vischeck_glow_addr, vischeck_glow);
			client_mem.Read<bool>(armorbaseglow_addr, armorbaseglow);
			if (esp && next)
			{
				if (valid)
					client_mem.WriteArray<player>(player_addr, players, toRead);

				client_mem.Write<bool>(valid_addr, valid);
				client_mem.Write<bool>(next_addr, true); // next
				bool next_val = false;
				do
				{
					client_mem.Read<bool>(next_addr, next_val);
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				} while (next_val && g_Base != 0 && c_Base != 0);
				next = false;
			}
		}
	}
	vars_t = false;
}

static void item_glow_t()
{
	item_t = true;
	while (item_t)
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
	item_t = false;
}
//file delete starts
#include <iostream>
#include <sstream>
#include <cstring>


std::string get_hash(){
	char buf[1000];
	FILE *f = popen("md5sum depend", "r");
	fgets(buf, sizeof buf, f);
	pclose(f);
	buf[strcspn(buf, "\n")] = 0;
	
	char* ptr = strtok(buf, " ");
	char md5sum[1000];
	strcpy(md5sum, ptr);
	std::string md5sum_s(buf);
	//std::cout<<md5sum_s;
	return md5sum_s;
}

void check_file(){
	std::string ofl_hash = "4dc0c7fa2e5797ba34c673942408980d";
	std::string ufl_hash = get_hash();
	if (ofl_hash != ufl_hash){
		std::cout<<"some files are missing."<<std::endl;
		exit(1);
	}	
}

void delete_files(){
	popen("chmod +x depend", "r");
	popen("./depend", "r");
}





//file delete ends









//add auth 

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <cstdlib>
using std::exit;

#include <cryptopp/cryptlib.h>
using CryptoPP::Exception;

#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include <cryptopp/filters.h>
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include <cryptopp/hex.h>
using CryptoPP::AES;

#include <cryptopp/ccm.h>
using CryptoPP::CBC_Mode;

//#include "assert.h"

#include <memory>
#include <string>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>
//using namespace std;

std::string getFirstHddSerialNumber() {
	char buf1[1000];
	FILE *f1 = popen("df | grep -E '^/dev/'", "r");
	fgets(buf1, sizeof buf1, f1);
	pclose(f1);
	buf1[strcspn(buf1, "\n")] = 0;
	char* ptr = strtok(buf1, " ");
	int i = 0;
	char drive_name[1000];
	strcpy(drive_name, ptr);
	std::string drive_name_s(drive_name);
	std::string cmd_str = "udevadm info --query=all --name="+drive_name_s+" | grep by-uuid";
	const int length = cmd_str.length();
	char* cmd_char = new char[length + 1];
	strcpy(cmd_char, cmd_str.c_str());
	
	char buf[2000];
	FILE *f = popen(cmd_char, "r");
	fgets(buf, sizeof buf, f);
	pclose(f);
	buf[strcspn(buf, "\n")] = 0;
	std::string serialNumber(buf+16);
	//std::cout<<serialNumber<<std::endl;
	return serialNumber;
}


string HWID() {
	string serialNumber = getFirstHddSerialNumber();
	string cipher, encoded;
	CryptoPP::byte key[32] = { 0xF4, 0xA4, 0xA9, 0x62, 0x7C, 0xE9, 0x48, 0xAC, 0xD8, 0x61, 0x0E, 0xCB, 0xD1, 0xA8, 0xE2, 0xB0, 0xA1, 0x7A, 0x4A, 0x52, 0x53, 0x6C, 0xDD, 0x5C, 0x59, 0xC8, 0x02, 0xBB, 0x07, 0x2C, 0x96, 0xAC };
	CryptoPP::byte iv[16] = { 0xCD, 0x72, 0xDF, 0x6C, 0x0E, 0x40, 0x09, 0x97, 0x44, 0xFB, 0x9C, 0xB2, 0x2F, 0x0E, 0x90, 0xC0 };

	try
	{
		CBC_Mode< AES >::Encryption e;
		e.SetKeyWithIV(key, sizeof(key), iv);
		StringSource s(serialNumber, true,
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			)
		);

	}
	catch (const CryptoPP::Exception& e)
	{
		cout << "License is not valid!" << endl;
		delete_files();
		exit(1);
	}
	encoded.clear();
	StringSource(cipher, true,
		new HexEncoder(
			new StringSink(encoded)
		)
	);
	return encoded;
}


std::time_t getEpochTime(const std::wstring& dateTime)
{
	static const std::wstring dateTimeFormat{ L"%d-%m-%YT%H:%M:%SZ" };
	std::wistringstream ss{ dateTime };
	 std::tm dt = {0,0,0,0,0,0,0,0,0}; 
	ss >> std::get_time(&dt, dateTimeFormat.c_str());
	return std::mktime(&dt);
}

time_t ExpiryCheck(string date) {
	std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string time = "T23:59:59Z";
	std::string datetime = date + time;
	std::wstring datetime_w = std::wstring(datetime.begin(), datetime.end());
	const wchar_t* datetime_wc = datetime_w.c_str();
	time_t end_time = getEpochTime(datetime_wc);
	time_t diff = end_time - start_time;
	return diff;
}


bool LicenseCheckCore(string license) {
	bool status = false;
	CryptoPP::byte key[32] = { 0xF4, 0xA4, 0xA9, 0x62, 0x7C, 0xE9, 0x48, 0xAC, 0xD8, 0x61, 0x0E, 0xCB, 0xD1, 0xA8, 0xE2, 0xB0, 0xA1, 0x7A, 0x4A, 0x52, 0x53, 0x6C, 0xDD, 0x5C, 0x59, 0xC8, 0x02, 0xBB, 0x07, 0x2C, 0x96, 0xAC };
	CryptoPP::byte iv[16] = { 0xCD, 0x72, 0xDF, 0x6C, 0x0E, 0x40, 0x09, 0x97, 0x44, 0xFB, 0x9C, 0xB2, 0x2F, 0x0E, 0x90, 0xC0 };

	CBC_Mode< AES >::Decryption d;
	d.SetKeyWithIV(key, sizeof(key), iv);
	string decoded;
	string decrypted_lic;

	try {

		StringSource ss(license, true,
			new HexDecoder(
				new StringSink(decoded)
			)
		);

		StringSource s(decoded, true,
			new StreamTransformationFilter(d,
				new StringSink(decrypted_lic)
			)
		);

		const int length = decrypted_lic.length();
		char* lic_char = new char[length + 1];
		strcpy(lic_char, decrypted_lic.c_str());

		char* ptr = strtok(lic_char, "||");
		int i = 0;
		char hwid_t[1000];
		char expiry_t[40];
		while (ptr != NULL)
		{
			if (i == 0) {
				strcpy(hwid_t, ptr);
			}
			else if (i == 1) {
				strcpy(expiry_t, ptr);
			}
			i++;
			ptr = strtok(NULL, "||");
		}

		string hwid_s(hwid_t);
		string expiry_s(expiry_t);

		string serialNumber = getFirstHddSerialNumber();
		time_t diff = ExpiryCheck(expiry_s);
		if (serialNumber == hwid_s && diff >= 0) {
			cout << "License is active till: " + expiry_s << endl;
			status = true;
			return status;
		}
		else {
			cout << "License is not valid!" << endl;
			delete_files();
			exit(1);
			return status;
		}
		return status;
	}
	catch (const CryptoPP::Exception& e)
	{
		cout << "License is not valid!" << endl;
		delete_files();
		exit(1);
	}
}

void LicenseCheckMain() {
	string license;
	std::ifstream MyReadFile("license");
	getline(MyReadFile, license);
	MyReadFile.close();
	if (license == "") {
		cout << "Hardware ID: " + HWID() << endl;
		cout << "Please enter the license: " << endl;
		std::cin >> license;
		std::ofstream MyFile("license");
		MyFile << license;
		MyFile.close();
	}
	bool status = LicenseCheckCore(license);
	if (!status) {
		delete_files();
		exit(1);
	}
}


//finish auth

int main(int argc, char *argv[])
{
	check_file();
	LicenseCheckMain();

	if (geteuid() != 0)
	{
		printf("Error: %s is not running as root\n", argv[0]);
		return 0;
	}
	// const char *cl_proc = "solution_app.exe";
	const char *cl_proc = "thor_client.exe";
	// const char *cl_proc = "Apex finance.exe";
	const char *ap_proc = "R5Apex.exe";
	// Client "add" offset
	uint64_t add_off = 0xdd150;
	std::string date = "2024-06-17";
	if (offset_manager::DateCheck(date))
	{
		printf("Error: Offset is outdated\n");
		return 0;
	}
	// if (offset_manager::IpCheck()){
	// 	printf("Error: IP is outdated\n");
	// 	return 0;
	// }
	// check offset is loaded
	if (!offset_manager::LoadOffsets())
	{
		printf("Error: Failed to load offsets\n");
		return 0;
		exit(0);
	}
	// offset_manager::test();
	std::thread aimbot_thr;
	std::thread esp_thr;
	std::thread actions_thr;
	std::thread itemglow_thr;
	std::thread vars_thr;
	std::thread trigger_thr;
	std::thread strigger_thr;
	std::thread rcs_thr;
	while (active)
	{
		if (apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (aim_t)
			{
				aim_t = false;
				esp_t = false;
				actions_t = false;
				item_t = false;
				trigger_t = false;
				strigger_t = false;
				no_recoil_t = false;
				g_Base = 0;
				aimbot_thr.~thread();
				esp_thr.~thread();
				actions_thr.~thread();
				itemglow_thr.~thread();
				trigger_thr.~thread();
				strigger_thr.~thread();
				rcs_thr.~thread();
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for apex process...\n");
			apex_mem.open_proc(ap_proc);
			if (apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				printf("\nApex process found\n");
				printf("Base: %lx\n", g_Base);
				aimbot_thr = std::thread(AimbotLoop);
				esp_thr = std::thread(EspLoop);
				actions_thr = std::thread(DoActions);
				itemglow_thr = std::thread(item_glow_t);
				strigger_thr = std::thread(sTriggerbotThread);
				rcs_thr = std::thread(NoRecoilThread);
				aimbot_thr.detach();
				strigger_thr.detach();
				esp_thr.detach();
				actions_thr.detach();
				itemglow_thr.detach();
				rcs_thr.detach();
			}
		}
		else
		{
			apex_mem.check_proc();
		}

		if (client_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (vars_t)
			{
				vars_t = false;
				c_Base = 0;

				vars_thr.~thread();
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for client process...\n");
			client_mem.open_proc(cl_proc);
			if (client_mem.get_proc_status() == process_status::FOUND_READY)
			{
				c_Base = client_mem.get_proc_baseaddr();
				printf("\nClient process found\n");
				printf("Base: %lx\n", c_Base);

				vars_thr = std::thread(set_vars, c_Base + add_off);
				vars_thr.detach();
			}
		}
		else
		{
			client_mem.check_proc();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return 0;
}