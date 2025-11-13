#include "config.h"
#include "./factory.h"
#include "./think.h"
#include "./render.h"

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
std::vector<int> GUI_LAYOUT;
int ACTIVE_PARTICLE;


bool mouseDown;

Gui* gui;

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

            GUI_LAYOUT.push_back(p->second.id);

            p++;
        
        }else
        {
            GUI_LAYOUT.push_back(0);

    }
        
    }
}
//END: SETUP LAYOUT


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


    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        int clickedTile = gui->detect_mouse();
        if(clickedTile != 0)
        ACTIVE_PARTICLE = clickedTile;
        
        mouseDown = true;
        
    } else {
        mouseDown = false;
    }

    //int selected_coords = detect_mouse(xN, yN);

    
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
    waterParticle.isDestroyer = true;
    waterParticle.rules = {ruleset[1], ruleset[2], ruleset[3], ruleset[3], ruleset[11], ruleset[7],
                    ruleset[6], ruleset[9], ruleset[10], ruleset[5], ruleset[4], ruleset[8]};
    waterParticle.transitions = {ruleset[4], ruleset[8], ruleset[12], ruleset[14], ruleset[13], ruleset[12],
                    ruleset[12], ruleset[12], ruleset[12], ruleset[12], ruleset[8], ruleset[4]};
    waterParticle.destroyables = {FIRE};

    Particle woodParticle;
    woodParticle.id = WOOD;
    woodParticle.color = {0.60f, 0.24f, 0.06f, 1.0f};
    woodParticle.secondColor = {0.22f, 0.11f, 0.0f, 1.0f};

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
    fireParticle.destroyables = {WOOD};


   
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
    Material_Lookup materialLookup;

    //Shared mesh data 
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
    Render* render = new Render(window, MAP, GUI_LAYOUT, world, materialLookup, particleInstance, tileInstance, charQuad, tiles, particles, characters);
    gui = new Gui(window, world, materialLookup, GUI_LAYOUT);
    //Sets up shaders and VAO of particles and tiles
    factory->make_particles();
    factory->make_tiles();
    factory-> make_char();

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

        render->update();
        //render_gui(tileInstance, tiles.at("LOG_BODY"), tiles.at("LOG_EDGE"), tiles.at("BUTTON"), particles);
        

        
 
        glfwSwapBuffers(window);
        glfwPollEvents();



    }


    delete factory;
    delete think;
    delete render;
    delete gui;
    glfwTerminate();
    return 0;

}

