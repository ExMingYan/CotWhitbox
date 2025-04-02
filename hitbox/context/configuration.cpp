#include "configuration.h"

namespace configurs {
	ImFont* mainfont = nullptr;
	ImFont* icon_awesmoe = nullptr;
	ImFont* icon_brands = nullptr;
	bool dp1 = false;
	bool dp2 = false;
	bool passineffective = false;
	float alpha = 0.25f;
	float thickness = 1.0f;
	int language = -1;
	std::vector<const char*> passname = { u8"不绘制无效框", u8"Do not draw ineffective boxes", u8"無効なボックスを描画しない" };
	std::vector<const char*> alphaname = { u8"透明度", u8"Transparency", u8"透明度" };
	std::vector<const char*> thicknessname = { u8"粗细", u8"Line Thickness", u8"線幅" };
	std::vector<descript> distancedesc = {
		{ { u8"轻拳识别距离", u8"Light Punch Distance", u8"A判定距离" }, false, false, ImColor(159, 255, 95) },
		{ { u8"轻脚识别距离", u8"Light Kick Distance", u8"B判定距离" }, false, false, ImColor(95, 255, 95) },
		{ { u8"重拳识别距离", u8"Light Punch Distance", u8"C判定距离" }, false, false, ImColor(159, 255, 159) },
		{ { u8"重脚识别距离", u8"Light Kick Distance", u8"D判定距离" }, false, false, ImColor(63, 127, 255) }
	};
	std::vector<const char*> languages = { u8"中文", u8"English", u8"Japanese" };

	void GetIniConfig()
	{
		if (!ini_read_int("Language", language))
			ini_write_int("Language", language);
		if (!ini_read_float(alphaname[language + 1], alpha))
			ini_write_float(alphaname[language + 1], alpha);
		if (!ini_read_float(thicknessname[language + 1], thickness))
			ini_write_float(thicknessname[language + 1], thickness);
		std::vector<Rgb> rgb = ini_read_colors(distancedesc);
		if (rgb.empty())
		{
			for (size_t i = 0; i < distancedesc.size(); i++)
			{
				rgb.push_back({
					static_cast<uint8_t>(std::round(distancedesc[i].color.Value.x * 255.0f)),
					static_cast<uint8_t>(std::round(distancedesc[i].color.Value.y * 255.0f)),
					static_cast<uint8_t>(std::round(distancedesc[i].color.Value.z * 255.0f)),
				});
			}
			ini_write_colors(rgb, distancedesc);
		}
		else
		{
			for (size_t i = 0; i < rgb.size(); i++)
			{
				distancedesc[i].color = ImColor(rgb[i][0], rgb[i][1], rgb[i][2]);
			}
		}
	}
}
