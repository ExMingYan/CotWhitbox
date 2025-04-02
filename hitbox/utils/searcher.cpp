#include "searcher.h"

void searcher::ParsePattern(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<PatternType>& mask)
{
	size_t i = 0;
	while (i < pattern.size())
	{
		if ((pattern[i] == '?' || pattern[i] == '*') && (pattern[i + 1] == '?' || pattern[i + 1] == '*' || pattern[i + 1] == ' '))
		{
			bytes.push_back(0);
			mask.push_back(PatternType::Pass);
			i += 2;
		}
		else if (std::isxdigit(pattern[i]) && i + 1 < pattern.size() && std::isxdigit(pattern[i + 1]))
		{
			uint8_t byte = static_cast<uint8_t>(std::stoi(pattern.substr(i, 2), nullptr, 16));
			bytes.push_back(byte);
			mask.push_back(PatternType::Normal); // 需要匹配
			i += 2;
		}
		else if ((pattern[i] == '?' || pattern[i] == '*') && std::isxdigit(pattern[i + 1]))
		{
			uint8_t byte = static_cast<uint8_t>(std::stoi(pattern.substr(i + 1, 1), nullptr, 16));
			bytes.push_back(byte);
			mask.push_back(PatternType::LowHalf); // 只需要匹配低半字节
			i += 2;
		}
		else if (std::isxdigit(pattern[i]) && (pattern[i + 1] == '?' || pattern[i + 1] == '*'))
		{
			uint8_t byte = static_cast<uint8_t>(std::stoi(pattern.substr(i, 1), nullptr, 16) << 4);
			bytes.push_back(byte);
			mask.push_back(PatternType::HighHalf); // 只需要匹配高半字节
			i += 2;
		}
		else
		{
			i++;
		}
	}
}
