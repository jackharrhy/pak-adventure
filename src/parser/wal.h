#pragma once

#include <cstdint>
#include "../types.h"

struct WALHeader {
    char name[32];
    uint32_t width;
    uint32_t height;
    uint32_t offset[4]; // mipmap offsets
    char animname[32];
    uint32_t flags;
    uint32_t contents;
    uint32_t value;
};

class WALParser {
public:
    static auto loadWAL(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<Texture>;
};

