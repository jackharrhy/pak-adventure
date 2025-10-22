#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <vector>

#include "../types.h"
#include "result.h"

using LoadArchiveFunc = PakResult (*)(const std::string &);
using ReadDataFunc = std::vector<uint8_t> (*)(const std::string &, const PakFileEntry &);

struct FormatHandlers {
    LoadArchiveFunc loadArchive;
    ReadDataFunc readData;
    std::string description;
};

class ParserRegistry {
public:
    static auto getFormatFromExtension(const std::string &extension) -> PakFormat;
    static std::unordered_map<PakFormat, FormatHandlers> handlers;
};
