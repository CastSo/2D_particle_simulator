#include "config.h"
#include "./components/particle.h"
#include "./components/tile.h"
#include "./components/world.h"
#include "./factory.h"

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
int MAP[(int)(SCR_HEIGHT/POINT_SIZE)][(int)(SCR_WIDTH/POINT_SIZE)];
int GUI_LAYOUT[(int)(SCR_WIDTH/TILE_SIZE)];
int ACTIVE_PARTICLE;


struct ParticleMesh {

    unsigned int VAO;
    unsigned int transformVBO;
    unsigned int colorVBO;
};

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
                transforms.push_back(map_particle(x, y));
                colors.push_back(particles.at("FIRE").color);
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


void render_gui(unsigned int VAO, unsigned int shader, Tile body, Tile edge, Tile frame, std::unordered_map<std::string, Particle> particles) {
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
    
    glBindVertexArray(VAO);
    glUseProgram(shader);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
    transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, edge.mainTexture);

    glUniform1i(frame.isSelected, false);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    //get matrix's uniform location and set matrix
    
    unsigned int transformLoc = glGetUniformLocation(shader, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    unsigned int bgColorLoc = glGetUniformLocation(shader, "backgroundColor");
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
            glBindTexture(GL_TEXTURE_2D, frame.mainTexture);

            if(detect_mouse(xm, ym) == GUI_LAYOUT[x])
            {
                glUniform1i(frame.isSelected, true);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, frame.selectTexture);
            }else {
                glUniform1i(frame.isSelected, false);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, 0);
            }


            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
            transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));

            //get matrix's uniform location and set matrix
            unsigned int transformLoc = glGetUniformLocation(shader, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));        
            

            unsigned int bgColorLoc = glGetUniformLocation(shader, "backgroundColor");
            glUniform4f(bgColorLoc, p->second.color.x, p->second.color.y, p->second.color.z, 1.0f); 

            p++;
        
        }
        else {
            // glBindVertexArray(body.VAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, body.mainTexture);

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(xN, yN, 1.0f));
            transform = glm::scale(transform, glm::vec3(tileWidth, tileHeight, 1.0f));

            //get matrix's uniform location and set matrix
            glUseProgram(shader);
            unsigned int transformLoc = glGetUniformLocation(shader, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        }
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Resets texture to just main
        glUniform1i(frame.isSelected, false);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        
    }
    
}


//END:RENDER OBJECTS

//START: MAKE MESHES

void make_tile_textures(unsigned int shader, Tile& frame, std::string selectFramePath) 
{
    unsigned int mainTexture, selectTexture;

    glGenTextures(1, &mainTexture);
    glBindTexture(GL_TEXTURE_2D, mainTexture); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
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

    glGenTextures(1, &selectTexture);
    glBindTexture(GL_TEXTURE_2D, selectTexture); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    data = stbi_load(selectFramePath.c_str(), &width, &height, &nrChannels, 0);
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

    glUseProgram(shader);
    unsigned int mainTexLoc = glGetUniformLocation(shader, "mainTexture");
    glUniform1i(mainTexLoc, 0);
    unsigned int selectTexLoc = glGetUniformLocation(shader, "selectTexture");
    glUniform1i(selectTexLoc, 1);
    

    unsigned int isSelectLoc = glGetUniformLocation(shader, "isSelected");
    glUniform1i(isSelectLoc, false);
    

    frame.mainTexture = mainTexture;
    frame.selectTexture = selectTexture;
    frame.isSelected = isSelectLoc;
}

unsigned int make_tile_mesh() {
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
    
        

    return VAO;
}
//END: MAKE MESHES

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
    
        r = 8;
        
    }

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
std::vector<int> define_rule(int material, int permutation, std::vector<int> state)
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

            if (material == FIRE)
            {
                if(state[i] == EMPTY || state[i] == material || state[i] == WOOD)//Passes only through empty, wood, and same material
                {    
                    rule.push_back(ruleset[permutation][i]);
                }else
                {
                    rule.push_back(state[i]);
                }   
            }else 
            {    if(state[i] == EMPTY || state[i] == material)//Passes only through empty and same material
                {    
                    rule.push_back(ruleset[permutation][i]);
                }else
                {
                    rule.push_back(state[i]);
                }
            }

            
    }
    return rule;
}


