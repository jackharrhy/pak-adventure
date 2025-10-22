#pragma once

#include <cstdint>
#include <vector>
#include "../types.h"

struct BinaryFile
{
    std::vector<uint8_t> data;
};

class BinaryFileParser {
public:
    static auto loadBinaryFile(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<BinaryFile>;
};
