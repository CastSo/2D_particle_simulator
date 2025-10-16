#include "config.h"
#include "./components/particle.h"
#include "./components/tile.h"

GLFWwindow* window;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_mouse(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 640;
int YBOUND = 8;
int XBOUND = 8;

const float POINT_SIZE = 4.0f;
const float TILE_SIZE = 32.0f;
const unsigned int MAX_PARTICLES = (SCR_WIDTH * SCR_HEIGHT) / 2;

int EMPTY = 0;
int WALL = 1;
int SAND = 2;
int WATER = 3;
int MAP[(int)(SCR_HEIGHT/POINT_SIZE)][(int)(SCR_WIDTH/POINT_SIZE)];
int GUI_LAYOUT[(int)(SCR_WIDTH/TILE_SIZE)];
int ACTIVE_PARTICLE;



bool mouseDown;



//START: SETUP LAYOUT
void setup_map() {
    for (int y = 0; y <  ((int)(SCR_HEIGHT/POINT_SIZE)); y++)
    {

        for (int x = 0; x < ((int)(SCR_WIDTH/POINT_SIZE)); x++)
        {

 
            MAP[y][x] = EMPTY;

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
std::vector<glm::vec2> map_particle(int x, int y, std::vector<glm::vec2> translations)
{
    // //Normalize pixel to screen
    float cellWidth =  POINT_SIZE / (float)SCR_WIDTH;   
    float cellHeight =  POINT_SIZE / (float)SCR_HEIGHT; 

    
    float xCellPos = -1.0f + (2.0f * (x + 0.5f) ) * cellWidth;
    float yCellPos = -1.0f + (2.0f * (y + 0.5f) ) * cellHeight; 


    translations.push_back(glm::vec2{xCellPos, yCellPos});

    return translations;
}

void render_map(GLFWwindow *window, std::unordered_map<std::string, Particle> particles, unsigned int VAO, 
                unsigned int shader, unsigned int transformVBO, unsigned int colorVBO) {
    glBindVertexArray(VAO);
    glUseProgram(shader);
    std::vector<glm::vec2> translations;
    std::vector<glm::vec4> colors;

    int particleCount = 0; 
    for (int y = YBOUND; y < ((int)(SCR_HEIGHT/POINT_SIZE))-YBOUND; y++)
    {
        for (int x = XBOUND; x < ((int)(SCR_WIDTH/POINT_SIZE))-XBOUND; x++)
        {
            if(MAP[y][x] == SAND) 
            {
                translations = map_particle(x, y, translations);
                colors.push_back(particles.at("SAND").color);
           }else if (MAP[y][x] == WATER) {
                translations = map_particle(x, y, translations);
                colors.push_back(particles.at("WATER").color);
            }else if (MAP[y][x] == WALL) {
                translations = map_particle(x, y, translations);
                colors.push_back(particles.at("WALL").color);
            }
    }
}
 
    glBindBuffer(GL_ARRAY_BUFFER, transformVBO);
    //set to dynamic 
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * translations.size(),  translations.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, transformVBO); 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, transformVBO);
    glVertexAttribDivisor(1, 1); 

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colors.size(),  colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribDivisor(2, 1); 

    glPointSize(POINT_SIZE);

    glDrawArraysInstanced(GL_POINTS, 0, 1, (GLsizei)translations.size());
}


void render_gui(Tile body, Tile edge, Tile frame, std::unordered_map<std::string, Particle> particles) {
    float tileWidth =  TILE_SIZE / (float)SCR_WIDTH;   
    float tileHeight =  TILE_SIZE / (float)SCR_HEIGHT; 
     
    float xpos = 0;
    float ypos = 0;
    float xN = -1.0f + (2.0f * (xpos + 0.5f) ) * tileWidth;
    float yN = -1.0f + (2.0f * (ypos + 0.5f) ) * tileHeight; 

    
    glBindVertexArray(edge.VAO);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
    transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));

    glBindTexture(GL_TEXTURE_2D, edge.texture);

    //get matrix's uniform location and set matrix
    glUseProgram(frame.shader);
    unsigned int transformLoc = glGetUniformLocation(frame.shader, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    unsigned int bgColorLoc = glGetUniformLocation(frame.shader, "backgroundColor");
    glUniform4f(bgColorLoc, edge.color.x, edge.color.y, edge.color.z, 0.0f); 



    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    auto p = particles.begin();
    for (int x = 1; x < (int)(SCR_WIDTH/TILE_SIZE); x++)
    {
        xN = -1.0f + (2.0f * (x + 0.5) ) * tileWidth;
        if (GUI_LAYOUT[x] != EMPTY)
        {
            //std::cout << GUI_LAYOUT[x] << std::endl;
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(frame.VAO);
            glBindTexture(GL_TEXTURE_2D, frame.texture);

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
            transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));

            //get matrix's uniform location and set matrix
            glUseProgram(frame.shader);
            unsigned int transformLoc = glGetUniformLocation(frame.shader, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));        
            

            unsigned int bgColorLoc = glGetUniformLocation(frame.shader, "backgroundColor");
            glUniform4f(bgColorLoc, p->second.color.x, p->second.color.y, p->second.color.z, 1.0f); 

           
            p++;
        
        }
        else {
            glBindVertexArray(body.VAO);
            glBindTexture(GL_TEXTURE_2D, body.texture);


            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
            transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));

            //get matrix's uniform location and set matrix
            glUseProgram(body.shader);
            unsigned int transformLoc = glGetUniformLocation(body.shader, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        }
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        
    }
    
}


