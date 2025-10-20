#pragma once

#include <string>
#include <cstdint>
#include <zip.h>
#include <GLFW/glfw3.h>

enum class PakFormat
{
    PAK,   // Original Quake/Quake 2 .pak format
    PKZIP, // ZIP-based formats (PK3, PK4, etc.)
    UNKNOWN
};

struct PakFileEntry
{
    std::string filename;
    uint32_t offset;
    uint32_t size;
    PakFormat format;
    zip_file_t *zipFile;
};

struct PCXImage
{
    int width;
    int height;
    GLuint textureID;
    std::string filename;
};
