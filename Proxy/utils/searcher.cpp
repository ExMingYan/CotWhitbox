#include "searcher.h"
#include <cctype>

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

bool SearchWStringInMemory(HANDLE hProcess, uintptr_t startAddress, size_t regionSize, const std::wstring& targetString)
{
	std::vector<uint8_t> buffer(regionSize);
	SIZE_T bytesRead = 0;

	if (!ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(startAddress), buffer.data(), regionSize, &bytesRead))
	{
		return false;
	}

	const uint8_t* targetBytes = reinterpret_cast<const uint8_t*>(targetString.c_str());
	size_t targetSize = targetString.size() * sizeof(wchar_t);

	if (targetSize == 0 || targetSize > bytesRead)
	{
		return false;
	}

	const uint8_t* bufPtr = buffer.data();
	const size_t searchEnd = bytesRead - targetSize;

	if (targetSize >= 4 && searchEnd >= 32)
	{
		uint8_t firstByte = targetBytes[0];
		uint8_t secondByte = targetBytes[1];

		__m128i firstByteVec = _mm_set1_epi8(firstByte);
		__m128i secondByteVec = _mm_set1_epi8(secondByte);

		size_t simdEnd = (searchEnd >= 15) ? searchEnd - 15 : 0;

		for (size_t i = 0; i <= simdEnd; i += 16)
		{
			__m128i data1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(bufPtr + i));
			__m128i cmp1 = _mm_cmpeq_epi8(data1, firstByteVec);

			__m128i data2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(bufPtr + i + 1));
			__m128i cmp2 = _mm_cmpeq_epi8(data2, secondByteVec);

			__m128i combined = _mm_and_si128(cmp1, cmp2);
			uint16_t mask = _mm_movemask_epi8(combined);

			if (mask != 0)
			{
				for (int bit = 0; bit < 16; bit++)
				{
					if (mask & (1u << bit))
					{
						size_t pos = i + bit;
						if (pos > searchEnd) break;

						if (memcmp(bufPtr + pos, targetBytes, targetSize) == 0)
						{
							return true;
						}
					}
				}
			}
		}
		for (size_t i = simdEnd + 16; i <= searchEnd; ++i)
		{
			if (memcmp(bufPtr + i, targetBytes, targetSize) == 0)
			{
				return true;
			}
		}
	}
	else
	{
		for (size_t i = 0; i <= searchEnd; ++i)
		{
			if (memcmp(bufPtr + i, targetBytes, targetSize) == 0)
			{
				return true;
			}
		}
	}
	return false;
}
