#pragma once
#include "config.h"
#include "./components/particle.h"
#include "./components/tile.h"
#include "./components/mesh.h"
#include "./components/world.h"
#include "./shader.h"

class Factory {
    public:
        Factory(World world, Mesh& particleInstance, Mesh& tileInstance, std::unordered_map<std::string, Tile>& tiles);
        ~Factory();

        void make_particles();
        void make_tiles();
        

    private:
        Mesh& particleInstance;
        Mesh& tileInstance;
        std::unordered_map<std::string, Tile>& tiles;
        unsigned int maxParticles; 
        unsigned int make_particle_instance();
        unsigned int make_particle_transform_buffer(); 
        unsigned int make_particle_color_buffer();


        void make_tile_textures(unsigned int shader, Tile& tile, std::string imgSelectPath);
        unsigned int make_tile_instance();

};