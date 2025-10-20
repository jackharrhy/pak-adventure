#include "pcxparser.h"

#include <vector>
#include <fstream>
#include <optional>
#include <algorithm>
#include <cstdint>

struct PCXHeader
{
    uint8_t manufacturer;
    uint8_t version;
    uint8_t encoding;
    uint8_t bitsPerPixel;
    uint16_t xmin, ymin, xmax, ymax;
    uint16_t hres, vres;
    uint8_t palette[48];
    uint8_t colorPlanes;
    uint16_t bytesPerLine;
};

auto readHeader(std::ifstream &file) -> std::optional<PCXHeader>
{
    PCXHeader header;
    file.read(reinterpret_cast<char *>(&header.manufacturer), 1);
    file.read(reinterpret_cast<char *>(&header.version), 1);
    file.read(reinterpret_cast<char *>(&header.encoding), 1);
    file.read(reinterpret_cast<char *>(&header.bitsPerPixel), 1);
    file.read(reinterpret_cast<char *>(&header.xmin), 2);
    file.read(reinterpret_cast<char *>(&header.ymin), 2);
    file.read(reinterpret_cast<char *>(&header.xmax), 2);
    file.read(reinterpret_cast<char *>(&header.ymax), 2);
    file.read(reinterpret_cast<char *>(&header.hres), 2);
    file.read(reinterpret_cast<char *>(&header.vres), 2);
    file.read(reinterpret_cast<char *>(header.palette), 48);
    file.seekg(1, std::ios::cur); // reserved
    file.read(reinterpret_cast<char *>(&header.colorPlanes), 1);
    file.read(reinterpret_cast<char *>(&header.bytesPerLine), 2);

    return file ? std::optional(header) : std::nullopt;
}

auto decodeRLE(const std::vector<uint8_t> &raw, size_t size) -> std::vector<uint8_t>
{
    std::vector<uint8_t> decoded(size);
    size_t src = 0, dst = 0;

    while (dst < size && src < raw.size())
    {
        uint8_t byte = raw[src++];
        if ((byte & 0xC0) == 0xC0)
        {
            uint8_t count = byte & 0x3F;
            byte = raw[src++];
            std::fill_n(decoded.begin() + dst, std::min<size_t>(count, size - dst), byte);
            dst += count;
        }
        else
        {
            decoded[dst++] = byte;
        }
    }
    return decoded;
}

auto PCXParser::loadPCX(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<PCXImage>
{
    std::ifstream file(pakPath, std::ios::binary);
    if (!file.is_open())
        return std::nullopt;

    file.seekg(entry.offset);
    auto header = readHeader(file);
    if (!header)
        return std::nullopt;

    int width = header->xmax - header->xmin + 1;
    int height = header->ymax - header->ymin + 1;

    // Read the raw image data
    std::vector<uint8_t> raw(entry.size - 128);
    file.seekg(entry.offset + 128);
    file.read(reinterpret_cast<char *>(raw.data()), raw.size());

    // Decode RLE data
    auto data = decodeRLE(raw, width * height);

    // Read the palette data
    std::vector<uint8_t> palette(768);
    file.seekg(entry.offset + entry.size - 769);
    uint8_t paletteMarker;
    file.read(reinterpret_cast<char *>(&paletteMarker), 1);
    if (paletteMarker == 12)
    {
        file.read(reinterpret_cast<char *>(palette.data()), 768);
    }

    std::vector<uint8_t> rgba(width * height * 4);
    for (int i = 0; i < width * height; i++)
    {
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

    return PCXImage{width, height, textureID, ""}; // Initiailize filename field
}

