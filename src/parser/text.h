#pragma once

#include <optional>
#include <string>

#include "../types.h"

struct TextFile
{
    std::string contents;
};

class TextFileParser {
public: 
    static auto loadTextFile(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<TextFile>;
};
