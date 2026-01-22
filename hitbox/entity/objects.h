#pragma once
#include <stdint.h>
#include "object.h"
#include "props.h"

#define Player1Offset 0x18
#define Player2Offset 0x20
#define ProjectileListOffset 0x38
#define SubObjOffset 0x75CA8

struct subobject
{
	uint32_t padding0[CalcFillSize(ProjectileListOffset, 0, sizeof(uint32_t), 0)];
	projectile_list* props_list;
};
static_assert(offsetof(subobject, props_list) == ProjectileListOffset, "subobject size error");

_declspec(align(8))
struct GameInstance
{
	uint64_t padding0[CalcFillSize(Player1Offset, 0, sizeof(uint64_t), 0)];
	object* p1;
	object* p2;
	uint64_t padding1[CalcFillSize(SubObjOffset, Player2Offset, sizeof(uint64_t))];
	subobject* subobjects;
};
static_assert(offsetof(GameInstance, subobjects) == SubObjOffset, "GameInstance size error");
