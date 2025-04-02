#pragma once
#include "attack.h"

enum class CategoryID : int32_t
{
	Stand = 0x0,
	Crouch = 0x1,
	OnAir = 0x2,
	FaceUpFall = 0x3,
	FaceDownFall = 0x4,
	FlyObject = 0x6,
};

enum class SubCategoryID : int32_t
{
	Normal,
	Attack,
	BeAttack,
	Defense,
	Fallen1,
	Fallen2,
	Kneel,
	Throw,
	BeThrown,
};

enum class ActionLineID : int32_t
{
	Body = 0x3,							//身位框
	Hit = 0x4,							//受击框
	Attack = 0x5,						//攻击框
	Cancel = 0x6,						//取消
	Branch = 0x7,						//分支
	Speed = 0xA,						//速度
	BlackOut = 0x13,					//暗转
};

enum class CollisionTypes : int32_t
{
	body = 0x0,
	normal = 0x1,							//受击框
	capture = 0x2,							//被投框
	guard = 0x3,							//防御框
	ground = 0x4,							//地面受击框
	negative = 0x5,							//抵消飞行道具框
	reflects = 0x6,							//反弹飞行道具框
	invulnerability = 0x7,					//霸体框
	parries = 0x8,							//当身框
	avoid = 0x9,							//回避触发框
	parriesex = 0xA,						//投技当身框
	none = 0x9999
};

struct HitRect
{
	float x;
	float y;
	float w;
	float h;
};

struct BaseAnime
{
	int32_t MotionFileID;
	int32_t MotionID;
	float MotionFrame;
	float Blend;
	float Transparent;
	float x;
	float y;
	float z;
	float landY;
};
static_assert(sizeof(BaseAnime) == 0x24, "Size Check");

struct FaceAnime
{
	int32_t MotionFileID;
	int32_t MotionID;
	float MotionFrame;
	float Blend;
};
static_assert(sizeof(FaceAnime) == 0x10, "Size Check");

struct Sound
{
	union
	{
		struct SoundPlay
		{
			float Volume;
			int32_t CommonID;
			int16_t UniqueID;
			int16_t GroupID;
			int32_t LowPassFilter;
			int16_t OptionFlag;
			int16_t TagID;
			int32_t Pan2DValue;
		} play;
		struct SoundStop
		{
			float FadeSec;
			int16_t CommonID;
			int16_t TagID;
			int32_t UniqueID;
			int32_t Reserve3;
			int32_t Reserve4;
			int32_t Reserve5;
		} stop;
		struct SoundStage
		{
			float Volume;
			int32_t FootStepID;
			int16_t Reserve1;
			int16_t Reserve2;
			int32_t LowPassFilter;
			int16_t OptionFlag;
			int16_t TagID;
			int32_t Pan2DValue;
		} stage;
	};
	int32_t Type;
};
static_assert(sizeof(Sound) == 0x1C, "Size Check");

struct Collision
{
	int32_t RectID;
	union {
		int32_t RectAttr;
		CollisionTypes Type;
	};
	int32_t Flag;
	int32_t BranchKey;
	int32_t BindIndex;
	float PushRate;
	HitRect rect;
};
static_assert(sizeof(Collision) == 0x28, "Size check");

struct Attack
{
	int32_t DataID;
	int32_t GroupID;
	HitRect rect;
	uint32_t HitOrGuard : 1;
	uint32_t HitNotGuard : 1;
	uint32_t NotGuard : 1;
};
static_assert(sizeof(Attack) == 0x1C, "Size check");

struct Speed
{
	int32_t SetFlag;
	float MoveX;
	float MoveY;
	float AddX;
	float AddY;
	uint32_t OptionFlag;
};
static_assert(sizeof(Speed) == 0x18, "Size Check");

struct Cancel
{
	uint32_t CancelFlag1;
	uint32_t CancelFlag2;
	uint32_t CancelFlag3;
	uint32_t Terms;
	uint32_t CancelFlag4;
	int32_t PrecedeFrame;
	uint32_t CancelFlag5;
	uint32_t CancelFlag6;
	uint32_t CancelFlag7;
};
static_assert(sizeof(Cancel) == 0x24, "Size Check");

struct Branch
{
	int32_t Type;
	float Param;
	int32_t ActionID;
	int32_t ActionFrame;
	int32_t Type2;
	float Param2;
	uint32_t OptionFlag;
};
static_assert(sizeof(Branch) == 0x1C, "Size Check");

struct Camera
{
	int16_t OffsetBase;
	int16_t Type;
	int32_t Index;
	float Frame;
	float x;
	float y;
	float z;
	uint32_t OptionFlag;
	float BonePosX;
	float BonePosY;
	float BonePosZ;
};
static_assert(sizeof(Camera) == 0x28, "Size Check");

struct BlackOut
{
	int32_t Attr;
	int32_t Type;
	int32_t Frame;
	int32_t FadeFrame;
	uint8_t a;
	uint8_t b;
	uint8_t g;
	uint8_t r;
};
static_assert(sizeof(BlackOut) == 0x14, "Size Check");

struct ActionStructure
{
	int32_t frame;
	union {
		BaseAnime baseanime;
		FaceAnime faceanime;
		Sound sound;
		Collision collision;
		Attack attack;
		Cancel cancel;
		Branch branch;
		Speed speed;
		BlackOut blackout;
	};
	uint32_t padding;
};
static_assert(sizeof(ActionStructure) == 0x30, "Size Check");

struct ActionLine						//ACT指针集
{
	uint32_t capacity;
	ActionLineID LineID;
	ActionStructure* actions;
};
static_assert(sizeof(ActionLine) == 0x10, "Size Check");

struct actions_entry							//动作实体类
{
	CategoryID iCategoryID;						//0x0；姿势
	SubCategoryID iSubCategoryID;				//0x4；动作类型
	int32_t EndFrame;
	int32_t LoopBackFrame;
	int32_t OptionFlag;
	int32_t Reserve1;
	int32_t Reserve2;
	int32_t Reserve3;
	int32_t Reserve4;
	int32_t LineCount;							//0x24；ACT指针集内指针数
	ActionLine* Line;							//0x28；ACT指针集
};
static_assert(sizeof(actions_entry) == 0x30, "Size check");

struct actions									//动作集
{
	char _0x0[0x20];							//0x0
	actions_entry* entry;						//0x20
	void* hold;									//0x28
};
static_assert(sizeof(actions) == 0x30, "Size check");
