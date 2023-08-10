#pragma once
#include "Math.h"
#include "offsets.h"
#include "memory.h"
#include "offset_manager.h"
using namespace offsets;
#define NUM_ENT_ENTRIES (1 << 12)
#define ENT_ENTRY_MASK (NUM_ENT_ENTRIES - 1)
struct Matrix
{
	float matrix[16];
};
struct GlowMode
{
	BYTE GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};
struct GColor
{
	float r, g, b;
};
typedef struct Bone
{
	uint8_t pad1[0xCC];
	float x;
	uint8_t pad2[0xC];
	float y;
	uint8_t pad3[0xC];
	float z;
} Bone;

class Entity
{
public:
	uint64_t ptr;
	uint8_t buffer[0x3FF0];
	Vector getPosition();
	bool isDummy();
	bool isPlayer();
	bool isKnocked();
	bool isAlive();
	bool isBleedOut();
	bool isDead();
	float lastVisTime();
	float getFYaw();
	float getFPitch();
	float getFRoll();
	int getTeamId();
	int getHealth();
	int getArmor();
	int getShield();
	int getMaxShield();

	bool isGlowing();
	bool isZooming();
	bool isOnGround();
	Vector getAbsVelocity();
	QAngle GetSwayAngles();
	QAngle GetViewAngles();
	Vector GetCamPos();
	QAngle GetRecoil();
	Vector GetViewAnglesV();
	QAngle GetCamAngles();
	void enableGlow();

	void disableGlow();
	void SetViewAngles(SVector angles);
	void SetViewAngles(QAngle &angles);
	Vector getBonePosition(int id);
	Vector getstudiohdr(int id);
	void get_name(uint64_t g_Base, uint64_t index, char *name);
	void NullName(uint64_t g_Base, uint64_t index);
};

class Item
{
public:
	uint64_t ptr;
	uint8_t buffer[0x3FF0];
	Vector getPosition();
	bool isItem();
	bool isGlowing();
	void enableGlowItem();
	bool isBox();
	bool isTrap();
	void enableGlow();
	void disableGlow();
};

class WeaponXEntity
{
public:
	void update(uint64_t LocalPlayer);
	float get_projectile_speed();
	float get_projectile_gravity();
	float get_zoom_fov();

private:
	float projectile_scale;
	float projectile_speed;
	float zoom_fov;
};

struct ClientClass
{
	uint64_t pCreateFn;
	uint64_t pCreateEventFn;
	uint64_t pNetworkName;
	uint64_t pRecvTable;
	uint64_t pNext;
	uint32_t ClassID;
	uint32_t ClassSize;
};

class HitBoxManager
{
private:
public:
	Vector ScreenHeadBone{};
	Vector ScreenNeckBone{};
	Vector ScreenChestBone{};
	Vector ScreenWaistBone{};
	Vector ScreenBotmBone{};

	Vector ScreenLeftshoulderBone{};
	Vector ScreenLeftelbowBone{};
	Vector ScreenLeftHandBone{};
	Vector ScreenRightshoulderBone{};
	Vector ScreenRightelbowBone{};
	Vector ScreenRightHandBone{};

	Vector ScreenLeftThighsBone{};
	Vector ScreenLeftkneesBone{};
	Vector ScreenLeftlegBone{};
	Vector ScreenRightThighsBone{};
	Vector ScreenRightkneesBone{};
	Vector ScreenRightlegBone{};
};
enum WeaponId
{
	r301 = 0,
	sentinel = 1,
	bow = 2,
	sheila_stationary = 9,
	rampage = 18,
	fists = 26,
	alternator = 104,
	re45,
	chargerifle,
	devotion,
	longbow,
	havoc,
	eva8,
	flatline,
	scout,
	hemlock,
	kraber,
	lstar,
	mastiff,
	mozambique,
	prowler,
	peacekeeper,
	r99,
	p2020,
	spitfire,
	tripletake,
	wingman,
	volt,
	repeater,
	car,
	shield,
	throwing_knife
};
Entity getEntity(uintptr_t ptr);
Item getItem(uintptr_t ptr);
bool WorldToScreen(Vector from, float *m_vMatrix, Vector &to);
float CalculateFov(Entity &from, Entity &target);
QAngle CalculateBestBoneAim(Entity &from, uintptr_t target, float max_fov);
void get_class_name(uint64_t entity_ptr, char *out_str);
int BoneByHitBox(int HitBox, uint64_t local_ent);
Vector HitBoxPos(int HitBox, uint64_t local_ent, Vector EntityPosition);
Vector PlayerPredict(Vector LocalPos, Vector EntityPosition, Vector vel, float bulletspeed, float bulletgravity);
Vector prediction(Vector LPlayerpos, Vector Targetpos, Entity &from, Entity &target);
HitBoxManager getHitbox(uint64_t local_ent, Vector EntityPosition, Matrix mtr);
int getBestBoneposition(Entity &from);
float CalculateboneFov(Entity &from, Entity &target, int bone);
int getBestBoneposition(Entity &from, Entity &target);
float CalculateboneFov(Entity &from, Entity &target, int bone);
enum class ButtonCode : int
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,
	KEY_FIRST = 0,
	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,
	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,
	MOUSE_FIRST = KEY_LAST + 1,
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP,
	MOUSE_WHEEL_DOWN,
	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1,
};
