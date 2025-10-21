#pragma once

#include <cstdint>
#include <string>

struct PakHeader {
    std::string signature;
    uint32_t dirOffset;
    uint32_t dirLength;
};

class PakParser {
public:
    static auto loadArchive(const std::string &path) -> std::optional<std::vector<PakFileEntry>>;
    static auto readData(const std::string &path, const PakFileEntry &entry) -> std::vector<uint8_t>;
};
