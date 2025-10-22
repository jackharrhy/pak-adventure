#pragma once

#include <vector>
#include <expected>

#include "../types.h"

enum class PakError {
    signatureError,
    fileError,
};

using PakResult = std::expected<std::vector<PakFileEntry>, PakError>;

struct PakHeader; // forward declaration from pak.h
using PakHeaderResult = std::expected<PakHeader, PakError>;
