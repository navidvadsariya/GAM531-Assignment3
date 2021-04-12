#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaderinit.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void init(void);
void render();


void drawCube1();
void drawCube2();
void drawCube3();
void drawCube4();
void drawPlane();

void applyTextureToCube1(unsigned int& texture);
void applyTextureToCube2(unsigned int& texture);
void applyTextureToCube3(unsigned int& texture);
void applyTextureToCube4(unsigned int& texture);

void applySecondTextureToCube1(unsigned int& texture);
void applySecondTextureToCube2(unsigned int& texture);
void applySecondTextureToCube3(unsigned int& texture);
void applySecondTextureToCube4(unsigned int& texture);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// variables
const unsigned int screen_width = 1200;
const unsigned int screen_height = 800;
const GLuint NumVertices = 36;

GLuint VBO1, VBO2, VBO3, VBO4, VBO5;
GLuint VAO1, VAO2, VAO3, VAO4 , VAO5;
GLuint EBO1, EBO2, EBO3, EBO4, EBO5;

unsigned int texture;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


bool objectScale = false;           //processInput functions sets this value true if user presses S and cube is rotating
bool objectMoveToCenter = false;    //processInput functions sets this value true if user presses W and cube is rotating
bool removeTransformation = false;  //processInput functions sets this value true if user presses Q and cube is rotating
bool isCubeRotating = false;        //processInput functions sets this value true if user presses R

int randomCubeNumber = -1;          //stores cube number from 1-4

bool changeTexture = false;

//light
unsigned int lightCubeVAO, lightCubeVBO;
unsigned int cubeVAO;

bool rubyMat = false;
int randomRubyMat = -1;

