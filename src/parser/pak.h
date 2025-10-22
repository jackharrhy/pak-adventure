#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "result.h"

/**
 * @brief Describes the header of a PAK archive 
 */
struct PakHeader {
    /** The first 4-bytes of a valid PAK archive should always be `PACK` (not null-terminated) **/
    std::string signature;
    uint32_t dirOffset;
    uint32_t dirLength;
};

class PakParser {
public:
    static auto loadArchive(const std::string &path) -> PakResult;
    static auto readData(const std::string &path, const PakFileEntry &entry) -> std::vector<uint8_t>;
};
