#include <optional>
#include <vector>
#include <fstream>
#include "../types.h"
#include "pak.h"
#include <spdlog/spdlog.h>

namespace {
    constexpr uint8_t PAK_FILENAME_LENGTH = 56;
    constexpr uint8_t PAK_SIGNATURE_LENGTH = 4;
    constexpr uint8_t PAK_DIR_ENTRY_SIZE = 64;

    /**
     * @brief Check if the signature is valid. PAK files should always have the signature `PACK` as the first 4 bytes.
     * @param signature The signature to check, a 4-byte array of characters that is not null-terminated.
     * @return True if the signature equals `PACK`, false otherwise.
     */
    inline auto hasValidSignature(const char (&signature)[PAK_SIGNATURE_LENGTH]) -> bool {
        return std::string(signature, PAK_SIGNATURE_LENGTH) == "PACK";
    }

    /**
     * @brief Read the header of a PAK archive.
     * @param file The file stream to read from.
     * @return A `PakHeader` struct containing the signature, directory offset, and directory length if successful, or `std::nullopt` if the file or signature is invalid.
     */
    auto readHeader(std::ifstream &file) -> PakHeaderResult {

        char signature[PAK_SIGNATURE_LENGTH];
        file.read(signature, PAK_SIGNATURE_LENGTH);

        // Ensure the read succeeded before using the buffer
        if (!file) {
            return std::unexpected(PakError::fileError);
        }

        // Validate the signature
        if (!hasValidSignature(signature)) {
            return std::unexpected(PakError::signatureError);
        }

        uint32_t dirOffset;
        uint32_t dirLength;

        file.read(reinterpret_cast<char *>(&dirOffset), 4);
        file.read(reinterpret_cast<char *>(&dirLength), 4);

        if (!file) {
            return std::unexpected(PakError::fileError); 
        }

        return PakHeader{std::string(signature, PAK_SIGNATURE_LENGTH), dirOffset, dirLength};
    }

    auto readEntry(std::ifstream &file) -> PakFileEntry {
        char name[PAK_FILENAME_LENGTH];
        file.read(name, PAK_FILENAME_LENGTH);
       
        uint32_t offset; 
        file.read(reinterpret_cast<char *>(&offset), 4);
        
        uint32_t size;
        file.read(reinterpret_cast<char *>(&size), 4);

        return PakFileEntry{std::string(name), offset, size};
    }
}

auto PakParser::loadArchive(const std::string &path) -> PakResult {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        return std::unexpected(PakError::fileError);
    }

    auto headerResult = readHeader(file);
    if (!headerResult.has_value()) {
        switch (headerResult.error()) {
            case PakError::signatureError:
                spdlog::error("Invalid signature in PAK archive");
                return std::unexpected(PakError::signatureError);
            case PakError::fileError:
                spdlog::error("Failed to read PAK archive header");
                return std::unexpected(PakError::fileError);
        }
    }

    const PakHeader &header = headerResult.value();

    file.seekg(header.dirOffset);
    std::vector<PakFileEntry> entries(header.dirLength / PAK_DIR_ENTRY_SIZE);

    for (auto &entry : entries) {
        entry = readEntry(file);
        entry.format = PakFormat::PAK;
    }

    return entries;
}

auto PakParser::readData(const std::string &path, const PakFileEntry &entry) -> std::vector<uint8_t> {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        return {};
    }

    file.seekg(entry.offset);
    std::vector<uint8_t> data(entry.size);
    file.read(reinterpret_cast<char *>(data.data()), entry.size);

    return data;
}
