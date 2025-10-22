#pragma once

#include <string>
#include <GLFW/glfw3.h>

struct Texture {
    int width;
    int height;
    GLuint textureID;
    std::string filename;
};
