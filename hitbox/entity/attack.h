#pragma once

enum class AttackType : int32_t		//攻击框类型
{
	normal = 0x0,					//普通攻击
	projectile = 0x1,				//飞行道具
	grasp = 0x2,					//投技
	force = 0x3,					//强制命中
	branch = 0x6,					//触发分支
	none = 0x9999
};

enum class AttackPosition : uint32_t
{
	normal = 0x0,
	IneffectiveBack = 0x1,
	IneffectiveFront = 0x2,
	IneffectiveAll = 0x3
};

#pragma pack(push, 2)
struct attack_collections
{
	AttackType types;
	char fill0[0xA4];
	AttackPosition position;
	char fill1[0x6C];
	uint32_t flagpadding : 6;
	uint32_t IneffectiveStand : 1;
	uint32_t IneffectiveCrouch : 1;
	uint32_t IneffectiveAir : 1;
	uint32_t IneffectiveNotGuard : 1;
	char fill2[0x162];
};
static_assert(sizeof(attack_collections) == 0x27E, "Size check");
#pragma pack(pop)

struct attacks
{
	char unknown[0x20];
	attack_collections* atccs;
};
