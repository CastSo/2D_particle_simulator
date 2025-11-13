#pragma once
#include "config.h"
#include "./components/components.h"
#include "./shader.h"
#include "./gui.h"

class Render {
    public:
        Render(GLFWwindow *window, std::vector<std::vector<int>>& MAP, std::vector<int>& guiLayout, World& world, Material_Lookup& materialLook, Mesh& particleInstance, 
                Mesh& tileInstance, Mesh& charQuad,
                std::unordered_map<std::string, Tile>& tiles,
                std::unordered_map<std::string, Particle>& particles,
                std::map<GLchar, Character>& characters);
        ~Render();
        void update();

    private:
        GLFWwindow *window;
        World& world;
        std::vector<std::vector<int>>& MAP;
        std::vector<int>& guiLayout;
        Material_Lookup& materialLook;
        Mesh& particleInstance;
        Mesh& tileInstance;
        Mesh& charQuad;
        std::unordered_map<std::string, Tile>& tiles;
        std::unordered_map<std::string, Particle>& particles;
        std::map<GLchar, Character>& characters;

        Gui* gui;

        void render_gui(Mesh& mesh, Tile body, Tile edge, Tile frame);
        void render_text(float x, float y, float scale, glm::vec3 color);

        glm::mat4 map_particle(int x, int y);
        void render_particles();
        
        void add_particles(int id);
};