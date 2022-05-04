// Std. Includes
#include <string>
#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"
// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );


// Camera
Camera camera( glm::vec3( -2.5f, 10.0f, 25.0f ) );
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
float rot = 0.0f;

// Variables
float transChair = 0.0f;
float rotChair = 0.0f;
bool activeChair;

float transTrash = 0.0f;
float rotTrash = 0.0f;
bool activeTrash;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(3.822f, 4.373f, 10.006f)
};

glm::vec3 Light1 = glm::vec3(0);


int main( )
{
    // Init GLFW
    glfwInit( );
    // Set all the required options for GLFW
    /*glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );*/
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "Proyecto final", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    
    // GLFW Options
    //glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    // Setup and compile our shaders
    Shader shader( "Shaders/modelLoading.vs", "Shaders/modelLoading.frag" );
    Shader lampshader( "Shaders/lamp.vs", "Shaders/lamp.frag" );
    Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");


    // Load models
    glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( float )SCREEN_WIDTH/( float )SCREEN_HEIGHT, 0.1f, 100.0f );
    
    GLfloat vertices[] =
    {
        // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f    // top left 

    };

    GLuint indices[] =
    {  // Note that we start from 0!
        0,1,3,
        1,2,3

    };

    // First, set the container's VAO (and VBO)
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load textures
    Model chair((char*)"Models/Silla/silla.obj");
    Model table((char*)"Models/Table/table.obj");
    Model sofa((char*)"Models/Sofa/sofa.obj");
    Model mostrador((char*)"Models/Mostrador/mostrador.obj");
    Model bote((char*)"Models/Bote/bote.obj");
    Model trash((char*)"Models/Trash/trash.obj");
    Model tapa((char*)"Models/Trash/tapa.obj");
    Model interior((char*)"Models/Interior/interior.obj");
    Model pilar((char*)"Models/Interior/pilar.obj");
    Model maquina((char*)"Models/Maquina/maquina.obj");
    Model lamp((char*)"Models/Lamp/lamp.obj");


    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    int textureWidth, textureHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* image;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Set texture units
    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        DoMovement();

        // Clear the colorbuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.Use();

        // Point light 1
        glm::vec3 lightColor;
        lightColor.x = abs(sin(glfwGetTime() * Light1.x));
        lightColor.y = abs(sin(glfwGetTime() * Light1.y));
        lightColor.z = sin(glfwGetTime() * Light1.z);

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.35f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.44f);

        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

        shader.Use();

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Draw the loaded model
        glm::mat4 model(1);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        
        // Draw the chairs
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(1.444f + transChair, 0.172f, 11.004f));
        model = glm::rotate(model, glm::radians(rotChair), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.684f, 0.684f, 0.684f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        chair.Draw(shader);
        
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(1.444f, 0.172f, 9.131f));
        model = glm::scale(model, glm::vec3(0.684f, 0.684f, 0.684f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        chair.Draw(shader);
        
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(1.444f, 0.172f, 5.981f));
        model = glm::scale(model, glm::vec3(0.684f, 0.684f, 0.684f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        chair.Draw(shader);
        
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(1.444f, 0.172f, 3.703f));
        model = glm::scale(model, glm::vec3(0.684f, 0.684f, 0.684f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        chair.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(1.444f, 0.172f, 0.745f));
        model = glm::scale(model, glm::vec3(0.684f, 0.684f, 0.684f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        chair.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(1.444f, 0.172f, -1.381f));
        model = glm::scale(model, glm::vec3(0.684f, 0.684f, 0.684f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        chair.Draw(shader);


        // Draw the tables
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(3.824f, 0.193f, 10.098f));
        model = glm::scale(model, glm::vec3(0.659f, 0.659f, 0.659f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        table.Draw(shader);
        
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(3.824f, 0.193f, 4.712f));
        model = glm::scale(model, glm::vec3(0.659f, 0.659f, 0.659f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        table.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.824f, 0.193f, -0.375f));
        model = glm::scale(model, glm::vec3(0.659f, 0.659f, 0.659f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        table.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-4.491f, 0.193f, 8.578f));
        model = glm::scale(model, glm::vec3(0.659f, 0.659f, 0.659f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        table.Draw(shader);
        
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-4.491f, 0.193f, -0.195f));
        model = glm::scale(model, glm::vec3(0.659f, 0.659f, 0.659f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        table.Draw(shader);
        

        // Draw the sofas
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(7.237f, 0.0f, 10.036f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.636f, 0.636f, 0.636f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sofa.Draw(shader);
        
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(7.237f, 0.0f, 4.66f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.636f, 0.636f, 0.636f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sofa.Draw(shader);
        
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(7.237f, 0.0f, -0.452f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.636f, 0.636f, 0.636f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sofa.Draw(shader);
        
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(-4.438f, 0.0f, 12.247f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.636f, 0.636f, 0.636f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sofa.Draw(shader);
        
        model = glm:: mat4(1);
        model = glm::translate(model, glm::vec3(-4.438f, 0.0f, 5.308f));
        model = glm::scale(model, glm::vec3(0.636f, 0.636f, 0.636f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sofa.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-4.438f, 0.0f, 3.399f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.636f, 0.636f, 0.636f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sofa.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-4.438f, 0.0f, -4.0f));
        model = glm::scale(model, glm::vec3(0.636f, 0.636f, 0.636f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sofa.Draw(shader);


        // Draw the bote
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(6.497f, 0.0f, -5.397f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        trash.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(6.497f + transTrash, 0.0f + transTrash, -5.397f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotTrash), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        tapa.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(6.497f, 0.0f, -7.692f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        trash.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(6.497f + transTrash, 0.0f + transTrash, -7.692f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotTrash), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        tapa.Draw(shader);


        // Draw the counter
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-3.083f, 0.0f, -10.832f));
        model = glm::scale(model, glm::vec3(0.716f, 0.716f, 0.716f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        mostrador.Draw(shader);


        // Draw the machines
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-9.660f, 0.159f, 6.245f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        maquina.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-11.705f, 0.159f, 6.245f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        maquina.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-9.660f, 0.159f, 0.452f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        maquina.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-11.705f, 0.159f, 0.452f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        maquina.Draw(shader);


        // Draw the lamps
        //model = glm::mat4(1);
        //model = glm::translate(model, glm::vec3(3.822f, 4.373f, 10.006f));
        //glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        //glEnable(GL_BLEND); //Activa la funcionalidad para trabajar el canal alfa
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //model = glm::mat4(1);
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTransparencia"), 0);
        //glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 0.0, 0.75);
        //lamp.Draw(lightingShader);
        //glDisable(GL_BLEND); //Desactiva el canal alfa 
        //glUniform4f(glGetUniformLocation(lightingShader.Program, "colorAlpha"), 1.0, 1.0, 1.0, 1.0);
        
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.822f, 6.703f, 10.006f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.822f, 6.703f, 4.545f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp.Draw(shader);
        
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(3.822f, 6.703f, -0.15f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-3.64f, 6.703f, 8.895f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp.Draw(shader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-3.64f, 6.703f, 0.503f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp.Draw(shader);


        // Draw the interior
        model = glm::mat4(1);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        interior.Draw(shader);

        model = glm::mat4(1);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        pilar.Draw(shader);

        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        lampshader.Use();

        // Draw the light object (using light's vertex attributes)
        glBindVertexArray(lightVAO);
        for (GLuint i = 0; i < 4; i++)
        {
            model = glm::mat4(1);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);

        // Swap the buffers
        glfwSwapBuffers( window );
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate( );
    return 0;
}


// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if ( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    
    if ( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }

    //Pointlight lamp 1
    if (keys[GLFW_KEY_R])
    {
        pointLightPositions[0].x += 0.01f;
    }
    if (keys[GLFW_KEY_F])
    {
        pointLightPositions[0].x -= 0.01f;
    }

    if (keys[GLFW_KEY_T])
    {
        pointLightPositions[0].y += 0.01f;
    }

    if (keys[GLFW_KEY_G])
    {
        pointLightPositions[0].y -= 0.01f;
    }
    if (keys[GLFW_KEY_Y])
    {
        pointLightPositions[0].z -= 0.1f;
    }
    if (keys[GLFW_KEY_H])
    {
        pointLightPositions[0].z += 0.01f;
    }

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if ( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if ( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }

    if (keys[GLFW_KEY_SPACE])
    {
        active = !active;
        if (active)
        {
            Light1 = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        else
        {
            Light1 = glm::vec3(0); //Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
        }
    }

    if (keys[GLFW_KEY_R])
    {
        activeChair = !activeChair;
        if (activeChair)
        {
            transChair -= 1.0f;
            rotChair += 50.0f;
        }
        else
        {
            transChair += 1.0f;
            rotChair -= 50.0f;
        }
    }
    
    if (keys[GLFW_KEY_F])
    {
        activeTrash = !activeTrash;
        if (activeTrash)
        {
            rotTrash += 60.0f;
            transTrash += 2.2f;
        }
        else
        {
            rotTrash -= 60.0f;
            transTrash -= 2.2f;
        }
    }
 
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if ( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}

