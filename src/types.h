#pragma once

#include <string>
#include <cstdint>
#include <zip.h>
#include <GLFW/glfw3.h>

enum class PakFormat {
    PAK,   // Original Quake/Quake 2 .pak format
    PKZIP, // ZIP-based formats (PK3, PK4, etc.)
    UNKNOWN
};

/**
 * @brief Describes a file inside any type of PAK archive
 */
struct PakFileEntry {
    /** The internal filename given to a file inside a PAK archive **/
    std::string filename;
    uint32_t offset;
    uint32_t size;
    PakFormat format;
    zip_file_t *zipFile;
};
