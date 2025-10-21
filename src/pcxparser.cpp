#include "pcxparser.h"

#include <vector>
#include <fstream>
#include <optional>
#include <algorithm>
#include <cstdint>

constexpr uint8_t  PCX_MAGIC_NUMBER        = 0x0A;  // The first byte of a PCX file should always equal this magic number.
constexpr uint8_t  PCX_HEADER_SIZE         = 128;   // Size in bytes of the PCX file header
constexpr uint16_t PALETTE_SIZE_256        = 768;   // The size in bytes of the 256 color palette (appended to the end of the file)
constexpr uint8_t  PALETTE_SIZE_EGA        = 48;    // The size in bytes of the 16-color EGA palette
constexpr uint8_t  PALETTE_256_MARKER_BYTE = 0x0C;  // Byte marker that indicates the start of a 256 color palette, which immediately precedes the palette data will

auto readHeader(std::ifstream &file) -> std::optional<PCXHeader> {
    // TODO: We should be checking the magic number and validating it's correct

    PCXHeader header;
    file.read(reinterpret_cast<char *>(&header.magic_number), 1);
    file.read(reinterpret_cast<char *>(&header.version), 1);
    file.read(reinterpret_cast<char *>(&header.encoding), 1);
    file.read(reinterpret_cast<char *>(&header.bitsPerPixel), 1);
    file.read(reinterpret_cast<char *>(&header.xmin), 2);
    file.read(reinterpret_cast<char *>(&header.ymin), 2);
    file.read(reinterpret_cast<char *>(&header.xmax), 2);
    file.read(reinterpret_cast<char *>(&header.ymax), 2);
    file.read(reinterpret_cast<char *>(&header.hres), 2);
    file.read(reinterpret_cast<char *>(&header.vres), 2);
    file.read(reinterpret_cast<char *>(header.palette), PALETTE_SIZE_EGA);
    file.seekg(1, std::ios::cur); // reserved
    file.read(reinterpret_cast<char *>(&header.colorPlanes), 1);
    file.read(reinterpret_cast<char *>(&header.bytesPerLine), 2);

    return file ? std::optional(header) : std::nullopt;
}

// Check if a byte has an run marker in it used by run-length encoding (RLE).
// RLE will set the top two bits high to indicate a run marker.
auto static inline hasRunMarker(uint8_t byte) -> bool {
    constexpr uint8_t RUN_MARKER_BITMASK = 0xC0; // 1100 0000

    if ((byte & RUN_MARKER_BITMASK) == RUN_MARKER_BITMASK) {
        return true;
    }

    return false;
}

// Get the run count from a run-length encoded byte. This is done by reading the
// bottom six bits of a byte.
auto static inline getRunCount(uint8_t byte) -> uint8_t {
    constexpr uint8_t RUN_COUNT_BITMASK = 0x3F; // 0011 1111
	//
    return byte & RUN_COUNT_BITMASK;
}

// Decodes PCX image data encoded using run-length encoding (RLE).
auto decodeRLE(const std::vector<uint8_t> &raw, size_t size) -> std::vector<uint8_t> {
    std::vector<uint8_t> decoded(size);

    size_t src = 0;
    size_t dst = 0;

    while (dst < size && src < raw.size()) {
        uint8_t byte = raw[src++];

        if (hasRunMarker(byte)) { // If the top two bits are being used as a run marker...
	    uint8_t count = getRunCount(byte);  // The remaining 6 bits tell us the run count.
	    byte = raw[src++];
            std::fill_n(decoded.begin() + dst, std::min<size_t>(count, size - dst), byte);
            dst += count;
        }
        else {
            decoded[dst++] = byte;
        }
    }
    return decoded;
}

auto static inline getImageDimensions(const PCXHeader header) -> std::pair<int, int> {
    // PCX images don't use a standard concept of width and height but rather a bounding box or 'position',
    // so we need to convert it into normal width and height values.

    int width = header.xmax - header.xmin + 1;
    int height = header.ymax - header.ymin + 1;

    return std::make_pair(width, height);
}

auto PCXParser::loadPCX(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<PCXImage> {
    std::ifstream file(pakPath, std::ios::binary);

    if (!file.is_open()) {
        return std::nullopt;
    }

    file.seekg(entry.offset);
    auto header = readHeader(file);

    if (!header) {
        return std::nullopt;
    }

    auto [width, height] = getImageDimensions(*header);

    // Read the raw image data
    std::vector<uint8_t> raw(entry.size - PCX_HEADER_SIZE);
    file.seekg(entry.offset + PCX_HEADER_SIZE);
    file.read(reinterpret_cast<char *>(raw.data()), raw.size());

    // Decode RLE data
    auto data = decodeRLE(raw, width * height);

    // TODO: We're assuming that the PCX image has a 256 color palette, which is going to be true
    // the vast majority of the time but isn't guaranteed, and the code will likely choke on those edge
    // cases.

    // Read the palette data
    std::vector<uint8_t> palette(PALETTE_SIZE_256);
    file.seekg(entry.offset + entry.size - PALETTE_SIZE_256 - 1);
    uint8_t paletteMarker;
    file.read(reinterpret_cast<char *>(&paletteMarker), 1);

    if (paletteMarker == PALETTE_256_MARKER_BYTE) {
        file.read(reinterpret_cast<char *>(palette.data()), PALETTE_SIZE_256);
    }

    std::vector<uint8_t> rgba(width * height * 4);

    for (int i = 0; i < width * height; i++) {
        uint8_t colorIndex = data[i];
        rgba[i * 4 + 0] = palette[colorIndex * 3 + 0]; // R
        rgba[i * 4 + 1] = palette[colorIndex * 3 + 1]; // G
        rgba[i * 4 + 2] = palette[colorIndex * 3 + 2]; // B
        rgba[i * 4 + 3] = 255;                         // A
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return PCXImage{width, height, textureID, ""}; // Initialize filename field
}

