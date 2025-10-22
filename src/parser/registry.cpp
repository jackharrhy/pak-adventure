#include <unordered_map>
#include <string>

#include "../types.h"
#include "registry.h"
#include "pak.h"
#include "pkzip.h"


std::unordered_map<PakFormat, FormatHandlers> ParserRegistry::handlers = {
    {PakFormat::PAK, {&PakParser::loadArchive, &PakParser::readData, "Quake/Quake 2 PAK Format"}},
    {PakFormat::PKZIP, {&PKZipParser::loadArchive, &PKZipParser::readData, "ZIP-based Format (PK3/PK4)"}}};

auto ParserRegistry::getFormatFromExtension(const std::string &extension) -> PakFormat {
    if (extension == ".pak") {
        return PakFormat::PAK;
    }
    if (extension == ".pk3" || extension == ".pk4") {
        return PakFormat::PKZIP;
    }

    return PakFormat::UNKNOWN;
}
