#include <optional>
#include <string>


#include "binary.h"
#include "../types.h"
#include "registry.h"

auto BinaryFileParser::loadBinaryFile(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<BinaryFile> {
        auto readDataFunc = ParserRegistry::handlers[entry.format].readData;
        auto data = readDataFunc(pakPath, entry);

        if (data.empty()) {
            return std::nullopt;
        }

        return BinaryFile{data};
}
