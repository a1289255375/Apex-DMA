#include "prediction.h"
#include <cmath>
extern Memory apex_mem;

extern bool firing_range;
bool aim_no_recoil = true;
bool bestbone_bool = false;
class Matrix3x4
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
		};

		float m[3][4];
		float mm[12];
	};

	inline Vector &GetAxis(int i)
	{
		return *(Vector *)&m[i][0];
	}
};

int BoneByHitBox(int HitBox, uint64_t local_ent)
{
	// get model ptr
	uint64_t Model = apex_mem.Read<uint64_t>(local_ent + OFFSET_STUDIOHDR);
	if (!Model)
		return -1;

	// get studio hdr
	uint64_t StudioHdr;
	apex_mem.Read<uint64_t>(Model + 0x8, StudioHdr);

	// get hitbox array
	uint16_t HitboxCache;
	apex_mem.Read<uint16_t>(StudioHdr + 0x34, HitboxCache);
	uint64_t HitboxArray = StudioHdr + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));

	uint16_t IndexCache;
	apex_mem.Read<uint16_t>(HitboxArray + 0x4, IndexCache);
	int HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));

	uint16_t Bone;
	apex_mem.Read<uint16_t>(HitboxIndex + HitboxArray + (HitBox * 0x20), Bone);

	return ((Bone < 0) || (Bone > 255)) ? -1 : Bone;
}

