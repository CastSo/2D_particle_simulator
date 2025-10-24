#include "factory.h"



Factory::Factory(World world, Mesh& particleInstance,  Mesh& tileInstance, std::unordered_map<std::string, Tile>& tiles): 
    particleInstance(particleInstance),
    tileInstance(tileInstance),
    tiles(tiles){
        maxParticles = world.MAX_PARTICLES;
    }

Factory::~Factory() {


}

void Factory::make_particles()
{
    particleInstance = make_particle_instance();
    unsigned int particleShader;
    particleShader = make_shader(
        "../src/view/particle_shader.vert",
        "../src/view/particle_shader.frag"
    );

    particleInstance.shader = particleShader;
    particleInstance.transformVBO = make_particle_transform_buffer();
    particleInstance.colorVBO = make_particle_color_buffer();


}

void Factory::make_tiles(){
    unsigned int tileShader;
    tileShader = make_shader(
        "../src/view/tile_shader.vert",
        "../src/view/tile_shader.frag"
    );

    tileInstance.shader = tileShader;
    tileInstance.VAO = make_tile_instance();

    for(auto& tile : tiles)
    {
        
        make_tile_textures(tileShader, tile.second, "../images/select_frame.png");
    }
    
}

//START: MAKE MESHES
Mesh Factory::make_particle_instance() {
    unsigned int VBO, VAO, EBO;
    std::vector<float> vertices = {
        0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    Mesh mesh;
    mesh.VAO = VAO;
    mesh.VBO = VBO;
    mesh.EBO = EBO;

    return mesh;

}

unsigned int Factory::make_particle_transform_buffer() 
{
    unsigned int transformVBO;
    //generates a VBO
    glGenBuffers(1, &transformVBO);
    glBindBuffer(GL_ARRAY_BUFFER, transformVBO);
    //passes through shader
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * maxParticles,  NULL, GL_STREAM_DRAW);
    std::size_t vec4Size = sizeof(glm::vec4);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0 * vec4Size));
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * vec4Size));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * vec4Size));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * vec4Size));
    glVertexAttribDivisor(4, 1);

    return transformVBO;
}

unsigned int Factory::make_particle_color_buffer() {
    unsigned int colorVBO;

    //generates a VBO
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * maxParticles,  NULL, GL_STREAM_DRAW);
    //passes through shader
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(5, 1); 

    
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    //unbinds buffer
    glBindVertexArray(0);   
    
    return colorVBO;
}

unsigned int Factory::make_tile_instance() {
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

void Factory::make_tile_textures(unsigned int shader, Tile& tile, std::string imgSelectPath) 
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

    unsigned char *data = stbi_load(tile.imgMainPath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture:  " << stbi_failure_reason() << std::endl;
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

    data = stbi_load(imgSelectPath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);

    glUseProgram(shader);
    unsigned int mainTexLoc = glGetUniformLocation(shader, "mainTexture");
    glUniform1i(mainTexLoc, 0);
    unsigned int selectTexLoc = glGetUniformLocation(shader, "selectTexture");
    glUniform1i(selectTexLoc, 1);
    

    unsigned int isSelectLoc = glGetUniformLocation(shader, "isSelected");
    glUniform1i(isSelectLoc, false);
    

    tile.mainTextureBufr = mainTexture;
    tile.selectTextureBufr = selectTexture;
    tile.isSelected = isSelectLoc;
}



//END: MAKE MESHES
