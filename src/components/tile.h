#pragma once 
#include "../config.h"

struct Tile {
    unsigned int mainTexture;
    unsigned int selectTexture;
    unsigned int isSelected;
    unsigned int dimension;
    glm::vec3 color;
    std::string imagePath;
     
};