//END:RENDER OBJECTS

//START: MAKE MESHES
unsigned int make_particle_mesh() {
    unsigned int VBO, VAO, EBO;
    std::vector<float> vertices = {
        0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    return VAO;
}

void make_tile_mesh(Tile& frame) {
    std::vector<float> vertices = {
         1.0f,  1.0f,  0.0f,   1.0f,  1.0f,  
         1.0f, -1.0f,  0.0f,   1.0f,  0.0f,
        -1.0f, -1.0f,  0.0f,   0.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,   0.0f,  1.0f
    };

    std::vector<unsigned int> indices = {
        3, 1, 2,
        3, 0, 1
    };

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); 
    glEnableVertexAttribArray(1);

    unsigned int tileTexture;

    glGenTextures(1, &tileTexture);
    glBindTexture(GL_TEXTURE_2D, tileTexture); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    unsigned char *data = stbi_load(frame.imagePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    
    frame.VAO = VAO;
    frame.texture = tileTexture;
}
//END: MAKE MESHES

void add_particles(GLFWwindow *window, int id) {
    // grid setup
    // // mouse in screen coords
    // double xpos, ypos;
    // glfwGetCursorPos(window, &xpos, &ypos);

    // // normalizes to NDC
    // int yNormal = (SCR_HEIGHT - (int)std::floor(ypos))/POINT_SIZE;
    // int xNormal = ((int)std::floor(xpos))/POINT_SIZE;


    // MAP[yNormal][xNormal] = id;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // normalizes to tilemap
    int yN = (SCR_HEIGHT - (int)std::floor(ypos))/POINT_SIZE;
    int xN = ((int)std::floor(xpos))/POINT_SIZE;
    
    int r = 6;
    int t1 = r / 16;
    int t2 = 0;
    int x = r;
    int y = 0;

    if(!((xN-x) < XBOUND || (xN+x) > ((int)SCR_WIDTH/POINT_SIZE)-XBOUND ||
        (yN-y) < YBOUND || (yN+y) > ((int)SCR_HEIGHT/POINT_SIZE)-YBOUND))
        {
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

//START: PARTICLE RULES
std::vector<int> define_rule(int material, int perm, std::vector<int> state)
{
    std::vector<int> rule;
    std::vector<std::vector<int>> ruleset = {
        {0,0,0,0}, {material,0,0,0}, {0,material,0,0}, {material,material,0,0},
        {0,0,material,0}, {material,0,material,0}, {0,material,material,0}, {material,material,material,0},
        {0,0,0,material}, {material,0,0,material}, {0,material,0,material}, {material,material,0,material},
        {0,0,material,material}, {material,0,material,material}, {0,material,material,material}, {material,material,material,material}
    };
    for(int i=0; i<4; i++)
    {   
            if(state[i] == EMPTY || state[i] == material)
                rule.push_back(ruleset[perm][i]);
            else //Non empty and current material block stays in place
                rule.push_back(state[i]);
    }
    return rule;
}

std::vector<int>  water_rules(int mat, int ul, int ur, int ll, int lr){
    std::vector<int> state = {ul,ur,ll,lr};
    srand(time(NULL));
    int p = 1+(rand() % 2);

    //Mat can only move through empty
    if (ul == mat && ur == EMPTY && ll == EMPTY && lr == EMPTY)//p1
    {
        return define_rule(mat, 4, state);
    }
    if (ul == EMPTY && ur == mat && ll == EMPTY && lr == EMPTY)//p2
    {
        return define_rule(mat, 8, state);
    }
    if (ul == mat && ur == mat && ll == EMPTY && lr == EMPTY)//p3
    {
        return define_rule(mat, 12, state);    
    }
    if (ul == mat && ur == mat && ll == EMPTY && lr == mat)//p11
    {
        return define_rule(mat, 14, state);    
    }
    if (ul == mat && ur == mat && ll == mat && lr == EMPTY) //p7
    {
        return define_rule(mat, 13, state);    
    }
    if (ul == EMPTY && ur == mat && ll == mat && lr == EMPTY) //p6
    {
        return define_rule(mat, 12, state);    
    }
    if (ul == mat && ur == EMPTY && ll == EMPTY && lr == mat) //p9
    {
        return define_rule(mat, 12, state);    
    }
    if (ul == EMPTY && ur == mat && ll == EMPTY && lr == mat) //p10
    {
        return define_rule(mat, 12, state);    
    }
    if (ul == mat && ur == EMPTY && ll == mat && lr == EMPTY) //p5
    {
        return define_rule(mat, 12, state);    
    } if (ul == EMPTY && ur == EMPTY && ll == EMPTY && lr == mat) //p4
    {   
        return define_rule(mat, 4, state);    
    } if (ul == EMPTY && ur == EMPTY && ll == mat && lr == EMPTY) //p8
    {
        return define_rule(mat, 8, state);    
    } 

    return state;
}


std::vector<int> sand_rules(int mat, int ul, int ur, int ll, int lr){
    std::vector<int> state = {ul,ur,ll,lr};
    srand(time(NULL));
    int p = 1+(rand() % 4);

    //Mat can only move through empty
    if (ul == mat && ur == EMPTY && ll == EMPTY && lr == EMPTY)//p1
    {
        return define_rule(mat, 4, state);
    }
    if (ul == EMPTY && ur == mat && ll == EMPTY && lr == EMPTY)//p2
    {
        return define_rule(mat, 8, state);
    }
    if (ul == mat && ur == mat && ll == EMPTY && lr == EMPTY)//p3
    {
        return define_rule(mat, 12, state);    
    }
    if (ul == mat && ur == mat && ll == EMPTY && lr == mat)//p11
    {
        return define_rule(mat, 14, state);    
    }
    if (ul == mat && ur == mat && ll == mat && lr == EMPTY) //p7
    {
        return define_rule(mat, 13, state);    
    }
    if (ul == EMPTY && ur == mat && ll == mat && lr == EMPTY) //p6
    {
        return define_rule(mat, 12, state);    
    }
    if (ul == mat && ur == EMPTY && ll == EMPTY && lr == mat) //p9
    {
        return define_rule(mat, 12, state);    
    }
    if (ul == EMPTY && ur == mat && ll == EMPTY && lr == mat) //p10
    {
        // if (p == 1)
        // {
        //     return define_rule(mat, 12, state);
        // }else
        // {
        //     return define_rule(mat, 10, state);    
        // }
        return define_rule(mat, 12, state);
    }
    if (ul == mat && ur == EMPTY && ll == mat && lr == EMPTY) //p5
    {
        // if (p == 1)
        // {
        //     return define_rule(mat, 12, state);
        // }else
        // {
        //     return define_rule(mat, 10, state);    
        // }
        
        return define_rule(mat, 12, state);
        
    }
    return state;
}

void map_think(int xshift, int yshift) {
    for (int y=(YBOUND+yshift); y < (int)((SCR_HEIGHT/POINT_SIZE)-YBOUND)+yshift; y+=2)
    {
       for (int x=(XBOUND+xshift); x < (int)((SCR_WIDTH/POINT_SIZE)-XBOUND)+xshift; x+=2)
       {
            int upLeft = MAP[y+1][x];
            int upRight = MAP[y+1][x+1];
            int lowLeft = MAP[y][x];
            int lowRight = MAP[y][x+1];

            std::vector<int> rules = sand_rules(SAND, upLeft, upRight, lowLeft, lowRight);

            upLeft = rules[0];
            upRight = rules[1];
            lowLeft = rules[2];
            lowRight = rules[3];

            rules = water_rules(WATER, upLeft, upRight, lowLeft, lowRight);

            MAP[y+1][x] = rules[0];
            MAP[y+1][x+1] = rules[1];
            MAP[y][x] = rules[2];
            MAP[y][x+1] = rules[3];
            
        }
    }

}
//END: PARTICLE RULES


void setup_glfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
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
        //std::cout << "clicked: (" << xN << ", " << yN << ")" << " upperbound: (" << xG << ", " << yG << ")" << std::endl;
        if((yN <= yG && yN >= 0) && (xN <= xG && xN > prev_xG))
        {
            
            ACTIVE_PARTICLE = GUI_LAYOUT[x];
           // std::cout << ACTIVE_PARTICLE << std::endl;
            break;
        }
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

int main()
{
    setup_glfw();
    std::unordered_map<std::string, Particle> particles;

    unsigned int particleShader, tileShader;
    particleShader = make_shader(
        "../src/view/particle_shader.vert",
        "../src/view/particle_shader.frag"
    );
    tileShader = make_shader(
        "../src/view/tile_shader.vert",
        "../src/view/tile_shader.frag"
    );
    
    unsigned int particleVAO = make_particle_mesh();

    unsigned int transformVBO;
    glGenBuffers(1, &transformVBO);

    unsigned int colorVBO;
    glGenBuffers(1, &colorVBO);

    Particle sandParticle;
    sandParticle.id = 2;
    sandParticle.color = {0.96f, 0.7f, 0.0f, 1.0f};

    Particle waterParticle;
    waterParticle.id = 3;
    waterParticle.color = {0.35f, 0.7f, 1.0f, 1.0f};

    Particle wallParticle;
    wallParticle.id = 1;
    wallParticle.color = {0.5f, 0.5f, 0.5f, 1.0f};

    particles.insert({"SAND", sandParticle});
    particles.insert({"WATER", waterParticle});
    particles.insert({"WALL", wallParticle});

    Tile particleButton;
    particleButton.imagePath = "../images/wood_frame.png";
    particleButton.shader = tileShader;
    particleButton.dimension = 32;
    particleButton.color = {0.7f, 0.6f, 0.0f};
    make_tile_mesh(particleButton);

    Tile logEdge;
    logEdge.imagePath = "../images/wood_edge.png";
    logEdge.shader = tileShader;
    logEdge.dimension = 32;
    logEdge.color = {0.0f, 0.0f, 0.0f};
    make_tile_mesh(logEdge);

    Tile logBody;
    logBody.imagePath = "../images/wood_body.png";
    logBody.shader = tileShader;
    logBody.dimension = 32;
    logEdge.color = {0.0f, 0.0f, 0.0f};
    make_tile_mesh(logBody);


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


        glClearColor(0.14f, 0.16f, 0.21f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        map_think(itrThink[itr_i][0], itrThink[itr_i][1]);

        //draw_tiles();
        render_map(window, particles, particleVAO, particleShader, transformVBO, colorVBO);

        render_gui(logBody, logEdge, particleButton, particles);
        
        if (itr_i < 3){    
            itr_i += 1;
        }else{ 
            itr_i = 0;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();



    }

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);
    // glDeleteProgram(shader);

    glfwTerminate();
    return 0;

}