std::vector<int> fire_rules(int mat, int ul, int ur, int ll, int lr) {
    std::vector<int> state = {ul,ur,ll,lr};

    int p = 1+(rand() % 4);

    if (ul == mat && ur != mat && ll != mat && lr != mat) { //p1
        
        if(p == 1)
        {
            return define_rule(mat, 2, state);
        }
        return define_rule(mat, 8, state);
        
    }
    if (ul != mat && ur == mat && ll != mat && lr != mat) { //p2

        if(p == 1)
        {
            return define_rule(mat, 1, state);
        }
        return define_rule(mat, 4, state);
        
    }
    if (ul != mat && ur != mat && ll != mat && lr == mat) {//p8

        if(p == 1)
        {
            return define_rule(mat, 8, state);
        }
        return define_rule(mat, 2, state);

    }
    if (ul != mat && ur != mat && ll == mat && lr != mat) {//p4
        if(p == 1)
        {
            return define_rule(mat, 4, state);
        }
        return define_rule(mat, 1, state);
       
    } 
    if (ul != mat && ur != mat && ll == mat && lr == mat) {//p12
        if (p == 1)
        {
            return define_rule(mat, 6, state);
        }
        return define_rule(mat, 9, state);
        
        
    }
     if (ul == mat && ur == mat && ll != mat && lr != mat) { //p3
        if (p == 1)
        {
            return define_rule(mat, 6, state);
        }
        return define_rule(mat, 9, state);
        
    }
    if (ul != mat && ur == mat && ll != mat && lr == mat) {//p10
        if (p == 1)
        {
            return define_rule(mat, 6, state);
        }
        return define_rule(mat, 9, state);
        
    }
     if (ul == mat && ur != mat && ll == mat && lr != mat) { //p5
        if (p == 1)
        {
            return define_rule(mat, 6, state);
        }
        return define_rule(mat, 9, state);
        
    }
    if (ul == mat && ur == mat && ll == mat && lr == mat) { //p15
        if (p == 1)
        {
            return define_rule(mat, 14, state);
        }
        return define_rule(mat, 13, state);
        
    }
    if (ul == mat && ur == mat && ll == mat && lr != mat) { //p7
        return define_rule(mat, 14, state);
        
    }
    if (ul != mat && ur == mat && ll == mat && lr == mat) { //p14
        return define_rule(mat, 7, state);
        
    }
     if (ul != mat && ur == mat && ll == mat && lr != mat) { //p6
    
        return define_rule(mat, 9, state);
        
    } 
    if (ul == mat && ur != mat && ll != mat && lr == mat) { //p9

        return define_rule(mat, 6, state);
        
    }
    return state;
}

