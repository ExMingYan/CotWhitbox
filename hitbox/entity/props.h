#pragma once
#include "action.h"
#include "object.h"

#define RemainingAttacksOffset 0x520
#define InvalidateOffset 0x52C

_declspec(align(8))
struct projectile
{
	int64_t padding0[CalcFillSize(ACTIONSOFFSET, 0, sizeof(int64_t))];
	actions* acts;
	int64_t padding1[CalcFillSize(ATTACKSOFFSET, ACTIONSOFFSET, sizeof(int64_t))];
	attacks* atks;
	int64_t padding2[CalcFillSize(ENEMYOFFSET, ATTACKSOFFSET, sizeof(int64_t))];
	object* enemy;
	object* owner;
	float x;
	float y;
	float z;
	int32_t padding3[CalcFillSize(XOFFOFFSET, ZOFFSET, sizeof(int32_t), sizeof(float))];
	float xoff;
	float yoff;
	float zoff;
	int32_t padding4[CalcFillSize(TOWARDOFFSET, ZOFFOFFSET, sizeof(int32_t), sizeof(float))];
	int32_t toward;
	int32_t padding5[CalcFillSize(ACTIONOFFSET, TOWARDOFFSET, sizeof(int32_t), sizeof(int32_t))];
	int32_t action;
	int32_t padding6[CalcFillSize(NOWOFFSET, ACTIONOFFSET, sizeof(int32_t), sizeof(int32_t))];
	int32_t now;
	int32_t padding7[CalcFillSize(RemainingAttacksOffset, NOWOFFSET, sizeof(int32_t), sizeof(int32_t))];
	int32_t RemainingAttacks;
	int32_t padding8[CalcFillSize(InvalidateOffset, RemainingAttacksOffset, sizeof(int32_t), sizeof(int32_t))];
	int32_t Invalidate;

	inline bool left() {
		return this->toward == 0;
	}

	inline CategoryID enemycategory() {
		return this->enemy->acts->entry[this->enemy->action].CategoryID;
	}

	inline SubCategoryID enemysubcategory() {
		return this->enemy->acts->entry[this->enemy->action].SubCategoryID;
	}

	inline bool positive() {
		if (!left())
			return this->x >= this->enemy->x;
		else
			return this->x <= this->enemy->x;
	}

	inline bool isInvalidate()
	{
		return ((this->Invalidate >> 8) & 0x1) != 0;
	}
};
static_assert(offsetof(projectile, Invalidate) == InvalidateOffset, "projectile size error");

struct projectile_list
{
	projectile_list* next;	//0x0
	void* hold;				//0x8
	projectile* props;		//0x10
};

struct range {
	float left;
	float top;
	float right;
	float bottom;
	float depth;

	range(object* obj, HitRect rect);
	range(projectile* obj, HitRect rect);
};
