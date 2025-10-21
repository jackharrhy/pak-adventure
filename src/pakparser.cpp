#include <optional>
#include <vector>
#include <fstream>
#include "types.h"
#include "pakparser.h"

static auto readHeader(std::ifstream &file) -> std::optional<PakHeader> {
    char signature[4];
    uint32_t dirOffset, dirLength;

    file.read(signature, 4);
    file.read(reinterpret_cast<char *>(&dirOffset), 4);
    file.read(reinterpret_cast<char *>(&dirLength), 4);

    if (!file || std::string(signature, 4) != "PACK") {
        return std::nullopt;
    }
    return PakHeader{std::string(signature, 4), dirOffset, dirLength};
}

static auto readEntry(std::ifstream &file) -> PakFileEntry {
    char name[56];
    uint32_t offset, size;
    file.read(name, 56);
    file.read(reinterpret_cast<char *>(&offset), 4);
    file.read(reinterpret_cast<char *>(&size), 4);
    return {std::string(name), offset, size};
}

auto PakParser::loadArchive(const std::string &path) -> std::optional<std::vector<PakFileEntry>> {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }

    auto header = readHeader(file);

    if (!header) {
        return std::nullopt;
    }

    file.seekg(header->dirOffset);
    std::vector<PakFileEntry> entries(header->dirLength / 64);

    for (auto &entry : entries) {
        entry = readEntry(file);
        entry.format = PakFormat::PAK;
    }

    return entries;
}

auto PakParser::readData(const std::string &path, const PakFileEntry &entry) -> std::vector<uint8_t> {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        return {};
    }

    file.seekg(entry.offset);
    std::vector<uint8_t> data(entry.size);
    file.read(reinterpret_cast<char *>(data.data()), entry.size);

    return data;
}
