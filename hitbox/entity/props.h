#pragma once
#include "action.h"
#include "object.h"

_declspec(align(8))
struct projectile
{
	int64_t padding0[0x7];
	actions* acts;
	int64_t padding1[0x5];
	attacks* atcs;
	int64_t padding2[0x4];
	object* enemy;
	object* owner;
	float x;
	float y;
	float z;
	int32_t _0xAC[0xC];
	float xoff;					//0xDC
	float yoff;					//0xE0
	float zoff;					//0xE4
	int32_t _0xE8[0x1B];		//0xE8
	int32_t toward;
	int _0x158[6];				//0x158
	int action;					//0x170
	int _0x174[2];				//0x174
	int now;					//0x17C: 正在执行帧数
	char _0x180[0x3A0];			//0x180
	int RemainingAttacks;		//0x520；剩余攻击次数
	int _0x51C[2];				//0x524
	int Invalidate;				//0x52C；是否无效

	inline bool left() {
		return this->toward == 0;
	}

	inline CategoryID enemycategory() {
		return this->enemy->acts->entry[this->enemy->action].iCategoryID;
	}

	inline SubCategoryID enemysubcategory() {
		return this->enemy->acts->entry[this->enemy->action].iSubCategoryID;
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
static_assert(offsetof(projectile, Invalidate) == 0x52C, "projectile size error");

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
