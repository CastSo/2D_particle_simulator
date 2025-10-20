#pragma once 
#include "../config.h"


struct Tile {
    unsigned int mainTextureBufr;
    unsigned int selectTextureBufr;
    unsigned int isSelected;
    unsigned int dimension;
    glm::vec3 color;
    std::string imgMainPath;
    std::string imgSelectPath;
};