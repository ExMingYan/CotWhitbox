#ifndef SEARCHER_H
#define SEARCHER_H

#include <cstdint>
#include <immintrin.h>
#include <vector>
#include <string>
#include <windows.h>
#include "singleton.hpp"

#if defined(_MSC_VER)
#define LOWESTBIT(x) ([&]() -> unsigned long { \
    unsigned long idx = 0; \
    return _BitScanForward(&idx, x) ? idx : 32; \
}())
#elif defined(__GNUC__) || defined(__clang__)
#define LOWESTBIT(x) (x == 0 ? 32 : __builtin_ctz(x))
#else
static_assert(false, "Unsupported compiler for LOWESTBIT macro");
#endif

enum class PatternType : int
{
	Normal,
	LowHalf,
	HighHalf,
	Pass
};

static uint8_t PatternMask[4] = { 0xFF, 0x0F, 0xF0, 0x00 };

class searcher : public singleton<searcher>
{
private:
	static void ParsePattern(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<PatternType>& mask);
    template<typename returned = uint64_t>
    static returned search(returned baseAddress, const size_t patternSize, const size_t firstValidByte, const uint8_t firstByte,
        const size_t searchEnd, const uint8_t* const bufPtr, const uint8_t* const bytesPtr, const PatternType* const maskPtr,
        const std::vector<uint8_t> maskArray)
    {
        if (patternSize >= 4 && searchEnd >= 32)
        {
            const __m256i firstByteVec = _mm256_set1_epi8(firstByte);
            const size_t simdEnd = searchEnd >= 31 ? searchEnd - 31 : 0;
            const size_t simdPatternEnd = (patternSize / 16) * 16;
            for (size_t i = 0; i <= simdEnd; i += 32)
            {
                const __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(bufPtr + i + firstValidByte));
                const __m256i cmp = _mm256_cmpeq_epi8(data, firstByteVec);
                uint32_t mask_result = _mm256_movemask_epi8(cmp);
                while (mask_result != 0)
                {
                    const size_t bit = LOWESTBIT(mask_result);
                    mask_result &= (mask_result - 1);
                    const size_t pos = i + bit;
                    if (pos > searchEnd)
                        break;
                    const uint8_t* const currentBufPtr = bufPtr + pos;
                    bool found = true;
                    if (patternSize >= 16)
                    {
                        size_t j = 0;
                        for (; j < simdPatternEnd && found; j += 16)
                        {
                            const __m128i bufData = _mm_loadu_si128(reinterpret_cast<const __m128i*>(currentBufPtr + j));
                            const __m128i patternData = _mm_loadu_si128(reinterpret_cast<const __m128i*>(bytesPtr + j));
                            const __m128i maskVec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(maskArray.data() + j));
                            const __m128i maskedBuf = _mm_and_si128(bufData, maskVec);
                            const __m128i maskedPattern = _mm_and_si128(patternData, maskVec);
                            const __m128i cmpResult = _mm_cmpeq_epi8(maskedBuf, maskedPattern);
                            uint16_t cmpMask = _mm_movemask_epi8(cmpResult);
                            uint16_t validMask = 0;
                            for (int k = 0; k < 16; ++k)
                            {
                                if (maskPtr[j + k] != PatternType::Pass)
                                    validMask |= (1 << k);
                            }
                            if ((cmpMask & validMask) != validMask)
                                found = false;
                        }

                        for (; j < patternSize && found; ++j)
                        {
                            if (maskPtr[j] == PatternType::Pass)
                                continue;
                            const uint8_t maskedBuf = currentBufPtr[j] & maskArray[j];
                            if (maskedBuf != bytesPtr[j])
                                found = false;
                        }
                    }
                    else
                    {
                        for (size_t j = 0; j < patternSize && found; ++j)
                        {
                            if (maskPtr[j] == PatternType::Pass)
                                continue;
                            const uint8_t maskedBuf = currentBufPtr[j] & maskArray[j];
                            if (maskedBuf != bytesPtr[j])
                                found = false;
                        }
                    }
                    if (found)
                        return baseAddress + pos;
                }
            }

