#pragma once 
#include "../config.h"

struct Tile {
    unsigned int VAO;
    unsigned int shader;
    unsigned int texture;
    unsigned int dimension;
    glm::vec3 color;
    std::string imagePath;
     
};