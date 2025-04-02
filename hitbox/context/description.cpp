#include "description.h"
#include "utils/inirw.h"

std::vector<description> descriptions = {
	{{u8"攻击框", u8"Active Hitbox", u8"攻撃判定" },		false, false, {255, 0, 0},			ActionLineID::Attack,	AttackType::normal,},
	{{u8"投技框", u8"Throw Hitbox", u8"投げ間合い" },		false, false, {127, 191, 191},	ActionLineID::Attack,	AttackType::grasp,},
	{{u8"飞行道具攻击框", u8"Projectile HitBox", u8"飛び道具判定" },false, false, {127, 255, 127},	ActionLineID::Attack,	AttackType::projectile,},
	{{u8"触发分支框", u8"Active Branch Box", u8"分支判定" },	false, false, {95, 255, 175},		ActionLineID::Attack,	AttackType::branch,},
	{{u8"防御框", u8"Guard Box", u8"ガード判定" },		false, false, {63, 127, 63},		ActionLineID::Hit, CollisionTypes::guard,},
	{{u8"霸体框", u8"Armor Box", u8"アーマー判定" },		false, false, {64, 127, 127},		ActionLineID::Hit, CollisionTypes::invulnerability,},
	{{u8"当身框", u8"Counter Box", u8"当身判定" },		false, false, {0, 0, 255},		ActionLineID::Hit, CollisionTypes::parries,},
	{{u8"回避触发框", u8"Dodye Trigger Box", u8"回避判定" },	false, false, {255, 0, 139},		ActionLineID::Hit, CollisionTypes::avoid},
	{{u8"对投当身框", u8"Renaldo's Ball HurtBox", u8"ロナウドのボール判定" },	false, false, {139, 0, 255},		ActionLineID::Hit, CollisionTypes::parriesex,},
	{{u8"身位框", u8"Push Box", u8"存在判定" },		false, false, {255, 165, 0},		ActionLineID::Body, CollisionTypes::body,},
	{{u8"受击框", u8"Body HurtBox", u8"判定" },		false, false, {255, 255, 0},		ActionLineID::Hit, CollisionTypes::normal,},
	{{u8"被投框", u8"ThrOw HurtBox", u8"投げられ判定" },		false, false, {127, 255, 255},	ActionLineID::Hit, CollisionTypes::capture,},
	{{u8"倒地受击框", u8"Down Box", u8"倒地判定" },	false, false, {0, 255, 0},		ActionLineID::Hit, CollisionTypes::ground,},
	{{u8"反弹飞行道具框", u8"Reflector Box", u8"飛び道具反射判定" }, false, false, {255, 127, 255},	ActionLineID::Hit, CollisionTypes::reflects,},
	{{u8"抵消飞行道具框", u8"Projectile Collision Box", u8"飛び道具存在判定" }, false, false, {128, 128, 255},	ActionLineID::Hit, CollisionTypes::negative,},
};

void reload_description_colors() {
	auto colors = ini_read_colors(descriptions, "Colors");
	if (!colors.empty()) {
		const int n = static_cast<int>(std::min<std::size_t>(colors.size(), descriptions.size()));
		for (int i = 0; i < n; ++i) {
			const auto& c = colors[i];
			descriptions[i].color = ImColor(c[0], c[1], c[2]);
		}
	}
	else {
		std::vector<Rgb> defaults;
		defaults.reserve(descriptions.size());
		for (auto& d : descriptions) {
			const ImVec4& v = d.color.Value;
			defaults.push_back({
				static_cast<std::uint8_t>(v.x * 255.0f + 0.5f),
				static_cast<std::uint8_t>(v.y * 255.0f + 0.5f),
				static_cast<std::uint8_t>(v.z * 255.0f + 0.5f)
				});
		}
		ini_write_colors(defaults, descriptions, "Colors");
	}
}
