#include <algorithm>
#include <cstdint>
#include <fstream>
#include <optional>
#include <vector>

#include <GLFW/glfw3.h>

#include "../types.h"
#include "../texture.h"
#include "wal.h"
#include "pak.h"
#include "pcx.h"

static std::optional<std::vector<uint8_t>> globalPalette;

static auto loadGlobalPalette(const std::string &pakPath, const std::vector<PakFileEntry> &entries) -> bool {
    // Find the colormap.pcx entry
    auto it = std::find_if(entries.begin(), entries.end(), [](const PakFileEntry &e) { return e.filename == "pics/colormap.pcx"; });

    if (it == entries.end()) {
        return false;
    }

    // Load the PCX file
    std::ifstream file(pakPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(it->offset);
    auto pcxImage = PCXParser::loadPCX(pakPath, *it);
    if (!pcxImage) {
        return false;
    }

    // Read back the texture data to get the palette
    std::vector<uint8_t> pixels(pcxImage->width * pcxImage->height * 4);
    glBindTexture(GL_TEXTURE_2D, pcxImage->textureID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Convert RGBA to RGB palette
    globalPalette = std::vector<uint8_t>(256 * 3);
    for (int i = 0; i < 256; i++) {
        globalPalette->at(i * 3 + 0) = pixels[i * 4 + 0];
        globalPalette->at(i * 3 + 1) = pixels[i * 4 + 1];
        globalPalette->at(i * 3 + 2) = pixels[i * 4 + 2];
    }

    // Clean up the temporary texture
    glDeleteTextures(1, &pcxImage->textureID);
    return true;
}

static auto readHeader(std::ifstream &file) -> std::optional<WALHeader> {
    WALHeader header;
    file.read(header.name, 32);
    file.read(reinterpret_cast<char *>(&header.width), 4);
    file.read(reinterpret_cast<char *>(&header.height), 4);
    file.read(reinterpret_cast<char *>(&header.offset), 16); // 4 mipmap offsets
    file.read(header.animname, 32);
    file.read(reinterpret_cast<char *>(&header.flags), 4);
    file.read(reinterpret_cast<char *>(&header.contents), 4);
    file.read(reinterpret_cast<char *>(&header.value), 4);

    return file ? std::optional(header) : std::nullopt;
}

auto WALParser::loadWAL(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<Texture> {
    // Load the global palette if we haven't already
    if (!globalPalette) {
        // We need to find all entries to locate the colormap
        auto entries = PakParser::loadArchive(pakPath);

        if (!entries || !loadGlobalPalette(pakPath, *entries)) {
            return std::nullopt;
        }
    }

    std::ifstream file(pakPath, std::ios::binary);
    if (!file.is_open()) {
        return std::nullopt;
    }

    file.seekg(entry.offset);
    auto header = readHeader(file);
    if (!header) {
        return std::nullopt;
    }

    // Read the main image data
    std::vector<uint8_t> data(header->width * header->height);
    file.seekg(entry.offset + header->offset[0]); // First mipmap level
    file.read(reinterpret_cast<char *>(data.data()), data.size());

    // Convert indexed color to RGBA using global palette
    std::vector<uint8_t> rgba(header->width * header->height * 4);
    for (int i = 0; i < header->width * header->height; i++) {
        uint8_t colorIndex = data[i];
        // Handle transparent pixels (index 255 is transparent)
        if (colorIndex == 255) {
            rgba[i * 4 + 0] = 0; // R
            rgba[i * 4 + 1] = 0; // G
            rgba[i * 4 + 2] = 0; // B
            rgba[i * 4 + 3] = 0; // A (transparent)
        }
        else {
            rgba[i * 4 + 0] = globalPalette->at(colorIndex * 3 + 0); // R
            rgba[i * 4 + 1] = globalPalette->at(colorIndex * 3 + 1); // G
            rgba[i * 4 + 2] = globalPalette->at(colorIndex * 3 + 2); // B
            rgba[i * 4 + 3] = 255;                                   // A (opaque)
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header->width, header->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return Texture{static_cast<int>(header->width), static_cast<int>(header->height), textureID, ""};
}
