#include "factory.h"



Factory::Factory(World world, Mesh& particleInstance,  Mesh& tileInstance, Mesh& charQuad,
            std::unordered_map<std::string, Tile>& tiles, std::map<GLchar, Character>& characters): 
    particleInstance(particleInstance),
    tileInstance(tileInstance),
    charQuad(charQuad),
    tiles(tiles),
    characters(characters){
        maxParticles = world.MAX_PARTICLES;
        SCR_WIDTH = world.SCR_WIDTH;
        SCR_HEIGHT = world.SCR_HEIGHT;
    }

Factory::~Factory() {
    glDeleteVertexArrays(1, &particleInstance.VAO);
    glDeleteBuffers(1, &particleInstance.VBO);
    glDeleteBuffers(1, &particleInstance.EBO);
    glDeleteProgram(particleInstance.shader);


    glDeleteVertexArrays(1, &tileInstance.VAO);
    glDeleteBuffers(1, &tileInstance.VBO);
    glDeleteBuffers(1, &tileInstance.EBO);
    glDeleteProgram(tileInstance.shader);
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

void Factory::make_char() {
    unsigned int charShader;
    charShader  = make_shader(
        "../src/view/character_shader.vert",
        "../src/view/character_shader.frag"
    );

    make_char_quad(charShader);


    charQuad.shader = charShader;
    
    


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

void Factory::make_tile_textures(unsigned int shader, Tile& tile, std::string imgSecondPath) 
{
    unsigned int mainTexture, secondTexture;

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

    glGenTextures(1, &secondTexture);
    glBindTexture(GL_TEXTURE_2D, secondTexture); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    data = stbi_load(imgSecondPath.c_str(), &width, &height, &nrChannels, 0);
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
    unsigned int secondTexLoc = glGetUniformLocation(shader, "secondTexture");
    glUniform1i(secondTexLoc, 1);
    

    unsigned int isSelectLoc = glGetUniformLocation(shader, "isSelected");
    glUniform1i(isSelectLoc, false);
    

    tile.mainTextureBufr = mainTexture;
    tile.secondTextureBufr = secondTexture;
    tile.isSelected = isSelectLoc;
}

void Factory::make_char_quad(unsigned int shader) {
    glUseProgram(shader);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        
    }

    
    std::string fontPath = std::filesystem::path("../src/fonts/Space_Mono/SpaceMono-Regular.ttf");
    if (fontPath.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
       
    } else {
        FT_Set_Pixel_Sizes(face, 0, 48);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

         for (unsigned char c = 0; c < 128; c++)
        {
             if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int charTexture;
            glGenTextures(1, &charTexture);
            glBindTexture(GL_TEXTURE_2D, charTexture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                charTexture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            characters.insert(std::pair<char, Character>(c, character));
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    charQuad.VAO = VAO;
    charQuad.VBO = VBO;

}

//END: MAKE MESHES
