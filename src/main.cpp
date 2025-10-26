#include "config.h"
#include "./factory.h"
#include "./think.h"

GLFWwindow* window;

int detect_mouse(int xN, int yN);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_mouse(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 640;
int YBOUND = 8;
int XBOUND = 1;

const float POINT_SIZE = 4.0f;
const float TILE_SIZE = 32.0f;
const unsigned int MAX_PARTICLES = ((SCR_WIDTH/ POINT_SIZE) * (SCR_HEIGHT/ POINT_SIZE));

int EMPTY = 0;
int WOOD = 1;
int SAND = 2;
int WATER = 3;
int GAS = 4;
int FIRE = 5;
//int MAP[(int)(SCR_HEIGHT/POINT_SIZE)][(int)(SCR_WIDTH/POINT_SIZE)];
std::vector<std::vector<int>> MAP;
int GUI_LAYOUT[(int)(SCR_WIDTH/TILE_SIZE)];
int ACTIVE_PARTICLE;
int HOVER_PARTICLE;

bool mouseDown;



//START: SETUP LAYOUT
void setup_map() {
    for (int y = 0; y <  ((int)(SCR_HEIGHT/POINT_SIZE)); y++)
    {
        MAP.push_back({});

        for (int x = 0; x < ((int)(SCR_WIDTH/POINT_SIZE)); x++)
        {

 
            MAP[y].push_back(EMPTY);

        }

    }
}


void setup_gui(std::unordered_map<std::string, Particle> particles) {
    auto p = particles.begin();
    for (int x = 0; x < ((int)(SCR_WIDTH/TILE_SIZE)); x++)
    {
        if (x % 2 == 1 && p != particles.end())
        {

            GUI_LAYOUT[x] = p->second.id;

            p++;
        
        }else
        {
            GUI_LAYOUT[x] = EMPTY;

    }
        
    }
}
//END: SETUP LAYOUT

//START: RENDER OBJECTS

void render_text(std::map<GLchar, Character> characters, Mesh& mesh, std::unordered_map<std::string, Particle> particles, 
            float x, float y, float scale, glm::vec3 color)
{
    std::string text;
    glUseProgram(mesh.shader);
    glUniform3f(glGetUniformLocation(mesh.shader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(mesh.VAO);

    for (auto particle : particles) {
        if(particle.second.id == HOVER_PARTICLE) {
            if(ACTIVE_PARTICLE == HOVER_PARTICLE) {
                text = "ACTIVE:";
            } 
            text += particle.first;
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
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
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

void render_gui(Mesh& mesh, Tile body, Tile edge, Tile frame, 
        std::unordered_map<std::string, Particle> particles, std::map<GLchar, Character> characters) {
    float tileWidth =  TILE_SIZE / (float)SCR_WIDTH;   
    float tileHeight =  TILE_SIZE / (float)SCR_HEIGHT; 
     
    float xpos = 0;
    float ypos = 0;
    float xN = -1.0f + (2.0f * (xpos + 0.5f) ) * tileWidth;
    float yN = -1.0f + (2.0f * (ypos + 0.5f) ) * tileHeight; 

    double xmouse, ymouse;
    glfwGetCursorPos(window, &xmouse, &ymouse);
    int ym = (int)(SCR_HEIGHT - std::floor(ymouse))/TILE_SIZE;
    int xm = ((int)std::floor(xmouse))/TILE_SIZE;
    
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
    for (int x = 1; x < (int)(SCR_WIDTH/TILE_SIZE); x++)
    {
        xN = -1.0f + (2.0f * (x + 0.5) ) * tileWidth;
        if (GUI_LAYOUT[x] != EMPTY)
        {

            //std::cout << GUI_LAYOUT[x] << std::endl;
            // glBindVertexArray(frame.VAO);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, frame.mainTextureBufr);

            if(detect_mouse(xm, ym) == GUI_LAYOUT[x])
            {
                HOVER_PARTICLE = GUI_LAYOUT[x];
                glUniform1i(frame.isSelected, true);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, frame.secondTextureBufr);
       
            }else if (ACTIVE_PARTICLE == GUI_LAYOUT[x])
            {
                glUniform1i(frame.isSelected, true);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, frame.secondTextureBufr);
            }
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
            // glBindVertexArray(body.VAO);
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


glm::mat4 map_particle(int x, int y)
{
    glm::mat4 transform = glm::mat4(1.0f);
    // //Normalize pixel to screen
    float cellWidth =  POINT_SIZE / (float)SCR_WIDTH;   
    float cellHeight =  POINT_SIZE / (float)SCR_HEIGHT; 

    
    float xCellPos = -1.0f + (2.0f * (x + 0.5f) ) * cellWidth;
    float yCellPos = -1.0f + (2.0f * (y + 0.5f) ) * cellHeight; 

    transform = glm::translate(transform, glm::vec3(xCellPos, yCellPos, 1.0f));


    return transform;

}



void render_particles(GLFWwindow *window, Mesh mesh, std::unordered_map<std::string, Particle> particles) {

    std::vector<glm::mat4> transforms;
    std::vector<glm::vec4> colors;

    glPointSize(POINT_SIZE);
    int particleCount = 0; 
    for (int y = YBOUND; y < ((int)(SCR_HEIGHT/POINT_SIZE))-YBOUND; y++)
    {
        for (int x = XBOUND; x < ((int)(SCR_WIDTH/POINT_SIZE))-XBOUND; x++)
        {
            if(MAP[y][x] == SAND) 
            {
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("SAND").color);
           }else if (MAP[y][x] == WATER) {
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("WATER").color);
            }else if (MAP[y][x] == WOOD) {
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("WOOD").color);
            } else if (MAP[y][x] == GAS) {
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("GAS").color);
            } else if (MAP[y][x] == FIRE) {
                int p = 1+(rand() % 2);
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
    } else if (transforms.size() == MAX_PARTICLES-1)
    {
        std::cout << "Reach MAX_PARTICLES" << std::endl;
        return;
    }
 
    glBindBuffer(GL_ARRAY_BUFFER, mesh.transformVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * MAX_PARTICLES,  NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4)* transforms.size(), transforms.data());

    
    glBindBuffer(GL_ARRAY_BUFFER, mesh.colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES,  NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)* colors.size(), colors.data());


    
    glBindVertexArray(mesh.VAO);
    glUseProgram(mesh.shader);
    
    glDrawArraysInstanced(GL_POINTS, 0, 1, transforms.size());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}








void add_particles(GLFWwindow *window, int id) {
    
    // grid setup
    // // mouse in screen coords
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // normalizes to NDC
    int yN = (SCR_HEIGHT - (int)std::floor(ypos))/POINT_SIZE;
    int xN= ((int)std::floor(xpos))/POINT_SIZE;

    int r = 2;
    
    if (id == WOOD)
    {   
        r = 6;
    }


    if(!((xN-r) < XBOUND || (xN+r) > ((int)SCR_WIDTH/POINT_SIZE)-XBOUND ||
            (yN-r) < YBOUND || (yN+r) > ((int)SCR_HEIGHT/POINT_SIZE)-YBOUND))
            {
                for(int i = 1; i < r; i++)
                {
                                    
                    int t1 = i / 12;
                    int t2 = 0;
                    int x = i;
                    int y = 0;   
                    while (x >= y)
                    {
                        MAP[yN+x][xN+y] = id;
                        MAP[yN-x][xN+y] = id;
                        MAP[yN+x][xN-y] = id;
                        MAP[yN-x][xN-y] = id;
                        MAP[yN+y][xN+x] = id;
                        MAP[yN+y][xN-x] = id;
                        MAP[yN-y][xN+x] = id;
                        MAP[yN-y][xN-x] = id;
                        
                        y = y + 1;
                        t1 = t1 + y;
                        t2 = t1 - x;
                        if(t2 >= 0)
                        {
                            t1 = t2;
                            x = x-1;
                        }
                    }
                }
        }

}


void setup_glfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particles Simulator", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        //return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    

     // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetMouseButtonCallback(window, process_mouse);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        //return -1;
    }

}


//START: PROCESS INPUT

int detect_mouse(int xN, int yN) {
    float tileWidth =  TILE_SIZE / (float)SCR_WIDTH;   
    float tileHeight =  TILE_SIZE / (float)SCR_HEIGHT; 
    int yG = -1.0 + (2.0f * (1)) * tileHeight;

    
    for (int x = 0; x < (int)(SCR_WIDTH/TILE_SIZE)/2; x++)
    {
        if(GUI_LAYOUT[x] == EMPTY)
        {
            continue;
        }

        float xG = x;
        float prev_xG = x-1;
        
        if((yN <= yG && yN >= 0) && (xN <= xG && xN > prev_xG))
        {
            
            
            
            return GUI_LAYOUT[x];
        }
    }
    return EMPTY;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){   
        glfwSetWindowShouldClose(window, true);
   }if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        setup_map();
    }
    
}