            const size_t remainingStart = (simdEnd / 32) * 32;
            for (size_t i = remainingStart; i <= searchEnd; ++i)
            {
                if (bufPtr[i + firstValidByte] != firstByte)
                    continue;
                const uint8_t* const currentBufPtr = bufPtr + i;
                bool found = true;
                if (patternSize <= 8)
                {
                    for (size_t j = 0; j < patternSize; ++j)
                    {
                        if (maskPtr[j] == PatternType::Pass)
                            continue;
                        const uint8_t maskedBuf = currentBufPtr[j] & maskArray[j];
                        if (maskedBuf != bytesPtr[j])
                        {
                            found = false;
                            break;
                        }
                    }
                }
                else
                {
                    for (size_t j = 0; j < patternSize && found; ++j)
                    {
                        if (maskPtr[j] == PatternType::Pass)
                            continue;
                        const uint8_t maskedBuf = currentBufPtr[j] & maskArray[j];
                        if (maskedBuf != bytesPtr[j])
                        {
                            found = false;
                        }
                    }
                }
                if (found)
                    return baseAddress + i;
            }
        }
        else
        {
            for (size_t i = 0; i <= searchEnd; ++i)
            {
                if (bufPtr[i + firstValidByte] != firstByte)
                    continue;
                const uint8_t* const currentBufPtr = bufPtr + i;
                bool found = true;
                for (size_t j = 0; j < patternSize && found; ++j)
                {
                    if (maskPtr[j] == PatternType::Pass)
                        continue;
                    const uint8_t maskedBuf = currentBufPtr[j] & maskArray[j];
                    if (maskedBuf != bytesPtr[j])
                        found = false;
                }
                if (found)
                    return baseAddress + i;
            }
        }
        return 0;
    }
public:
	template<typename returned = uint64_t>
	static returned search(const char* moduleName, const char* section, const std::string &signature)
	{
        if (signature.size() == 0)
            return 0;
		auto hmodule = reinterpret_cast<unsigned __int64>(GetModuleHandleA(moduleName));
		auto dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(hmodule);
		auto nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(hmodule + dos_header->e_lfanew);
		PIMAGE_SECTION_HEADER section_header = IMAGE_FIRST_SECTION(nt_headers);

		for (int i = 0; i < nt_headers->FileHeader.NumberOfSections; i++) {
			char buffer[9]{};
			memcpy(buffer, section_header->Name, 8);
			if (_stricmp(buffer, section) == 0)
				break;
			section_header++;
		}

		std::vector<uint8_t> bytes;
		std::vector<PatternType> mask;
		ParsePattern(signature, bytes, mask);
		const size_t patternSize = bytes.size();
		if (patternSize == 0)
			return 0;

		size_t firstValidByte = 0;
		while (firstValidByte < patternSize && mask[firstValidByte] == PatternType::Pass)
			firstValidByte++;
        const uint8_t firstByte = bytes[firstValidByte];
        const size_t searchEnd = section_header->SizeOfRawData - patternSize;
		uintptr_t baseAddress = hmodule + section_header->VirtualAddress;
        const uint8_t* const bufPtr = (uint8_t*)(baseAddress);
        const uint8_t* const bytesPtr = bytes.data();
        const PatternType* const maskPtr = mask.data();

        std::vector<uint8_t> maskArray(patternSize);
        for (size_t i = 0; i < patternSize; ++i)
			maskArray[i] = PatternMask[static_cast<int>(maskPtr[i])];
        return search<returned>(baseAddress, patternSize, firstValidByte, firstByte,
			searchEnd, bufPtr, bytesPtr, maskPtr, maskArray);
	}

    template<typename returned = uint64_t>
    static returned search(const char* section, const std::string& signature)
    {
		return search<returned>(nullptr, section, signature);
    }

	template<typename parameter, typename returned = uint64_t>
	static returned transform(parameter address) {
		LARGE_INTEGER result{};
		if (address != 0) {
			result.QuadPart = (unsigned long long)((unsigned long long)address + 7);
			result.LowPart += *(unsigned __int32*)((unsigned long long)address + 3);
		}
		return (returned)result.QuadPart;
	}
};

bool SearchWStringInMemory(HANDLE hProcess, uintptr_t startAddress, size_t regionSize, const std::wstring& targetString);

#endif // SEARCHER_H
