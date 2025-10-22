#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "result.h"
#include "../types.h"

/**
 * @class PKZipParser
 * @brief Parser for .pak3 (pkzip) archives
 */
class PKZipParser {
public:
    static auto loadArchive(const std::string &path) -> PakResult;
    static auto readData(const std::string &path, const PakFileEntry &entry) -> std::vector<uint8_t>;
};