void process_mouse(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // normalizes to tilemap
    int yN = (int)(SCR_HEIGHT - std::floor(ypos))/TILE_SIZE;
    int xN = ((int)std::floor(xpos))/TILE_SIZE;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        mouseDown = true;
        
    } else {
        mouseDown = false;
    }

    int selected_coords = detect_mouse(xN, yN);
    if(detect_mouse(xN, yN) != EMPTY)
    {
        
        ACTIVE_PARTICLE = selected_coords;
    }
    
}



//END: PROCESS INPUT

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



int main(){
    
    srand(time(NULL));
    setup_glfw();

    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::unordered_map<std::string, Tile> tiles;
    std::unordered_map<std::string, Particle> particles;
    std::vector<std::vector<int>> ruleset = {
        {0,0,0,0}, {1,0,0,0}, {0,1,0,0}, {1,1,0,0},
        {0,0,1,0}, {1,0,1,0}, {0,1,1,0}, {1,1,1,0},
        {0,0,0,1}, {1,0,0,1}, {0,1,0,1}, {1,1,0,1},
        {0,0,1,1}, {1,0,1,1}, {0,1,1,1}, {1,1,1,1}
    };
    

    Particle sandParticle;
    sandParticle.id = SAND;
    sandParticle.color = {0.96f, 0.7f, 0.0f, 1.0f};
    sandParticle.isDestroyer = false;
    sandParticle.rules = {ruleset[1], ruleset[2], ruleset[3], ruleset[3], ruleset[11], ruleset[7],
                    ruleset[6], ruleset[9], ruleset[10], ruleset[5]};
     sandParticle.transitions = {ruleset[4], ruleset[8], ruleset[12], ruleset[14], ruleset[13], ruleset[12],
                    ruleset[12], ruleset[12], ruleset[12], ruleset[12]};
   

    Particle waterParticle;
    waterParticle.id = WATER;
    waterParticle.color = {0.35f, 0.7f, 1.0f, 1.0f};
    waterParticle.isDestroyer = false;
    waterParticle.rules = {ruleset[1], ruleset[2], ruleset[3], ruleset[3], ruleset[11], ruleset[7],
                    ruleset[6], ruleset[9], ruleset[10], ruleset[5], ruleset[4], ruleset[8]};
    waterParticle.transitions = {ruleset[4], ruleset[8], ruleset[12], ruleset[14], ruleset[13], ruleset[12],
                    ruleset[12], ruleset[12], ruleset[12], ruleset[12], ruleset[8], ruleset[4]};

    Particle woodParticle;
    woodParticle.id = WOOD;
    woodParticle.color = {0.60f, 0.24f, 0.06f, 1.0f};

    Particle gasParticle;
    gasParticle.id = GAS;
    gasParticle.color = {0.0f, 0.7f, 0.4f, 1.0f};
    gasParticle.isDestroyer = false;
    gasParticle.rules = {ruleset[1], ruleset[2], ruleset[8], ruleset[4], ruleset[12], ruleset[3],
                    ruleset[10], ruleset[5], ruleset[7], ruleset[14], ruleset[6], ruleset[9]};
    gasParticle.transitions = {ruleset[8], ruleset[4], ruleset[2], ruleset[1], ruleset[9], ruleset[9],
                    ruleset[9], ruleset[9], ruleset[14], ruleset[7], ruleset[9], ruleset[6]};
    gasParticle.transitions_p = {ruleset[2], ruleset[1], ruleset[8], ruleset[4], ruleset[6],
                    ruleset[6], ruleset[6], ruleset[6]}; 

    Particle fireParticle;
    fireParticle.id = FIRE;
    fireParticle.color = {1.0f, 0.3f, 0.0f, 1.0f};
    fireParticle.secondColor = {0.85f, 0.78f, 0.0f, 1.0f};
    fireParticle.isDestroyer = true;
    fireParticle.rules = {ruleset[1], ruleset[2], ruleset[8], ruleset[4], ruleset[12], ruleset[3],
                    ruleset[10], ruleset[5], ruleset[7], ruleset[14], ruleset[6], ruleset[9]};
    fireParticle.transitions = {ruleset[8], ruleset[4], ruleset[2], ruleset[1], ruleset[9], ruleset[9],
                    ruleset[9], ruleset[9], ruleset[14], ruleset[7], ruleset[9], ruleset[6]};
    fireParticle.transitions_p = {ruleset[2], ruleset[1], ruleset[8], ruleset[4], ruleset[6],
                    ruleset[6], ruleset[6], ruleset[6]}; 
   


   
    particles.insert({"FIRE", fireParticle});
    particles.insert({"WOOD", woodParticle});
    particles.insert({"GAS", gasParticle});
    particles.insert({"WATER", waterParticle});
    particles.insert({"SAND", sandParticle});
    

    std::string selectFramePath = "../images/select_frame.png";

    
    Tile particleButton;
    particleButton.imgMainPath = "../images/wood_frame.png";
    particleButton.dimension = (int)TILE_SIZE;
    particleButton.color = {0.7f, 0.6f, 0.0f};
    particleButton.imgSecondPath = selectFramePath;


    
    Tile logEdge;
    logEdge.imgMainPath  = "../images/wood_edge.png";
    logEdge.dimension = (int)TILE_SIZE;
    logEdge.color = {0.0f, 0.0f, 0.0f};
    logEdge.imgSecondPath = selectFramePath;


    Tile logBody;
    logBody.imgMainPath  = "../images/wood_body.png";
    logBody.dimension = (int)TILE_SIZE;
    logEdge.color = {0.0f, 0.0f, 0.0f};
    logEdge.imgSecondPath = selectFramePath;

    Tile label;



    tiles.insert({"BUTTON", particleButton});
    tiles.insert({"LOG_EDGE", logEdge});
    tiles.insert({"LOG_BODY", logBody});

    std::map<GLchar, Character> characters;
    World world;
    Mesh particleInstance;
    Mesh tileInstance;
    Mesh charQuad;
    world.MAX_PARTICLES = ((SCR_WIDTH/ POINT_SIZE) * (SCR_HEIGHT/ POINT_SIZE));
    world.SCR_HEIGHT = SCR_HEIGHT;
    world.SCR_WIDTH = SCR_WIDTH;
    world.XBOUND = XBOUND;
    world.YBOUND = YBOUND;
    world.POINT_SIZE = POINT_SIZE;
    world.TILE_SIZE = TILE_SIZE;
    Factory* factory = new Factory(world, particleInstance, tileInstance, charQuad, tiles, characters);
    Think* think = new Think(world, MAP, particles);

    //Sets up shaders and VAO of particles and tiles
    factory->make_particles();
    factory->make_tiles();
    factory-> make_char();

    std::cout << characters.size() << std::endl;
    setup_map();
    setup_gui(particles);

    ACTIVE_PARTICLE = SAND;
    int itrThink[4][2] = {{0,0}, {1,1}, {0,1}, {1,0}};
    int itr_i = 0;

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        if(mouseDown)
        {
            add_particles(window, ACTIVE_PARTICLE);
        }


        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        think->update_map();

        render_particles(window, particleInstance, particles);
        render_gui(tileInstance, tiles.at("LOG_BODY"), tiles.at("LOG_EDGE"), tiles.at("BUTTON"), particles, characters);
        render_text(characters, charQuad, particles , 0.0f, (SCR_HEIGHT-16), 0.3f,  glm::vec3(0.5, 0.8f, 0.2f));
        
 
        glfwSwapBuffers(window);
        glfwPollEvents();



    }


    delete factory;
    delete think;
    glfwTerminate();
    return 0;

}

