#pragma once

#define PaddingOffset 0x238
#define ActionDataSize 0x286

constexpr size_t CalcFillSize(size_t nextOffset, size_t prevOffset, size_t fillsize, size_t prevsize = 8)
{
	if (prevOffset == 0)
		return (nextOffset - prevsize) / fillsize;
	return (nextOffset - prevOffset - prevsize) / fillsize;
}

enum class AttackType : int32_t		//攻击框类型
{
	normal = 0x0,					//普通攻击
	projectile = 0x1,				//飞行道具
	grasp = 0x2,					//投技
	force = 0x3,					//强制命中
	branch = 0x6,					//触发分支
	none = 0x9999
};

enum class GuardAttrID : uint32_t
{
    Upper = 0, Middle = 1, Lower = 2, NoGuard = 3
};

enum class HitBackDirID : uint32_t
{
	normal = 0x0,
	IneffectiveBack = 0x1,
	IneffectiveFront = 0x2,
	IneffectiveAll = 0x3
};

struct AttackSubData
{
    int32_t GaugeAddMine;
    int32_t GaugeAddTarget;
    int32_t KnockBackFrame;
    float KnockBaseDistance;
    int32_t HitStopFrameMine;
    int32_t HitStopFrameTarget;
    int32_t HeatGaugeAddMine;
    int32_t HeatGaugeAddTarget;
};

struct DamageActionSetting
{
    int32_t Stand;
    int32_t Crouch;
    int32_t Jump;
    int32_t CounterStand;
    int32_t CounterCrouch;
    int32_t CounterJump;
    int32_t GuardStand;
    int32_t GuardCrouch;
    int32_t Nokezori;
    int32_t Kuzure;
    int32_t Fukitobi;
    int32_t Down;
    int32_t DownDown;
    int32_t PunishStand;
    int32_t PunishCrouch;
    int32_t PunishAir;
    int32_t Padding0;
    int32_t Padding1;
    int32_t Padding2;
	int32_t Padding3;
	int32_t Padding4;
};

struct DamageReverseData
{
    int32_t AtkID;
    int32_t Condition;
};

struct PoisonDamageData
{
    int16_t Count;
    int16_t IntervalFrame;
    int16_t Damage;
    uint16_t DamageReset : 1;
    uint16_t GuardReset : 1;
    uint16_t AddCount : 1;
    uint16_t EnableGuard : 1;
};

struct HitOffsetData
{
    int32_t SettingX;
    int32_t SettingY;
    float x;
    float y;
};

struct HitQuakeCameraData
{
    int32_t PresetID;
    int32_t Frame;
    float x;
    float y;
    float z;
};

struct SoundSettingData
{
    float Volume;
    int32_t CommonID;
    int16_t UniqueID;
    int16_t GroupID;
    int32_t Priority;
    uint32_t Reverse0;
    uint32_t Reverse1;
    uint32_t Reverse2;
};

struct HitEffectData
{
    int16_t ListID;
    int16_t ActNo;
    int32_t CounterActNo;
    float AngleX;
    float AngleY;
    float AngleZ;
};

struct HitSlowData
{
    int32_t HitSlowFrame;
    int32_t HitSlowRate;
};

#pragma pack(push, 2)
struct AttackData
{
	AttackType types;
	int32_t SkillType;
	int32_t Damage;
	int32_t LowerDamage;
	int32_t CheapDamage;
	int32_t Stun;
	int32_t Guard;
	int32_t Score;
	AttackSubData hitData;
	AttackSubData guardData;
	AttackSubData counterData;
	AttackSubData justData;
	int32_t ComboRevise;
	GuardAttrID GuardAttr;
	HitBackDirID hitBackDir;
	DamageActionSetting damageAction;
	int32_t DownFrame;
	int32_t PursuitFrame;
	int32_t PursuitFrameAdd;
	int32_t HitNokeDir;
	int32_t GuardNokeDir;
	DamageReverseData damageReverse;
	uint32_t TargetFlag;
	uint64_t flagpadding : 6;
	uint64_t IneffectiveStand : 1;
	uint64_t IneffectiveCrouch : 1;
	uint64_t IneffectiveAir : 1;
	uint64_t IneffectiveNotPunish : 1;
	int32_t ActionBranchKey;
	int32_t DamageAttr;
	int32_t VitalRecovery;
	PoisonDamageData poisonDamage;
	int32_t AddComboCount;
	int32_t ComboDirectHit;
	int32_t JustHealValue;
	HitOffsetData hitOffset;
	uint64_t SetInvalidAttackID;
	uint64_t CheckInvalidAttackID;
	int32_t DamageActPri;
	HitQuakeCameraData hitQuakeCamera;
	SoundSettingData hitSound0;
	SoundSettingData hitSound1;
	SoundSettingData hitVoice;
	SoundSettingData guardSound;
	HitEffectData hitEffect0;
	HitEffectData hitEffect1;
	HitEffectData guardEffect;
	HitSlowData hitSlow;
	int32_t LineAttackType;
	uint8_t Padding[CalcFillSize(ActionDataSize, PaddingOffset, sizeof(uint8_t), 0)];
};
static_assert(sizeof(AttackData) == ActionDataSize, "Size check");
#pragma pack(pop)

struct attacks
{
	char unknown[0x20];
	AttackData* attackdatas;
};
