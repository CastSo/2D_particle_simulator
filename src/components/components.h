#pragma once 
#include "../config.h"

struct Mesh {

    unsigned int VAO, VBO, EBO;
    unsigned int transformVBO;
    unsigned int colorVBO;
    unsigned int textureVBO;
    unsigned int shader;
};

struct Particle {

    unsigned int id;
    glm::vec4 color; 
    glm::vec4 secondColor;
    bool isDestroyer;
    std::vector<std::vector<int>> rules;
    std::vector<std::vector<int>> transitions;
    std::vector<std::vector<int>> transitions_p;
    std::vector<int> destroyables; 
};

struct Character {
    unsigned int textureBufr;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

struct Tile {
    unsigned int mainTextureBufr;
    unsigned int secondTextureBufr;
    unsigned int isSelected;
    unsigned int dimension;
    glm::vec3 color;
    std::string imgMainPath;
    std::string imgSecondPath;
};





struct World {
    unsigned int MAX_PARTICLES;

    unsigned int SCR_WIDTH;
    unsigned int SCR_HEIGHT;
    int YBOUND;
    int XBOUND;

    float POINT_SIZE;
    float TILE_SIZE;
};