// light source position
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window;
    window = glfwCreateWindow(screen_width, screen_height, "OpenGLAssignment", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    // build and compile our shader program

    Shader ourShader("lightingmap.vs", "lightingmap.fs"); // you can name your shader files however you like
    Shader lightCubeShader("lightsource.vs", "lightsource.fs");
    Shader lightingShader("materials.vs", "materials.fs");

    float light[] = {
     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
       0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
       0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
       0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
       0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

       0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
       0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
       0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
       0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
       0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
       0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
       0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
       0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
       0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
       0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
       0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(light), light, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //cube 1
    float vertices1[] = {
        // top left cube-negative z
        // positions            // colors           // texture coords
        -1.0f, 0.5f, -0.25f,    1.0f, 0.0f, 1.0f,       1.0f, 1.0f,         // top left
        -0.5f, 0.5f, -0.25f,    0.0f, 1.0f, 0.0f,       1.0f, 0.0f,         // bottom left
        -0.5f, 1.0f, -0.25f,     1.0f, 0.0f, 1.0f,      0.0f, 0.0f,         // top right
        -1.0f, 1.0f, -0.25f,    1.0f, 1.0f, 1.0f,       0.0f, 1.0f,          // bottom right
        // top left cube-positive z
        // positions            // colors           // texture coords
        -1.0f, 0.5f, 0.25f, 1.0f, 0.0f, 1.0f,        1.0f, 1.0f,         // top left
         -0.5f, 0.5f, 0.25f, 0.0f, 1.0f, 0.0f,       1.0f, 0.0f,         // bottom left
        -0.5f, 1.0f, 0.25f, 1.0f, 0.0f, 1.0f,        0.0f, 0.0f,         // top right
        -1.0f, 1.0f, 0.25f, 1.0f, 0.0f, 1.0f,        0.0f, 1.0f         // bottom right
    };
    unsigned int indices[] = {
        5, 4, 0,
        1, 5, 0,

        6, 5, 1,
        2, 6, 1,

        7, 6, 2,
        3, 7, 2,

        4, 7, 3,
        0, 4, 3,

        6, 7, 4,
        5, 6, 4,

        1, 0, 3,
        2, 1, 3
    };

    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);
    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute information
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute information
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute information
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //cube 2
    float vertices2[] = {
        // bottom left cube-negative z
        -1.0f, -1.0f, -0.25f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -1.0f, -0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, -0.5f, -0.25f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        // bottom left cube-positive z
        -1.0f, -1.0f, 0.25f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
       -0.5f, -1.0f, 0.25f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, -0.5f, 0.25f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices2[] = {

        5, 4, 0,
        1, 5, 0,

        6, 5, 1,
        2, 6, 1,

        7, 6, 2,
        3, 7, 2,

        4, 7, 3,
        0, 4, 3,

        6, 7, 4,
        5, 6, 4,

        1, 0, 3,
        2, 1, 3
    };

    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO2);
    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);

    // position attribute information
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute information
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute information
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //cube 3
    float vertices3[] = {
        // top right cube-negative z
    0.5f, 0.5f, -0.25f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,// bottom left
   1.0f, 0.5f, -0.25f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,// bottom right
   1.0f, 1.0f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,// top right
   0.5f, 1.0f, -0.25f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // top left

    // top right cube-positive z
   0.5f, 0.5f, 0.25f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,// bottom left
   1.0f, 0.5f, 0.25f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,// bottom right
   1.0f, 1.0f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,// top right
   0.5f, 1.0f, 0.25f,0.0f, 1.0f, 0.0f,  0.0f, 1.0f// top left
    };
    unsigned int indices3[] = {

        5, 4, 0,
        1, 5, 0,

        6, 5, 1,
        2, 6, 1,

        7, 6, 2,
        3, 7, 2,

        4, 7, 3,
        0, 4, 3,

        6, 7, 4,
        5, 6, 4,

        1, 0, 3,
        2, 1, 3
    };

    glGenVertexArrays(1, &VAO3);
    glGenBuffers(1, &VBO3);
    glGenBuffers(1, &EBO3);
    glBindVertexArray(VAO3);

    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices3), indices3, GL_STATIC_DRAW);

    // position attribute information
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute information
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute information
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //cube 4
    float vertices4[] = {
        // bottom right cube-negative z
   0.5f, -1.0f, -0.25f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,// bottom left
   1.0f, -1.0f, -0.25f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,// bottom right
   1.0f, -0.5f, -0.25f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,// top right
   0.5f, -0.5f, -0.25f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,// top left

   // bottom right cube-positive z
    0.5f, -1.0f, 0.25f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,// bottom left
   1.0f, -1.0f, 0.25f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,// bottom right
   1.0f, -0.5f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,// top right
   0.5f, -0.5f, 0.25f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f// top left
    };
    unsigned int indices4[] = {
        5, 4, 0,
        1, 5, 0,

        6, 5, 1,
        2, 6, 1,

        7, 6, 2,
        3, 7, 2,

        4, 7, 3,
        0, 4, 3,

        6, 7, 4,
        5, 6, 4,

        1, 0, 3,
        2, 1, 3
    };

    glGenVertexArrays(1, &VAO4);
    glGenBuffers(1, &VBO4);
    glGenBuffers(1, &EBO4);
    glBindVertexArray(VAO4);

    glBindBuffer(GL_ARRAY_BUFFER, VBO4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices4), vertices4, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices4), indices4, GL_STATIC_DRAW);

    // position attribute information
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute information
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute information
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    float vertices5[] = {
   -8.0f, -8.0f, -6.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,// bottom left
   8.0f, -8.0f, -6.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,// bottom right
   8.0f, 8.0f, -6.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,// top right
   -8.0f, 8.0f, -6.0f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f,// top left

   // bottom right cube-positive z
    -8.0f, -8.0f, -5.8f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,// bottom left
   8.0f, -8.0f, -5.8f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,// bottom right
   8.0f, 8.0f, -5.8f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,// top right
  -8.0f, 8.0f, -5.8f, 1.0f, 1.0f, 0.0f,  0.0f, 1.0f// top left
    };
    unsigned int indices5[] = {
        5, 4, 0,
        1, 5, 0,

        6, 5, 1,
        2, 6, 1,

        7, 6, 2,
        3, 7, 2,

        4, 7, 3,
        0, 4, 3,

        6, 7, 4,
        5, 6, 4,

        1, 0, 3,
        2, 1, 3
    };

    glGenVertexArrays(1, &VAO5);
    glGenBuffers(1, &VBO5);
    glGenBuffers(1, &EBO5);
    glBindVertexArray(VAO5);

    glBindBuffer(GL_ARRAY_BUFFER, VBO5);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices5), vertices5, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO5);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices5), indices5, GL_STATIC_DRAW);

    // position attribute information
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute information
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 view = glm::mat4(1.0f); // view matrix initialization
   // setting the radius variable 
    float radius = 10.0f;
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        ourShader.use();

        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("viewPos", cameraPos);

        // light properties
        glm::vec3 lightColor;
        lightColor.x = 1.0f;
        lightColor.y = 1.0f;
        lightColor.z = 1.0f;
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        lightingShader.setVec3("light.ambient", ambientColor);
        lightingShader.setVec3("light.diffuse", diffuseColor);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        lightingShader.use();

        lightingShader.setVec3("light.ambient", ambientColor);
        lightingShader.setVec3("light.diffuse", diffuseColor);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("material.ambient", 0.1745f, 0.01175f, 0.01175f);
        lightingShader.setVec3("material.diffuse", 0.61424f, 0.04136f, 0.04136f);
        lightingShader.setVec3("material.specular", 0.727811f, 0.626959f, 0.626959f);
        lightingShader.setFloat("material.shininess", 76.8f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)1024 / (float)768, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
 
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);

        ourShader.use();
        //cube 1 

        if (rubyMat && randomRubyMat == 1) {
            lightingShader.use();
        }
        else {
            ourShader.use();
        }
        if (changeTexture) {
            applySecondTextureToCube1(texture);
        }
        else {
            applyTextureToCube1(texture);
        }

        glm::mat4 model1 = glm::mat4(1.0f);
        glm::mat4 projection1 = glm::mat4(1.0f);
        if (randomCubeNumber == 1) {
            model1 = glm::rotate(model1, (float)glfwGetTime(), glm::vec3(-45.0f, 45.0f, 0.0f));

            if (objectScale) {
                model1 = glm::scale(model1, glm::vec3(2.5f, 2.5f, 2.5f));
            }
            if (objectMoveToCenter) {
                model1 = glm::translate(model1, glm::vec3(0.75f, -0.75f, 0.0f));
            }
        }

        projection1 = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);

        unsigned int modelLoc1 = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc1 = glGetUniformLocation(ourShader.ID, "view");

        glUniformMatrix4fv(modelLoc1, 1, GL_FALSE, glm::value_ptr(model1));
        glUniformMatrix4fv(viewLoc1, 1, GL_FALSE, &view[0][0]);
        // Send the projection to the Shader
        ourShader.setMat4("projection", projection1);

        render();
        drawCube1();

        //cube 2
        ourShader.use();
        if (rubyMat && randomRubyMat == 2) {
            lightingShader.use();
        }
        else {
            ourShader.use();
        }
        if (changeTexture) {
            applySecondTextureToCube2(texture);
        }
        else {
            applyTextureToCube2(texture);
        }

        glm::mat4 model2 = glm::mat4(1.0f);
        glm::mat4 projection2 = glm::mat4(1.0f);

        if (randomCubeNumber == 2) {
            model2 = glm::rotate(model2, (float)glfwGetTime(), glm::vec3(-45.0f, -45.0f, 0.0f));

            if (objectScale) {
                model2 = glm::scale(model2, glm::vec3(2.5f, 2.5f, 2.5f));
            }
            if (objectMoveToCenter) {
                model2 = glm::translate(model2, glm::vec3(0.75f, 0.75f, 0.0f));
            }
        }

        projection2 = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);


        unsigned int modelLoc2 = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc2 = glGetUniformLocation(ourShader.ID, "view");

        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, &view[0][0]);
        // Send the projection to the Shader
        ourShader.setMat4("projection", projection2);
        drawCube2();

        ////cube 3
        ourShader.use();
        if (rubyMat && randomRubyMat == 3) {
            lightingShader.use();
        }
        else {
            ourShader.use();
        }
        if (changeTexture) {
            applySecondTextureToCube3(texture);
        }
        else {
            applyTextureToCube3(texture);
        }

        glm::mat4 model3 = glm::mat4(1.0f);
        glm::mat4 projection3 = glm::mat4(1.0f);

        if (randomCubeNumber == 3) {
            model3 = glm::rotate(model3, (float)glfwGetTime(), glm::vec3(45.0f, 45.0f, 0.0f));

            if (objectScale) {
                model3 = glm::scale(model3, glm::vec3(2.5f, 2.5f, 2.5f));
            }
            if (objectMoveToCenter) {
                model3 = glm::translate(model3, glm::vec3(-0.75f, -0.75f, 0.0f));
            }
        }
        projection3 = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);


        unsigned int modelLoc3 = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc3 = glGetUniformLocation(ourShader.ID, "view");

        glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model3));
        glUniformMatrix4fv(viewLoc3, 1, GL_FALSE, &view[0][0]);
        // Send the projection to the Shader
        ourShader.setMat4("projection", projection3);
        drawCube3();

        ////cube 4
        ourShader.use();
        if (rubyMat && randomRubyMat == 4) {
            lightingShader.use();
        }
        else {
            ourShader.use();
        }
        if (changeTexture) {
            applySecondTextureToCube4(texture);
        }
        else {
            applyTextureToCube4(texture);
        }

        glm::mat4 model4 = glm::mat4(1.0f);
        glm::mat4 projection4 = glm::mat4(1.0f);

        if (randomCubeNumber == 4) {
            model4 = glm::rotate(model4, (float)glfwGetTime(), glm::vec3(45.0f, -45.0f, 0.0f));

            if (objectScale) {
                model4 = glm::scale(model4, glm::vec3(2.5f, 2.5f, 2.5f));
            }
            if (objectMoveToCenter) {
                model4 = glm::translate(model4, glm::vec3(-0.75f, 0.75f, 0.0f));
            }
        }
        projection4 = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);


        unsigned int modelLoc4 = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc4 = glGetUniformLocation(ourShader.ID, "view");

        glUniformMatrix4fv(modelLoc4, 1, GL_FALSE, glm::value_ptr(model4));
        glUniformMatrix4fv(viewLoc4, 1, GL_FALSE, &view[0][0]);
        // Send the projection to the Shader
        ourShader.setMat4("projection", projection4);
        drawCube4();

        ourShader.use();
    
        glm::mat4 model7 = glm::mat4(1.0f);
        glm::mat4 projection7 = glm::mat4(1.0f);

        if (randomCubeNumber == 3) {
            model7 = glm::rotate(model7, (float)glfwGetTime(), glm::vec3(45.0f, 45.0f, 0.0f));

            if (objectScale) {
                model7 = glm::scale(model7, glm::vec3(2.5f, 2.5f, 2.5f));
            }
            if (objectMoveToCenter) {
                model7 = glm::translate(model7, glm::vec3(-0.75f, -0.75f, 0.0f));
            }
        }
        projection7 = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);


        unsigned int modelLoc7 = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc7 = glGetUniformLocation(ourShader.ID, "view");

        glUniformMatrix4fv(modelLoc7, 1, GL_FALSE, glm::value_ptr(model7));
        glUniformMatrix4fv(viewLoc7, 1, GL_FALSE, &view[0][0]);
        // Send the projection to the Shader
        ourShader.setMat4("projection", projection7);
        drawPlane();

        glm::mat4 model5 = glm::mat4(1.0f);
        glm::mat4 projection5 = glm::mat4(1.0f);
        projection5 = glm::perspective(glm::radians(45.0f), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
     

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection5);
        lightCubeShader.setMat4("view", view);
        model5 = glm::mat4(1.0f);
        model5 = glm::translate(model5, lightPos);
        model5 = glm::scale(model5, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model5);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // glfw: swap buffers
        glfwSwapBuffers(window);
        // gldw: poll for events
        glfwPollEvents();
    }

    return 0;
}

