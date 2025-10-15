#pragma once 
#include "../config.h"

struct Particle {
    unsigned int id;
    unsigned int VAO;
    unsigned int shader;
    glm::vec3 color; 
};