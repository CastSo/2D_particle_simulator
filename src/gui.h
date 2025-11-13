#pragma once
#include "config.h"
#include "./components/components.h"
#include "./shader.h"

class Gui {
    public:
        Gui(GLFWwindow *window, World& world, Material_Lookup& materialLook, std::vector<int>&  guiLayout);
        ~Gui();
        int detect_mouse();
    private:
        GLFWwindow *window;
        World& world;
        Material_Lookup& materialLook;
        std::vector<int>&  guiLayout;
};