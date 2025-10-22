#include <string>
#include "../types.h"
#include "../texture.h"

class STBImageParser {
public:
static auto loadSTBImage(const std::string &pakPath, const PakFileEntry &entry) -> std::optional<Texture>;
};
