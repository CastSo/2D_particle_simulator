#include "./gui.h"


Gui::Gui(GLFWwindow *window, World& world, Material_Lookup& materialLook, std::vector<int>& guiLayout):
    window(window),
    world(world),
    materialLook(materialLook),
    guiLayout(guiLayout) {

}

Gui::~Gui() {

}

int Gui::detect_mouse() {
    float tileWidth =  world.TILE_SIZE / (float)world.SCR_WIDTH;   
    float tileHeight =  world.TILE_SIZE / (float)world.SCR_HEIGHT; 
    int yG = -1.0 + (2.0f * (1)) * tileHeight;

    int hover_particle = 0;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // normalizes to tilemap
    int yN = (int)(world.SCR_HEIGHT - std::floor(ypos))/world.TILE_SIZE;
    int xN = ((int)std::floor(xpos))/world.TILE_SIZE;

    for (int x = 0; x < (int)(world.SCR_WIDTH/world.TILE_SIZE)/2; x++)
    {
        if(guiLayout[x] == materialLook.EMPTY)
        {
            continue;
        }

        float xG = x;
        float prev_xG = x-1;
        
        if((yN <= yG && yN >= 0) && (xN <= xG && xN > prev_xG))
        {
            
            hover_particle = guiLayout[x];
            
            return hover_particle;
        }
    }

    return hover_particle;
}
