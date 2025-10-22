#include <optional>
#include <string>

#include "../types.h"
#include "text.h"
#include "registry.h"

auto TextFileParser::loadTextFile(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<TextFile> {
    auto readDataFunc = ParserRegistry::handlers[entry.format].readData;
    auto data = readDataFunc(pakPath, entry);

    if (data.empty()) {
        return std::nullopt;
    }

    return TextFile{std::string(data.begin(), data.end())};
}