void render()
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv(GL_COLOR, 0, black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, texture);
}


// user input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (randomCubeNumber == -1) {
            randomCubeNumber = rand() % 4 + 1; //generates a random number from 1-4
            isCubeRotating = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (isCubeRotating) {       //Cube only scales if it is rotating
            objectScale = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (isCubeRotating) {       //Cube only moves to center if it is rotating
            objectMoveToCenter = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { //sets all values to default to remove all changes
        objectScale = false;
        objectMoveToCenter = false;
        randomCubeNumber = -1;
        isCubeRotating = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        changeTexture = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        changeTexture = false;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        rubyMat = true;
        randomRubyMat = rand() % 4 + 1; //generates a random number from 1-4
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        changeTexture = false;
    }
    // increate the camera speed using the deltaTime
    float cameraSpeed = 3 * deltaTime;
    // forward movement
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    // backward movement
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    // left movement
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // right movement
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: viewport to window adjustment
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//Draw the fist cube
void drawCube1()
{
    glBindVertexArray(VAO1); //Use the appropriate VAO
    glDrawElements(GL_TRIANGLES, NumVertices, GL_UNSIGNED_INT, 0);
}

//Draw the second cube
void drawCube2()
{
    glBindVertexArray(VAO2); //Use the appropriate VAO
    glDrawElements(GL_TRIANGLES, NumVertices, GL_UNSIGNED_INT, 0);
}

//Draw the third cube
void drawCube3()
{
    glBindVertexArray(VAO3); //Use the appropriate VAO
    glDrawElements(GL_TRIANGLES, NumVertices, GL_UNSIGNED_INT, 0);
}

//Draw the fourth cube
void drawCube4()
{
    glBindVertexArray(VAO4); //Use the appropriate VAO
    glDrawElements(GL_TRIANGLES, NumVertices, GL_UNSIGNED_INT, 0);
}

void drawPlane()
{
    glBindVertexArray(VAO5); //Use the appropriate VAO
    glDrawElements(GL_TRIANGLES, NumVertices, GL_UNSIGNED_INT, 0);
}
// glfw: callback called when the mouse moves
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // a ratio that define how sensitive the mouse movement will be
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void applyTextureToCube1(unsigned int& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // the texture object is applied with all the texture operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set GL_REPEAT as the wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image (mybox.png) and create the texture 
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/1.jpg", &width, &height, &nrChannels, 0);
    // Generate mipmaps
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void applyTextureToCube2(unsigned int& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // the texture object is applied with all the texture operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set GL_REPEAT as the wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image (mybox.png) and create the texture 
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/2.png", &width, &height, &nrChannels, 0);
    // Generate mipmaps
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
void applyTextureToCube3(unsigned int& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // the texture object is applied with all the texture operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set GL_REPEAT as the wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image (mybox.png) and create the texture 
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/3.jpg", &width, &height, &nrChannels, 0);
    // Generate mipmaps
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
void applyTextureToCube4(unsigned int& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // the texture object is applied with all the texture operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set GL_REPEAT as the wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image (mybox.png) and create the texture 
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/4.jpg", &width, &height, &nrChannels, 0);
    // Generate mipmaps
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
void applySecondTextureToCube1(unsigned int& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // the texture object is applied with all the texture operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set GL_REPEAT as the wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image (mybox.png) and create the texture 
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/5.jpg", &width, &height, &nrChannels, 0);
    // Generate mipmaps
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
void applySecondTextureToCube2(unsigned int& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // the texture object is applied with all the texture operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set GL_REPEAT as the wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image (mybox.png) and create the texture 
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/6.jpg", &width, &height, &nrChannels, 0);
    // Generate mipmaps
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
void applySecondTextureToCube3(unsigned int& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // the texture object is applied with all the texture operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set GL_REPEAT as the wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image (mybox.png) and create the texture 
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/7.jpg", &width, &height, &nrChannels, 0);
    // Generate mipmaps
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
void applySecondTextureToCube4(unsigned int& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // the texture object is applied with all the texture operations
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set GL_REPEAT as the wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image (mybox.png) and create the texture 
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/8.jpg", &width, &height, &nrChannels, 0);
    // Generate mipmaps
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