std::vector<int> gas_rules(int mat, int ul, int ur, int ll, int lr) {
    std::vector<int> state = {ul,ur,ll,lr};

    int p = 1+(rand() % 3);

    if (ul == mat && ur == EMPTY && ll == EMPTY && lr == EMPTY) { //p1
        
        if(p == 1)
        {
            return define_rule(mat, 2, state);
        }
        return define_rule(mat, 8, state);
        
    }
    if (ul == EMPTY && ur == mat && ll == EMPTY && lr == EMPTY) { //p2

        if(p == 1)
        {
            return define_rule(mat, 1, state);
        }
        return define_rule(mat, 4, state);
        
    }
    if (ul == EMPTY && ur == EMPTY && ll == EMPTY && lr == mat) {//p8

        if(p == 1)
        {
            return define_rule(mat, 8, state);
        }
        return define_rule(mat, 2, state);

    }
    if (ul == EMPTY && ur == EMPTY && ll == mat && lr == EMPTY) {//p4
        if(p == 1)
        {
            return define_rule(mat, 4, state);
        }
        return define_rule(mat, 1, state);
       
    } 
    if (ul == EMPTY && ur == EMPTY && ll == mat && lr == mat) {//p12
        if (p == 1)
        {
            return define_rule(mat, 6, state);
        }
        return define_rule(mat, 9, state);
        
        
    }
     if (ul == mat && ur == mat && ll == EMPTY && lr == EMPTY) { //p3
        if (p == 1)
        {
            return define_rule(mat, 6, state);
        }
        return define_rule(mat, 9, state);
        
    }
    if (ul == EMPTY && ur == mat && ll == EMPTY && lr == mat) {//p10
        if (p == 1)
        {
            return define_rule(mat, 6, state);
        }
        return define_rule(mat, 9, state);
        
    }
     if (ul == mat && ur == EMPTY && ll == mat && lr == EMPTY) { //p5
        if (p == 1)
        {
            return define_rule(mat, 6, state);
        }
        return define_rule(mat, 9, state);
        
    }
    if (ul == mat && ur == mat && ll == mat && lr == mat) { //p15
        if (p == 1)
        {
            return define_rule(mat, 14, state);
        }
        return define_rule(mat, 13, state);
        
    }
    if (ul == mat && ur == mat && ll == mat && lr == EMPTY) { //p7
        return define_rule(mat, 14, state);
        
    }
    if (ul == EMPTY && ur == mat && ll == mat && lr == mat) { //p14
        return define_rule(mat, 7, state);
        
    }
     if (ul == EMPTY && ur == mat && ll == mat && lr == EMPTY) { //p6
    
        return define_rule(mat, 9, state);
        
    } 
    if (ul == mat && ur == EMPTY && ll == EMPTY && lr == mat) { //p9

        return define_rule(mat, 6, state);
        
    }
    return state;
}

std::vector<int>  water_rules(int mat, int ul, int ur, int ll, int lr){
    std::vector<int> state = {ul,ur,ll,lr};


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
    } 
    if (ul == EMPTY && ur == EMPTY && ll == EMPTY && lr == mat) //p4
    {   
        
        return define_rule(mat, 4, state);    
    }
    if (ul == EMPTY && ur == EMPTY && ll == mat && lr == EMPTY) //p8
    {

        return define_rule(mat, 8, state);    
    } 

    return state;
}


std::vector<int> sand_rules(int mat, int ul, int ur, int ll, int lr){
    std::vector<int> state = {ul,ur,ll,lr};

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
        
    }
    return state;
}

bool in_quad(int material, int upLeft, int upRight, int lowLeft, int lowRight)
{
    if (upLeft == material || upRight == material || lowLeft == material || lowRight == material)
    {
        return true;
    }
    return false;
}

