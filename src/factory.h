#pragma once
#include "config.h"
#include "./components/components.h"
#include "./shader.h"

class Factory {
    public:
        Factory(World world, Mesh& particleInstance, Mesh& tileInstance, Mesh& charQuad,
                std::unordered_map<std::string, Tile>& tiles,
                std::map<GLchar, Character>& characters);
        ~Factory();

        void make_particles();
        void make_tiles();
        void make_char();

    private:
        Mesh& particleInstance;
        Mesh& tileInstance;
        Mesh& charQuad;
        std::unordered_map<std::string, Tile>& tiles;
        unsigned int maxParticles;
        unsigned int SCR_WIDTH;
        unsigned int SCR_HEIGHT;
        
        std::map<GLchar, Character>& characters;
        
        Mesh make_particle_instance();
        unsigned int make_particle_transform_buffer(); 
        unsigned int make_particle_color_buffer();

        void make_char_quad(unsigned int shader);
        void make_tile_textures(unsigned int shader, Tile& tile, std::string imgSecondPath);
        unsigned int make_tile_instance();

};