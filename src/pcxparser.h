#pragma once

#include <optional>
#include <string>
#include "types.h"

class PCXParser
{
public:
    static auto loadPCX(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<PCXImage>;
};

