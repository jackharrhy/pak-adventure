#include <cstdint>
#include <string>
#include <optional>
#include <vector>
#include <cstring>

#include "../types.h"
#include "pkzip.h"

auto PKZipParser::loadArchive(const std::string &path) -> PakResult {
        int error;
        zip_t *archive = zip_open(path.c_str(), 0, &error);
        if (!archive)
            return std::unexpected(PakError::fileError);

        std::vector<PakFileEntry> entries;
        zip_int64_t num_entries = zip_get_num_entries(archive, 0);

        for (zip_int64_t i = 0; i < num_entries; i++)
        {
            struct zip_stat st;
            if (zip_stat_index(archive, i, 0, &st) == -1)
                continue;

            // Skip directories
            if (st.name[strlen(st.name) - 1] == '/')
                continue;

            PakFileEntry entry;
            entry.filename = st.name;
            entry.size = st.size;
            entry.format = PakFormat::PKZIP;
            entry.zipFile = nullptr; // Will be set when file is opened
            entries.push_back(entry);
        }

        zip_close(archive);
        return entries;
}

auto PKZipParser::readData(const std::string &path, const PakFileEntry &entry) -> std::vector<uint8_t> {
    int error;
    zip_t *archive = zip_open(path.c_str(), 0, &error);
    if (!archive)
        return {};

    zip_file_t *file = zip_fopen(archive, entry.filename.c_str(), 0);
    if (!file)
    {
        zip_close(archive);
        return {};
    }

    std::vector<uint8_t> data(entry.size);
    zip_fread(file, data.data(), entry.size);
    zip_fclose(file);
    zip_close(archive);

    return data;
}
