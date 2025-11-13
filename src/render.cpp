#include "render.h"


Render::Render(GLFWwindow *window, std::vector<std::vector<int>>& MAP, std::vector<int>& guiLayout, World& world, Material_Lookup& materialLook, Mesh& particleInstance, 
                Mesh& tileInstance, Mesh& charQuad,
                std::unordered_map<std::string, Tile>& tiles,
                std::unordered_map<std::string, Particle>& particles,
                std::map<GLchar, Character>& characters):
                window(window),
                world(world),
                MAP(MAP),
                guiLayout(guiLayout),
                materialLook(materialLook),
                particleInstance(particleInstance),
                tileInstance(tileInstance),
                charQuad(charQuad),
                tiles(tiles),
                particles(particles),
                characters(characters){
                gui = new Gui(window, world, materialLook, guiLayout);

                }

Render::~Render() {
    delete gui;
}


void Render::update() {
    render_particles();
    render_gui(tileInstance, tiles.at("LOG_BODY"), tiles.at("LOG_EDGE"), tiles.at("BUTTON"));
    render_text( 0.0f, (world.SCR_HEIGHT-16), 0.3f,  glm::vec3(0.5, 0.8f, 0.2f));
}

void Render::render_particles() {

    std::vector<glm::mat4> transforms;
    std::vector<glm::vec4> colors;

    glPointSize(world.POINT_SIZE);
    int particleCount = 0; 
    int p;
    for (int y = world.YBOUND; y < ((int)(world.SCR_HEIGHT/world.POINT_SIZE))-world.YBOUND; y++)
    {
        for (int x = world.XBOUND; x < ((int)(world.SCR_WIDTH/world.POINT_SIZE))-world.XBOUND; x++)
        {
            if(MAP[y][x] == materialLook.SAND) 
            {
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("SAND").color);
           }else if (MAP[y][x] == materialLook.WATER) {
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("WATER").color);
            }else if (MAP[y][x] == materialLook.WOOD) {
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("WOOD").color);
                
            } else if (MAP[y][x] == materialLook.GAS) {
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("GAS").color);
            } else if (MAP[y][x] == materialLook.FIRE) {
                p = 1+(rand() % 2);
                transforms.push_back(map_particle(x, y));
                if(p == 1)
                {
                    colors.push_back(particles.at("FIRE").secondColor);
                }else {
                    colors.push_back(particles.at("FIRE").color);
                }
            }
    }
}

    if(transforms.size() == 0 )
    {
        return;
    } else if (transforms.size() == world.MAX_PARTICLES-1)
    {
        std::cout << "Reach MAX_PARTICLES" << std::endl;
        return;
    }
 
    glBindBuffer(GL_ARRAY_BUFFER, particleInstance.transformVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * world.MAX_PARTICLES,  NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4)* transforms.size(), transforms.data());

    
    glBindBuffer(GL_ARRAY_BUFFER, particleInstance.colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * world.MAX_PARTICLES,  NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)* colors.size(), colors.data());


    
    glBindVertexArray(particleInstance.VAO);
    glUseProgram(particleInstance.shader);
    
    glDrawArraysInstanced(GL_POINTS, 0, 1, transforms.size());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::mat4 Render::map_particle(int x, int y)
{
    glm::mat4 transform = glm::mat4(1.0f);
    // //Normalize pixel to screen
    float cellWidth =  world.POINT_SIZE / (float)world.SCR_WIDTH;   
    float cellHeight =  world.POINT_SIZE / (float)world.SCR_HEIGHT; 

    
    float xCellPos = -1.0f + (2.0f * (x + 0.5f) ) * cellWidth;
    float yCellPos = -1.0f + (2.0f * (y + 0.5f) ) * cellHeight; 

    transform = glm::translate(transform, glm::vec3(xCellPos, yCellPos, 1.0f));


    return transform;

}

void Render::render_gui(Mesh& mesh, Tile body, Tile edge, Tile frame) {
    float tileWidth =  world.TILE_SIZE / (float)world.SCR_WIDTH;   
    float tileHeight =  world.TILE_SIZE / (float)world.SCR_HEIGHT; 
     
    float xpos = 0;
    float ypos = 0;
    float xN = -1.0f + (2.0f * (xpos + 0.5f) ) * tileWidth;
    float yN = -1.0f + (2.0f * (ypos + 0.5f) ) * tileHeight; 

    double xmouse, ymouse;
    glfwGetCursorPos(window, &xmouse, &ymouse);
    int ym = (int)(world.SCR_HEIGHT - std::floor(ymouse))/world.TILE_SIZE;
    int xm = ((int)std::floor(xmouse))/world.TILE_SIZE;
    
    glBindVertexArray(mesh.VAO);
    glUseProgram(mesh.shader);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
    transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, edge.mainTextureBufr);

    glUniform1i(frame.isSelected, false);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    //get matrix's uniform location and set matrix
    
    unsigned int transformLoc = glGetUniformLocation(mesh.shader, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    unsigned int bgColorLoc = glGetUniformLocation(mesh.shader, "backgroundColor");
    glUniform4f(bgColorLoc, edge.color.x, edge.color.y, edge.color.z, 0.0f); 


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    auto p = particles.begin();
    for (int x = 1; x < (int)(world.SCR_WIDTH/world.TILE_SIZE); x++)
    {
        xN = -1.0f + (2.0f * (x + 0.5) ) * tileWidth;
        if (guiLayout[x] != materialLook.EMPTY)
        {

            //std::cout << GUI_LAYOUT[x] << std::endl;
            // glBindVertexArray(frame.VAO);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frame.mainTextureBufr);

            if(gui->detect_mouse() == guiLayout[x])
            {
                // HOVER_PARTICLE = GUI_LAYOUT[x];
                glUniform1i(frame.isSelected, true);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, frame.secondTextureBufr);
       
             }//else if (ACTIVE_PARTICLE == GUI_LAYOUT[x])
            // {
            //     // HOVER_PARTICLE = ACTIVE_PARTICLE;
            //     glUniform1i(frame.isSelected, true);
            //     glActiveTexture(GL_TEXTURE1);
            //     glBindTexture(GL_TEXTURE_2D, frame.secondTextureBufr);
            // }
            else {
                glUniform1i(frame.isSelected, false);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, 0);
            }


            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
            transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));

            //get matrix's uniform location and set matrix
            unsigned int transformLoc = glGetUniformLocation(mesh.shader, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));        
            

            unsigned int bgColorLoc = glGetUniformLocation(mesh.shader, "backgroundColor");
            glUniform4f(bgColorLoc, p->second.color.x, p->second.color.y, p->second.color.z, 1.0f); 

            p++;
        
        }
        else {

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, body.mainTextureBufr);

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
            transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));

            //get matrix's uniform location and set matrix
            glUseProgram(mesh.shader);
            unsigned int transformLoc = glGetUniformLocation(mesh.shader, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        }
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Resets texture to just main
        glUniform1i(frame.isSelected, false);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        
    }
    
}


void Render::render_text(float x, float y, float scale, glm::vec3 color)
{
    std::string text;
    glUseProgram(charQuad.shader);
    glUniform3f(glGetUniformLocation(charQuad.shader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(charQuad.VAO);

    for (auto particle : particles) {
        if(particle.second.id == gui->detect_mouse()) {
            text += particle.first;
            break;
        } else if (gui->detect_mouse() == 0){
            break;
        }
    }
     // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = characters[*c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureBufr);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, charQuad.VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

