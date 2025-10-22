/*
 * SPDX-FileCopyrightText: Copyright PAK adventure contributors
 * SPDX-License-Identifier: MIT
 */

// Basic parser for the PCX image format

#pragma once

#include <optional>
#include <string>
#include "../types.h"

/**
 * @enum
 * @brief Describes the version of a PCX image.
 */
enum class PCXVersion: uint8_t {
    /** Version 2.5 - early fixed-palette EGA variant. Unsupported. **/
    PCX_VERSION_2_5_FIXED_EGA  = 0x00,
    /** Version 2.8 - early EGA variant with palette information. Unsupported. **/
    PCX_VERSION_2_8_EGA        = 0x02,
    /** Version 2.8 - with no palette information. Unsupported. **/
    PCX_VERSION_2_8_NO_PALETTE = 0x03,
    /** Version 4.0 - Windows-specific extension. Unsupported. **/
    PCX_VERSION_WINDOWS        = 0x04,
    /** Version 3.0 - The standard format used by most PCX images. Supported **/
    PCX_VERSION_3              = 0x05
};

/**
 * @enum
 * @brief Describes the type of encoding used on a PCX image.
 */
enum class PCXEncodingType: uint8_t {
    /** No encoding. Unsupported. **/
    PCX_NO_ENCODING,
    /** Run-length encoding (RLE). Almost all PCX images use this. **/
    PCX_RLE_ENCODING
};

/**
 * @enum PCXBitsPerPixel
 * @brief Describes the number of bits per pixel per color plane in a PCX image.
 *
 * Combined with the @ref PCXHeader::colorPlanes field, this determines the total
 * color depth of the image (e.g., 8 bits × 3 planes = 24-bit RGB).
 */
enum class PCXBitsPerPixel: uint8_t {
    /** 1 bit per pixel — monochrome or planar EGA. Unsupported. */
    BPP_1  = 1,

    /** 2 bits per pixel — 4 colors (rare, early CGA use). Unsupported. */
    BPP_2  = 2,

    /** 4 bits per pixel — 16 colors (EGA or early VGA). Unsupported. */
    BPP_4  = 4,

    /** 8 bits per pixel — 256 colors or one component of 24/32-bit color. */
    BPP_8  = 8
};

// Pragmas to ensure the PCX header is packed/aligned correctly if we ever want to export PCX files
#pragma pack(push, 1)

/**
 * @enum
 * @brief Describes the header of a PCX file **/
struct PCXHeader {
    /** Will always be 0x0A in a valid PCX file **/
    uint8_t magic_number;
    PCXVersion version;
    PCXEncodingType encoding;
    PCXBitsPerPixel bpp;
    uint16_t xmin;
    uint16_t ymin;
    uint16_t xmax;
    uint16_t ymax;
    uint16_t hres;
    uint16_t vres;
    /** The EGA palette for 16-color PCX images **/
    uint8_t palette[48];
    uint8_t colorPlanes;
    uint16_t bytesPerLine;
};
#pragma pack(pop)

class PCXParser {
public:
    static auto loadPCX(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<Texture>;
};