Vector HitBoxPos(int HitBox, uint64_t local_ent, Vector EntityPosition)
{
	// get bones base data
	uint64_t Bones = apex_mem.Read<uint64_t>(local_ent + OFFSET_BONECLASS);
	if (!Bones)
		return Vector();

	int Bone = BoneByHitBox(HitBox, local_ent);

	if (Bone == -1)
		return Vector();

	Matrix3x4 BoneMatrix = apex_mem.Read<Matrix3x4>(Bones + (Bone * sizeof(Matrix3x4)));
	Vector BoneOff = {BoneMatrix._14, BoneMatrix._24, BoneMatrix._34};

	Vector Pos = EntityPosition + BoneOff;
	return Pos;
}
Vector Entity::getstudiohdr(int id)
{
	uint64_t bones = apex_mem.Read<uint64_t>(this->ptr + OFFSET_BONECLASS);
	if (!bones)
		return Vector();
	int bone = BoneByHitBox(id, this->ptr);
	if (bone == -1)
		return Vector();
	Matrix3x4 BoneMatrix = apex_mem.Read<Matrix3x4>(bones + (bone * sizeof(Matrix3x4)));
	Vector BoneOff = {BoneMatrix._14, BoneMatrix._24, BoneMatrix._34};
	Vector Pos = getPosition() + BoneOff;
	return Pos;
}
HitBoxManager getHitbox(uint64_t local_ent, Vector EntityPosition, Matrix mtr)
{
	HitBoxManager HitBoxs;
	WorldToScreen(HitBoxPos(0, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenHeadBone);
	WorldToScreen(HitBoxPos(1, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenNeckBone);
	WorldToScreen(HitBoxPos(2, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenChestBone);
	WorldToScreen(HitBoxPos(3, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenWaistBone);
	WorldToScreen(HitBoxPos(4, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenBotmBone);
	WorldToScreen(HitBoxPos(6, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenLeftshoulderBone);
	WorldToScreen(HitBoxPos(7, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenLeftelbowBone);
	WorldToScreen(HitBoxPos(8, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenLeftHandBone);
	WorldToScreen(HitBoxPos(9, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenRightshoulderBone);
	WorldToScreen(HitBoxPos(10, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenRightelbowBone);
	WorldToScreen(HitBoxPos(11, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenRightHandBone);
	WorldToScreen(HitBoxPos(12, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenLeftThighsBone);
	WorldToScreen(HitBoxPos(13, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenLeftkneesBone);
	WorldToScreen(HitBoxPos(14, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenLeftlegBone);
	WorldToScreen(HitBoxPos(16, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenRightThighsBone);
	WorldToScreen(HitBoxPos(17, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenRightkneesBone);
	WorldToScreen(HitBoxPos(18, local_ent, EntityPosition), mtr.matrix, HitBoxs.ScreenRightlegBone);
	return HitBoxs;
}

// https://github.com/CasualX/apexbot/blob/master/src/state.cpp#L104
void get_class_name(uint64_t entity_ptr, char *out_str)
{
	uint64_t client_networkable_vtable;
	apex_mem.Read<uint64_t>(entity_ptr + 8 * 3, client_networkable_vtable);

	uint64_t get_client_class;
	apex_mem.Read<uint64_t>(client_networkable_vtable + 8 * 3, get_client_class);

	uint32_t disp;
	apex_mem.Read<uint32_t>(get_client_class + 3, disp);
	const uint64_t client_class_ptr = get_client_class + disp + 7;

	ClientClass client_class;
	apex_mem.Read<ClientClass>(client_class_ptr, client_class);

	apex_mem.ReadArray<char>(client_class.pNetworkName, out_str, 32);
}

int Entity::getTeamId()
{
	return *(int *)(buffer + OFFSET_TEAM);
}

int Entity::getHealth()
{
	return *(int *)(buffer + OFFSET_HEALTH);
}

int Entity::getShield()
{
	return *(int *)(buffer + OFFSET_SHIELD);
}
int Entity::getArmor()
{
	return *(int *)(buffer + OFFSET_ARMORTYPE);
}
int Entity::getMaxShield()
{
	return *(int *)(buffer + OFFSET_MAXSHIELD);
}

Vector Entity::getAbsVelocity()
{
	return *(Vector *)(buffer + OFFSET_ABS_VELOCITY);
}

Vector Entity::getPosition()
{
	return *(Vector *)(buffer + OFFSET_ORIGIN);
}

bool Entity::isPlayer()
{
	return *(uint64_t *)(buffer + OFFSET_NAME) == 125780153691248;
}

bool Entity::isDummy()
{
	char class_name[33] = {};
	get_class_name(ptr, class_name);

	return strncmp(class_name, "CAI_BaseNPC", 11) == 0;
}

bool Entity::isKnocked()
{
	return *(int *)(buffer + OFFSET_BLEED_OUT_STATE) > 0;
}
bool Entity::isBleedOut()
{
	return *(int *)(buffer + OFFSET_BLEED_OUT_STATE) != 0;
}
bool Entity::isAlive()
{
	return *(int *)(buffer + OFFSET_LIFE_STATE) == 0;
}
bool Entity::isDead()
{
	return *(int *)(buffer + OFFSET_LIFE_STATE) > 0;
}

float Entity::lastVisTime()
{
	return *(float *)(buffer + OFFSET_VISIBLE_TIME);
}

float Entity::getFYaw()
{
	return *(float *)(buffer + OFFSET_FYAW);
}
float Entity::getFPitch()
{
	return *(float *)(buffer + OFFSET_FPITCH);
}
float Entity::getFRoll()
{
	return *(float *)(buffer + OFFSET_FROLL);
}
Vector Entity::getBonePosition(int id)
{
	Vector position = getPosition();
	uintptr_t boneArray = *(uintptr_t *)(buffer + OFFSET_BONES);
	Vector bone = Vector();
	uint32_t boneloc = (id * 0x30);
	Bone bo = {};
	apex_mem.Read<Bone>(boneArray + boneloc, bo);
	bone.x = bo.x + position.x;
	bone.y = bo.y + position.y;
	bone.z = bo.z + position.z;
	return bone;
}

QAngle Entity::GetSwayAngles()
{
	return *(QAngle *)(buffer + OFFSET_BREATH_ANGLES);
}

QAngle Entity::GetViewAngles()
{
	return *(QAngle *)(buffer + OFFSET_VIEWANGLES);
}

Vector Entity::GetViewAnglesV()
{
	return *(Vector *)(buffer + OFFSET_VIEWANGLES);
}

bool Entity::isGlowing()
{
	return *(int *)(buffer + OFFSET_GLOW_ENABLE) == 7;
}
bool Entity::isOnGround()
{
	return (*(uint32_t *)(buffer + OFFSET_FLAGS) & 0x1) != 0;
}
bool Entity::isZooming()
{
	return *(int *)(buffer + OFFSET_ZOOMING) == 1;
}

void Entity::enableGlow()
{
	// apex_mem.Write<int>(ptr + OFFSET_GLOW_T1, 16256);
	// apex_mem.Write<int>(ptr + OFFSET_GLOW_T2, 1193322764);
	apex_mem.Write<int>(ptr + OFFSET_GLOW_ENABLE, 1);
	apex_mem.Write<int>(ptr + OFFSET_GLOW_THROUGH_WALLS, 1);
}
void Item::enableGlowItem()
{
	apex_mem.Write<int>(ptr + OFFSET_GLOW_T1, 16256);
	apex_mem.Write<int>(ptr + OFFSET_GLOW_T2, 1193322764);
	apex_mem.Write<int>(ptr + OFFSET_GLOW_ENABLE, 1);
	apex_mem.Write<int>(ptr + OFFSET_GLOW_THROUGH_WALLS, 2);
}

void Entity::disableGlow()
{
	apex_mem.Write<int>(ptr + OFFSET_GLOW_T1, 0);
	apex_mem.Write<int>(ptr + OFFSET_GLOW_T2, 0);
	apex_mem.Write<int>(ptr + OFFSET_GLOW_ENABLE, 2);
	apex_mem.Write<int>(ptr + OFFSET_GLOW_THROUGH_WALLS, 5);
}

void Entity::SetViewAngles(SVector angles)
{
	apex_mem.Write<SVector>(ptr + OFFSET_VIEWANGLES, angles);
}

void Entity::SetViewAngles(QAngle &angles)
{
	SetViewAngles(SVector(angles));
	// apex_mem.Write<QAngle>(ptr + OFFSET_VIEWANGLES, angles);
}

Vector Entity::GetCamPos()
{
	return *(Vector *)(buffer + OFFSET_CAMERAPOS);
}

QAngle Entity::GetCamAngles()
{
	return *(QAngle *)(buffer + OFFSET_CAMERAANGLES);
}

QAngle Entity::GetRecoil()
{
	return *(QAngle *)(buffer + OFFSET_AIMPUNCH);
}

void Entity::get_name(uint64_t g_Base, uint64_t index, char *name)
{
	index *= 0x10;
	uint64_t name_ptr = 0;
	apex_mem.Read<uint64_t>(g_Base + OFFSET_NAME_LIST + index, name_ptr);
	apex_mem.ReadArray<char>(name_ptr, name, 32);
}
void Entity::NullName(uint64_t g_Base, uint64_t index)
{
	index *= 0x10;
	uint64_t name_ptr = apex_mem.Read<uint64_t>(g_Base + OFFSET_NAME_LIST + index);
	apex_mem.WriteArray<char>(name_ptr, "", 32);
}

bool Item::isItem()
{
	char class_name[33] = {};
	get_class_name(ptr, class_name);

	return strncmp(class_name, "CPropSurvival", 13) == 0;
}
bool Item::isBox()
{
	char class_name[33] = {};
	get_class_name(ptr, class_name);

	return strncmp(class_name, "CDeathBoxProp", 13) == 0;
}

bool Item::isTrap()
{
	char class_name[33] = {};
	get_class_name(ptr, class_name);

	return strncmp(class_name, "caustic_trap", 13) == 0;
}

bool Item::isGlowing()
{
	return *(int *)(buffer + OFFSET_ITEM_GLOW) == 1363184265;
}

void Item::enableGlow()
{
	apex_mem.Write<int>(ptr + OFFSET_ITEM_GLOW, 1363184265);
}

void Item::disableGlow()
{
	apex_mem.Write<int>(ptr + OFFSET_ITEM_GLOW, 1411417991);
}

Vector Item::getPosition()
{
	return *(Vector *)(buffer + OFFSET_ORIGIN);
}

float CalculateFov(Entity &from, Entity &target)
{
	QAngle ViewAngles = from.GetViewAngles();
	Vector LocalCamera = from.GetCamPos();
	Vector EntityPosition = target.getPosition();
	QAngle Angle = Math::CalcAngle(LocalCamera, EntityPosition);
	return Math::GetFov(ViewAngles, Angle);
}
float CalculateboneFov(Entity &from, Entity &target, int bone)
{
	QAngle ViewAngles = from.GetViewAngles();
	Vector LocalCamera = from.GetCamPos();
	Vector EntityBonePos = target.getstudiohdr(bone);
	QAngle Angle = Math::CalcAngle(LocalCamera, EntityBonePos);
	return Math::GetFov(ViewAngles, Angle);
}
int getBestBoneposition(Entity &from, Entity &target)
{
	float bestBone_fov = 9999;
	int bestBoneCurEnt = -1;

	for (int i = 0; i < 19; i++)
	{
		Vector BonePos = target.getstudiohdr(i);
		float fov = CalculateboneFov(from, target, i);
		if (fov < bestBone_fov)
		{
			bestBone_fov = fov;
			bestBoneCurEnt = i;
		}
	}
	if (bestBoneCurEnt == -1)
	{
		bestBone_fov = bone;
		CalculateboneFov(from, target, bone);
	}
	return bestBoneCurEnt;
}
QAngle CalculateBestBoneAim(Entity &from, uintptr_t t, float max_fov)
{
	Entity target = getEntity(t);
	if (firing_range)
	{
		if (!target.isAlive())
		{
			return QAngle(0, 0, 0);
		}
	}
	else
	{
		if (!target.isAlive() || target.isKnocked())
		{
			return QAngle(0, 0, 0);
		}
	}

	Vector LocalCamera = from.GetCamPos();
	// Vector TargetBonePosition = target.getBonePosition(bone);
	// get TartgetBonePosition by using hitboxpos
	Vector TargetBonePosition;

	TargetBonePosition = target.getstudiohdr(bone);

	QAngle CalculatedAngles = QAngle(0, 0, 0);

	WeaponXEntity curweap = WeaponXEntity();
	curweap.update(from.ptr);
	float BulletSpeed = curweap.get_projectile_speed();
	float BulletGrav = curweap.get_projectile_gravity();
	float zoom_fov = curweap.get_zoom_fov();

	if (zoom_fov != 0.0f && zoom_fov != 1.0f)
	{
		max_fov *= zoom_fov / 90.0f;
	}

	// more accurate prediction
	if (BulletSpeed > 1.f)
	{
		PredictCtx Ctx;
		Ctx.StartPos = LocalCamera;
		Ctx.TargetPos = TargetBonePosition;
		Ctx.BulletSpeed = BulletSpeed - (BulletSpeed * 0.08);
		Ctx.BulletGravity = BulletGrav + (BulletGrav * 0.05);
		Ctx.TargetVel = target.getAbsVelocity();

		if (BulletPredict(Ctx))
			CalculatedAngles = QAngle{Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f};
	}

	if (CalculatedAngles == QAngle(0, 0, 0))
		CalculatedAngles = Math::CalcAngle(LocalCamera, TargetBonePosition);
	QAngle ViewAngles = from.GetViewAngles();
	QAngle SwayAngles = from.GetSwayAngles();
	// remove sway and recoil
	if (aim_no_recoil)
		CalculatedAngles -= SwayAngles - ViewAngles;
	Math::NormalizeAngles(CalculatedAngles);
	QAngle Delta = CalculatedAngles - ViewAngles;
	double fov = Math::GetFov(SwayAngles, CalculatedAngles);
	if (fov > max_fov)
	{
		return QAngle(0, 0, 0);
	}

	Math::NormalizeAngles(Delta);

	QAngle SmoothedAngles = ViewAngles + Delta / smooth;

	return SmoothedAngles;
}
Vector prediction(Vector LPlayerpos, Vector Targetpos, Entity &from, Entity &target)
{
	WeaponXEntity curweap = WeaponXEntity();
	curweap.update(from.ptr);
	float BulletSpeed = curweap.get_projectile_speed();
	float BulletGrav = curweap.get_projectile_gravity();
	float distance = LPlayerpos.DistTo(Targetpos);
	float time = distance / BulletSpeed;
	Vector BulletGravChange = {0, BulletGrav * time, 0};
	Vector TargetVel = target.getAbsVelocity();
	Vector MovementChange = TargetVel * time;
	Vector FinalPos = Targetpos + MovementChange + BulletGravChange;
	return FinalPos;
}
Entity getEntity(uintptr_t ptr)
{
	Entity entity = Entity();
	entity.ptr = ptr;
	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
	return entity;
}

Item getItem(uintptr_t ptr)
{
	Item entity = Item();
	entity.ptr = ptr;
	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
	return entity;
}

bool WorldToScreen(Vector from, float *m_vMatrix, Vector &to)
{
	/*1920x1080*/
	int targetWidth = 1920;
	int targetHeight = 1080;
	/*3440x1440*/
	// int targetWidth = 3440;
	// int targetHeight = 1440;
	/*2560x1440*/
	// int targetWidth = 2560;
	// int targetHeight = 1440;
	float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

	if (w < 0.01f)
		return false;

	to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
	to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float x = targetWidth / 2;
	float y = targetHeight / 2;

	x += 0.5 * to.x * targetWidth + 0.5;
	y -= 0.5 * to.y * targetHeight + 0.5;

	to.x = x;
	to.y = y;
	to.z = 0;
	return true;
}

void WeaponXEntity::update(uint64_t LocalPlayer)
{
	extern uint64_t g_Base;
	uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
	uint64_t wephandle = 0;
	apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_WEAPON, wephandle);

	wephandle &= 0xffff;

	uint64_t wep_entity = 0;
	apex_mem.Read<uint64_t>(entitylist + (wephandle << 5), wep_entity);
	int wep_index = 0;
	apex_mem.Read<int>(wep_entity + OFFSET_WEAPONID, wep_index);
	projectile_speed = 0;
	apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SPEED, projectile_speed);
	projectile_scale = 0;
	apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SCALE, projectile_scale);
	int charge_level = 0;
	if (wep_index == WeaponId::bow)
	{ // bocek
		apex_mem.Read<int>(wep_entity + OFFSET_CHARGE_LEVEL, charge_level);
		projectile_speed += pow(charge_level, 5.4684388195808646929601911511950477696820577803882); // log6 18000
	}
	zoom_fov = 0;
	apex_mem.Read<float>(wep_entity + OFFSET_ZOOM_FOV, zoom_fov);
}

float WeaponXEntity::get_projectile_speed()
{
	return projectile_speed;
}

float WeaponXEntity::get_projectile_gravity()
{
	return 750.0f * projectile_scale;
}

float WeaponXEntity::get_zoom_fov()
{
	return zoom_fov;
}
