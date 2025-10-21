// Basic parser for the PCX image format

#pragma once

#include <optional>
#include <string>
#include "types.h"

enum class PCXVersion: uint8_t {
    PCX_VERSION_2_5_FIXED_EGA  = 0x00,
    PCX_VERSION_2_8_EGA        = 0x02,
    PCX_VERSION_2_8_NO_PALETTE = 0x03,
    PCX_VERSION_WINDOWS        = 0x04,
    PCX_VERSION_3              = 0x05  // Likely the only version worth supporting
};

enum class PCXEncodingType: uint8_t {
    PCX_NO_ENCODING,
    PCX_RLE_ENCODING // 99% of PCX images use RLE and is the only type worth supporting
};

// Pragmas to ensure the PCX header is packed/aligned correctly if we ever want to export PCX files
#pragma pack(push, 1)
struct PCXHeader {
    uint8_t magic_number;     // Will always be 0x0A in a valid PCX file
    PCXVersion version;
    PCXEncodingType encoding; // Type of encoding in use (run-length encoding, or none)
    uint8_t bitsPerPixel;
    uint16_t xmin;
    uint16_t ymin;
    uint16_t xmax;
    uint16_t ymax;
    uint16_t hres;
    uint16_t vres;
    uint8_t palette[48];      // The EGA pallete for 16-color images
    uint8_t colorPlanes;
    uint16_t bytesPerLine;
};
#pragma pack(pop)

struct PCXImage {
    int width;
    int height;
    GLuint textureID;
    std::string filename;
};


class PCXParser {
public:
    static auto loadPCX(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<PCXImage>;
};
