#include <string>
#include <optional>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include "../texture.h"
#include "../types.h"
#include "registry.h"
#include "stb.h"


auto STBImageParser::loadSTBImage(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<Texture> {
    auto readDataFunc = ParserRegistry::handlers[entry.format].readData;
    auto data = readDataFunc(pakPath, entry);

    if (data.empty())
        return std::nullopt;

    int width, height, channels;
    unsigned char *imageData = stbi_load_from_memory(data.data(), data.size(), &width, &height, &channels, STBI_rgb_alpha);
    if (!imageData)
        return std::nullopt;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(imageData);
    return Texture{width, height, textureID, ""};
}
