#include "dominator.h"

bool dominator::window()
{
	view::compoment hb;
	hb.ins(u8"settings", [&] {
		control::trigger("p1", &configurs::dp1);
		control::trigger("p2", &configurs::dp2);

		configurs::GetIniConfig();

		control::combobox(u8"Language", configurs::languages, configurs::language);

		control::textfloatinput(configurs::alphaname[configurs::language + 1], &configurs::alpha);
		control::textfloatinput(configurs::thicknessname[configurs::language + 1], &configurs::thickness);

		for (int i = 0; i < configurs::distancedesc.size(); i++)
		{
			control::label(i, configurs::distancedesc[i].multititles[configurs::language + 1],
				&configurs::distancedesc[i].open, &configurs::distancedesc[i].activity, &configurs::distancedesc[i].color, "Configs");
		}
		});

	hb.ins(u8"hitboxes", [&] {
		reload_description_colors();
		for (int i = 0; i < descriptions.size(); i++) {
			description* desc = &descriptions[i];
			control::label(i + 1, desc->multititles[configurs::language + 1], &desc->open, &desc->activity, &desc->color);
		}
		});

	hb.ins(u8"Credits", [&] {
		ImGui::Text(u8"主程序：\n\tRedundantcy\n\tExMingYan");
		ImGui::Text(u8"翻译：\n\tHISTORYPETER\n\tAmedo\n\t白井");
		ImGui::Text(u8"开源项目：\n\tImGui\n\tMinHook");
		});

	view::decoration decor;
	decor.insert(ICON_FA_EXPAND, hb);
	decor.icon(configurs::icon_awesmoe);
	decor.logo(configurs::icon_brands, ICON_FA_GITHUB);
	decor.show("Hitbox");
	return true;
}