//Determines the rules of each quad
void map_think(int xshift, int yshift) {
    srand(time(NULL));
    for (int y=(YBOUND+yshift); y < (int)((SCR_HEIGHT/POINT_SIZE)-YBOUND)+yshift; y+=2)
    {
       for (int x=(XBOUND+xshift); x < (int)((SCR_WIDTH/POINT_SIZE)-XBOUND)+xshift; x+=2)
       {
            int upLeft = MAP[y+1][x];
            int upRight = MAP[y+1][x+1];
            int lowLeft = MAP[y][x];
            int lowRight = MAP[y][x+1];
            std::vector<int> rules;

            if((upLeft == EMPTY && upRight == EMPTY && lowLeft == EMPTY && lowRight == EMPTY) ||
            (upLeft > EMPTY && upRight > EMPTY && lowLeft > EMPTY && lowRight > EMPTY) ||
            (upLeft == WOOD || upRight == WOOD || lowLeft == WOOD || lowRight == WOOD) )
            {
                continue;
            }



            if(in_quad(SAND, upLeft, upRight, lowLeft, lowRight)){
                rules  = sand_rules(SAND, upLeft, upRight, lowLeft, lowRight);

                upLeft = rules[0];
                upRight = rules[1];
                lowLeft = rules[2];
                lowRight = rules[3];
            }

            if(in_quad(WATER, upLeft, upRight, lowLeft, lowRight)){
                rules = water_rules(WATER, upLeft, upRight, lowLeft, lowRight);

                upLeft = rules[0];
                upRight = rules[1];
                lowLeft = rules[2];
                lowRight = rules[3];
            }

            if(in_quad(GAS, upLeft, upRight, lowLeft, lowRight)){
                rules = gas_rules(GAS, upLeft, upRight, lowLeft, lowRight);

                upLeft = rules[0];
                upRight = rules[1];
                lowLeft = rules[2];
                lowRight = rules[3];
            }

            if(in_quad(FIRE, upLeft, upRight, lowLeft, lowRight)){
                rules = fire_rules(FIRE, upLeft, upRight, lowLeft, lowRight);
                
                upLeft = rules[0];
                upRight = rules[1];
                lowLeft = rules[2];
                lowRight = rules[3];
            }

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
            
            
            //std::cout << ACTIVE_PARTICLE << std::endl;
            
            return GUI_LAYOUT[x];
        }
    }
    return EMPTY;
}

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
    setup_glfw();

    World world;
    Mesh particleInstance;
    Mesh tileInstance;
    world.MAX_PARTICLES = ((SCR_WIDTH/ POINT_SIZE) * (SCR_HEIGHT/ POINT_SIZE));

    std::unordered_map<std::string, Particle> particles;
    Factory* factory = new Factory(world, particleInstance, tileInstance);
    //std::cout << particleInstance.transformVBO << std::endl;
    factory->make_particles();
    factory->make_tiles();

    Particle sandParticle;
    sandParticle.id = SAND;
    sandParticle.color = {0.96f, 0.7f, 0.0f, 1.0f};

    Particle waterParticle;
    waterParticle.id = WATER;
    waterParticle.color = {0.35f, 0.7f, 1.0f, 1.0f};

    Particle woodParticle;
    woodParticle.id = WOOD;
    woodParticle.color = {0.60f, 0.24f, 0.06f, 1.0f};

    Particle gasParticle;
    gasParticle.id = GAS;
    gasParticle.color = {0.0f, 0.7f, 0.4f, 1.0f};

    Particle fireParticle;
    fireParticle.id = FIRE;
    fireParticle.color = {1.0f, 0.3f, 0.0f, 1.0f};



   
    particles.insert({"FIRE", fireParticle});
    particles.insert({"WOOD", woodParticle});
    particles.insert({"GAS", gasParticle});
    particles.insert({"WATER", waterParticle});
    particles.insert({"SAND", sandParticle});
    

    std::string selectFramePath = "../images/select_frame.png";

    unsigned int tileVAO = make_tile_mesh();

    Tile particleButton;
    particleButton.imagePath = "../images/wood_frame.png";
    particleButton.dimension = (int)TILE_SIZE;
    particleButton.color = {0.7f, 0.6f, 0.0f};
    make_tile_textures(tileInstance.shader, particleButton, selectFramePath);

    Tile logEdge;
    logEdge.imagePath = "../images/wood_edge.png";
    logEdge.dimension = (int)TILE_SIZE;
    logEdge.color = {0.0f, 0.0f, 0.0f};
    make_tile_textures(tileInstance.shader, logEdge, selectFramePath);

    Tile logBody;
    logBody.imagePath = "../images/wood_body.png";
    logBody.dimension = (int)TILE_SIZE;
    logEdge.color = {0.0f, 0.0f, 0.0f};
    make_tile_textures(tileInstance.shader, logBody, selectFramePath);



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


        map_think(itrThink[itr_i][0], itrThink[itr_i][1]);

        //draw_tiles();
        render_particles(window, particleInstance, particles);

        render_gui(tileInstance.VAO, tileInstance.shader,logBody, logEdge, particleButton, particles);
        
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
    delete factory;
    glfwTerminate();
    return 0;

}

