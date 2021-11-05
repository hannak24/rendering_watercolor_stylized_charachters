#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "ImGuiFileBrowser.h"
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
 
#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

enum Shading { PHONG, TOON, GRID};
enum Primitives { CUBE, SPHERE, PYRAMID};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path, bool gammaCorrection);
void renderScene(Shader& shader, Model ourModel, Shader& modelShader, int wall_texture);
unsigned int loadCubemap(vector<std::string> faces);
void renderQuad();

//GUI functions
static void ShowExampleMenuFile();
void ImGui::ShowDemoWindow(bool* p_open);
static void ShowExampleAppMainMenuBar();
static void ShowObjectMenu();
static void ShowObjectTextureMenu();
static void ShowMaterialMenu();
static void ShowTransformationsMenu();
static void ShowCubeBoxMenu();
static void ShowCamerasMenu();
static void ShowLightsMenu();
static void ShowSettingsMenu();

bool show_app_main_menu_bar = true;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 2.1999;
float turbulance = 3.5349;
float density = 0.4438;
float bleed = 0.1977;
float light = 0.877;
float darkEdge = 0.0677;
float granulation = 0.5129;
float finalTremor = 0.0080;
bool wall = false;

// camera
Camera camera(glm::vec3(0.0f, 1.268f, 5.046f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// meshes
unsigned int planeVAO;
unsigned int wallVAO;

// model
float modelYawAngle = 87.4999f;
float modelRollAngle = 0.0f;
float modelPitchAngle = -6.2999f;
float modelScale = 1.0f;

//GUI
bool modelEnabled = true;
char defaultModel[128] = "objects/Penguin/PenguinBaseMesh.obj";
string previousModel= "objects/Penguin/PenguinBaseMesh.obj";
int shadowing = PHONG;
int primitive = SPHERE;
char diffuseTextureObject[128] = "";
char diffuseTextureWall[128] = "";
char diffuseTextureFloor[128] = "";
char normalMapObject[128] = "";
char heightMapObject[128] = "";
bool textureObjectEnabled = true;
bool textureFloorEnabled = true;
bool textureWallEnabled = true;
bool normalMapObjectEnabled = false;
bool heightMapObjectEnabled = false;
int heightLevelsObject = 7;
bool watercolorEnabled = true;
float Shininess = 64.0f;
bool materialEnabled = false;
float materialAmbient[3] = { 1.0f, 0.5f, 0.31f };
float materialDiffuse[3] = { 1.0f, 0.5f, 0.31f };
float materialSpecular[3] = { 0.5f, 0.5f, 0.5f };
float translateX = 0.0f;
float translateY = 0.0f;
float translateZ = 0.0f;
float scaleX = 2.2281;
float scaleY = 2.2281;
float scaleZ = 2.2281;
bool cubeBoxEnabled = false;
char cubeBoxRight[128] = "skyboxes/watercolor paper/right.jpg";
char cubeBoxLeft[128] = "skyboxes/watercolor paper/left.jpg";
char cubeBoxTop[128] = "skyboxes/watercolor paper/top.jpg";
char cubeBoxBottom[128] = "skyboxes/watercolor paper/bottom.jpg";
char cubeBoxFront[128] = "skyboxes/watercolor paper/front.jpg";
char cubeBoxBack[128] = "skyboxes/watercolor paper/back.jpg";
bool wallEnabled = false;
bool floorEnabled = false;
float cameraRight = 0.0;
float cameraLeft = 0.0;
float cameraUp = 0.0;
float cameraDown = 0.0;
float cameraForward = 0.0;
float cameraBackward = 0.0;
bool waterEnabled = false;
bool dirLightEnabled = true;
float dirLightDirectionX = 0.0;
float dirLightDirectionY = 0.0;
float dirLightDirectionZ = 0.0;
float dirLightAmbient[3] = { 1.0, 1.0, 1.0 };
float dirLightDiffuse[3] = { 1.0f, 1.0f, 1.0f };
float dirLightSpecular[3] = { 0.5 , 0.5 , 0.5};
float dirLightCangiante = 0.7;
float dirLightDilution = 0.99;
bool keyBoardEnabled = true;
bool mouseCaptureEnabled = false;
float yawIncrease = 0.7;
float yawDecrease = 0.7;
float pitchIncrease = 0.7;
float pitchDecrease = 0.7;
float rollIncrease = 0.7;
float rollDecrease = 0.7;
float scaleIncrease = 1.01;
float scaleDecrease = 0.99;
float turbulanceIncrease = 1.01;
float turbulanceDecrease = 0.99;
float densityIncrease = 1.005;
float densityDecrease = 0.99;
float bleedIncrease = 1.005;
float bleedDecrease = 0.99;
float lightIncrease = 1.005;
float lightDecrease = 0.99;
float darkEdgeIncrease = 1.005;
float darkEdgeDecrease = 0.99;
float granulationIncrease = 1.005;
float granulationDecrease = 0.99;
float tremorIncrease = 1.005;
float tremorDecrease = 0.99;
bool spotLightEnabled = false;
float spotLightPositionX = 0.0;
float spotLightPositionY = 0.0;
float spotLightPositionZ = 0.0;
float spotLightAmbient[3] = { 0.2, 0.2, 0.2};
float spotLightDiffuse[3] = { 1.0, 1.0, 1.0};
float spotLightSpecular[3] = { 0.5, 0.5, 0.5};
float spotLightCangiante = 0.7;
float spotLightDilution = 0.8;
float spotLightCutoff = glm::cos(glm::radians(12.5f));
float spotLightOuterCutoff = glm::cos(glm::radians(15.0f));
float spotLightConstant = 1.0;
float spotLightLinear = 0.09;
float spotLightQuadratic = 0.032;
bool  pointLight0Enabled = true;
float pointLight0PositionX = 0.0;
float pointLight0PositionY = 0.5;
float pointLight0PositionZ = 1.5;
float pointLight0Ambient[3] = {0.05, 0.05, 0.05 };
float pointLight0Diffuse[3] = { 0.8, 0.8, 0.8};
float pointLight0Specular[3] = { 1.0, 1.0, 1.0 };
float pointLight0Cangiante = 0.7;
float pointLight0Dilution = 0.3;
float pointLight0Constant = 1.0;
float pointLight0Linear = 0.09;
float pointLight0Quadratic = 0.032;
bool  pointLight1Enabled = true;
float pointLight1PositionX = -4.0;
float pointLight1PositionY = 0.5;
float pointLight1PositionZ = -3.0;
float pointLight1Ambient[3] = {0.05, 0.05, 0.05 };
float pointLight1Diffuse[3] = { 0.8, 0.8, 0.8 };
float pointLight1Specular[3] = { 1.0, 1.0, 1.0 };
float pointLight1Cangiante = 0.7;
float pointLight1Dilution = 0.6;
float pointLight1Constant = 1.0;
float pointLight1Linear = 0.09;
float pointLight1Quadratic = 0.032;
bool  pointLight2Enabled = true;
float pointLight2PositionX = 3.0;
float pointLight2PositionY = 0.5;
float pointLight2PositionZ = 1.0;
float pointLight2Ambient[3] = {0.05, 0.05, 0.05 };
float pointLight2Diffuse[3] = { 0.8, 0.8, 0.8 };
float pointLight2Specular[3] = { 1.0, 1.0, 1.0 };
float pointLight2Cangiante = 0.7;
float pointLight2Dilution = 0.3;
float pointLight2Constant = 1.0;
float pointLight2Linear = 0.09;
float pointLight2Quadratic = 0.032;
bool  pointLight3Enabled = true;
float pointLight3PositionX = -0.8;
float pointLight3PositionY = 2.4;
float pointLight3PositionZ = -1.0;
float pointLight3Ambient[3] = { 0.05, 0.05, 0.05 };
float pointLight3Diffuse[3] = { 0.8, 0.8, 0.8 };
float pointLight3Specular[3] = { 1.0, 1.0, 1.0 };
float pointLight3Cangiante = 0.7;
float pointLight3Dilution = 0.3;
float pointLight3Constant = 1.0;
float pointLight3Linear = 0.09;
float pointLight3Quadratic = 0.032;
float pointLight0RelativeWeight = 0.15f;
float pointLight1RelativeWeight = 0.15f;
float pointLight2RelativeWeight = 0.15f;
float pointLight3RelativeWeight = 0.15f;
float cameraPositionX = 0.0;
float cameraPositionY = 0.0;
float cameraPositionZ = 0.0;
float wallTranslateX = 0.0;
float wallTranslateY = 0.0;
float wallTranslateZ = 0.0;
float floorTranslateX = 0.0;
float floorTranslateY = 0.0;
float floorTranslateZ = 0.0;
bool show_open_dialog = false;




int main()
{
    // glfw: initialize and configure
    // ------------------------------
    const char* glsl_version = "#version 330";
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 9);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Watercolor application", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, 1);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();


   // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
   //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // build and compile shaders
    // -------------------------
    Shader shaderBlur("blurVShader.glsl", "paper_blurFShader.glsl");
    Shader shaderBloomFinal("bloom_finalVShader.glsl", "bloom_finalFShader.glsl");
    Shader shader("normalVShader.glsl", "normalFShader.glsl");
    Shader simpleDepthShader("shadeVShader.glsl", "shadeFShader.glsl");
    Shader skyboxShader("loadSkyBoxVshader.glsl", "loadSkyBoxFshader.glsl");

    // load models
    // -----------
    Model ourModel("objects/Penguin/PenguinBaseMesh.obj");
    //Model ourModel("objects/Duck/Duck.obj");
    //Model ourModel("objects/Goldfish/Goldfish.obj");
    //Model ourModel("objects/Swan/Swan.obj");
    //Model ourModel("objects/dolphin/dolphin.obj");
    //Model ourModel("objects/Duck/Duck.obj");
    //Model ourModel(defaultModel);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float planeVertices[] = {
        // positions            // normals         // texcoords
         50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
        -50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,

         50.0f, -0.5f,  50.0f,  0.0f, 1.0f, 0.0f,  50.0f,  0.0f,
        -50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,   0.0f, 50.0f,
         50.0f, -0.5f, -50.0f,  0.0f, 1.0f, 0.0f,  50.0f, 50.0f
    };

    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    float wallVertices[] = {
        // positions            // normals         // texcoords
         50.0f, -0.5f,  -50.0f,  0.0f, 0.0f, 1.0f,  50.0f,  0.0f,
        -50.0f, -0.5f,  -50.0f,  0.0f, 0.0f, 1.0f,   0.0f,  0.0f,
         -50.0f, 50.0f, -50.0f,  0.0f, 0.0f, 1.0f,   0.0f, 50.0f,

         50.0f, -0.5f,  -50.0f,  0.0f, 0.0f, 1.0f,  50.0f,  0.0f,
        -50.0f, 50.0f, -50.0f,  0.0f, 0.0f, 1.0f,   0.0f, 50.0f,
         50.0f, 50.0f, -50.0f,  0.0f, 0.0f, 1.0f,  50.0f, 50.0f
    };

    // wall VAO
    unsigned int wallVBO;
    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glBindVertexArray(wallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    unsigned int skyboxVBO, skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    vector<std::string> faces =
    {
            cubeBoxRight,
            cubeBoxLeft,
            cubeBoxTop,
            cubeBoxBottom,
            cubeBoxFront,
            cubeBoxBack
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // load textures
    // -------------
    unsigned int containerTexture = loadTexture("textures/container2.png", true); // note that we're loading the texture as an SRGB texture
    unsigned int waterColorPaperTexture = loadTexture("textures/watercolor paper.jpg", true);
    unsigned int paperHeightTexture = loadTexture("dicplacementMap/disp.png", true);
    unsigned int paperTexture = loadTexture("dicplacementMap/disp.png", true);
    

   

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure (floating point) framebuffers
    // ---------------------------------------
    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    unsigned int colorBuffers[3];
    glGenTextures(3, colorBuffers);
    for (unsigned int i = 0; i < 3; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    // lighting info
    // -------------
    // positions
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    std::vector<glm::vec3> lightPositions;
    lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
    lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
    lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
    lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
    // colors
    std::vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
    lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
    lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
    lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));


    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 1);
    shader.setInt("noise_texture", 3);
    shaderBlur.use();
    shaderBlur.setInt("image", 0);
    shaderBloomFinal.use();
    shaderBloomFinal.setInt("scene", 0);
    shaderBloomFinal.setInt("bloomBlur", 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //initialisations
        /*camera.Position.x += cameraRight - cameraLeft;
        camera.Position.y += cameraUp - cameraDown;
        camera.Position.z += cameraForward - cameraBackward;*/
        cameraPositionX = camera.Position.x;
        cameraPositionY = camera.Position.y;
        cameraPositionZ = camera.Position.z;
        /*cameraRight = 0;
        cameraLeft = 0;
        cameraUp = 0;
        cameraDown = 0;
        cameraForward = 0;
        cameraBackward = 0;*/
        spotLightPositionX = camera.Position.x;
        spotLightPositionY = camera.Position.y;
        spotLightPositionZ = camera.Position.z;

        //update textures
        unsigned int wall_texture = 0;
        unsigned int floor_texture = 0;
        if(wallEnabled)
            unsigned int wall_texture = loadTexture(diffuseTextureWall, true);
        if(floorEnabled)
            unsigned int floor_texture = loadTexture("textures/watercolor paper.jpg", true);
        if (strcmp(previousModel.c_str(), defaultModel) != 0) {
            ourModel = Model(defaultModel);
            previousModel = defaultModel;
        }
        

        // 1. render depth of scene to texture (from light's perspective)
        // --------------------------------------------------------------
        glCullFace(GL_FRONT);
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterColorPaperTexture);

        simpleDepthShader.use();
        // floor
        glm::mat4 model = glm::mat4(1.0f);
        renderScene(simpleDepthShader, ourModel, simpleDepthShader, wall_texture);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCullFace(GL_BACK);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. render scene as normal using the generated depth/shadow map  
        // --------------------------------------------------------------
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shader.setBool("water", waterEnabled);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterColorPaperTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, wall_texture);

        shader.use();
        renderScene(shader, ourModel, shader, wall_texture);

        //add skybox
        if (cubeBoxEnabled) {
            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            glDepthMask(GL_FALSE);
            skyboxShader.use();

            // view/projection transformations
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
            skyboxShader.setMat4("projection", projection);
            skyboxShader.setMat4("view", view);

            // render the loaded model
            model = glm::mat4(1.0f);
            skyboxShader.setMat4("model", model);

            glBindVertexArray(skyboxVAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthMask(GL_TRUE);
        }

        
        // 3. blur bright fragments with two-pass Gaussian Blur 
        // --------------------------------------------------
        bool horizontal = true, first_iteration = true;
        shaderBlur.use();
        for (unsigned int i = 0; i < 42; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            shaderBlur.setInt("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, colorBuffers[2]);
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
       
        //// 4. now render floating point color buffer to 2D quad 
        //--------------------------------------------------------------------------------------------------------------------------

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderBloomFinal.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, waterColorPaperTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, paperHeightTexture);
       
        shaderBloomFinal.setInt("paper", 2);
        shaderBloomFinal.setInt("texture_height", 3);
        shaderBloomFinal.setInt("bloom", bloom);
        shaderBloomFinal.setFloat("exposure", exposure);
        shaderBloomFinal.setFloat("bleed", bleed);
        shaderBloomFinal.setFloat("darkEdge", darkEdge);
        shaderBloomFinal.setFloat("granulation", granulation);
        shaderBloomFinal.setFloat("finalTremor", finalTremor);
        shaderBloomFinal.setVec3("viewPos", camera.Position);
        shaderBloomFinal.setFloat("height_scale", 0.1f);
        renderQuad();


        // render GUI
        // Menu
        ShowExampleAppMainMenuBar();
        
        
        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
    {
        bloom = !bloom;
        bloomKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        bloomKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        if (exposure > 0.0f)
            exposure -= 0.001f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        exposure += 0.001f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        modelPitchAngle = modelPitchAngle - 0.7;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        modelPitchAngle = modelPitchAngle + 0.7;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        modelYawAngle = modelYawAngle + 0.7;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        modelYawAngle = modelYawAngle - 0.7;
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        modelRollAngle = modelRollAngle + 0.7;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        modelRollAngle = modelRollAngle - 0.7;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        modelScale = modelScale * 1.01;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        modelScale = modelScale * 0.99;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        turbulance = turbulance * 1.01;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        turbulance = turbulance * 0.99;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        density = density * 0.99;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        density = density * 1.005;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        bleed = bleed * 0.99;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        bleed = bleed * 1.005;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        light = light * 0.99;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        light = light * 1.005;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        darkEdge = darkEdge * 0.99;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        darkEdge = darkEdge * 1.005;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        granulation = granulation * 0.99;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        granulation = granulation * 1.005;
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        finalTremor = finalTremor * 0.99;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        finalTremor = finalTremor * 1.005;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// renders the 3D scene
// --------------------
void renderScene(Shader& shader, Model ourModel, Shader& modelShader, int wall_texture)
{
    shader.use();

    // floor
    shader.setFloat("wallFlag", 0.0f);
    shader.setFloat("normalFlag", 0.0f);
    shader.setFloat("parralaxFlag", 0.0f);
    shader.setFloat("height_scale", 0.0f);
    shader.setFloat("diluteAreaVariable", 1);
    shader.setFloat("modelFlag", 0.0f);
    shader.setFloat("tremor", 0.0f);
    shader.setInt("noise_texture", 3);
    glm::mat4 model = glm::mat4(1.0f);
    shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    if (floorEnabled) {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    //wall
    if (wallEnabled) {
        shader.setFloat("normalFlag", 0.0f);
        shader.setFloat("parralaxFlag", 0.0f);
        shader.setFloat("height_scale", 0.0f);
        shader.setFloat("wallFlag", 1.0f);
        shader.setFloat("diluteAreaVariable", 1);
        shader.setFloat("modelFlag", 0.0f);
        shader.setFloat("tremor", 0.0f);
        shader.setInt("noise_texture", 3);
        shader.setInt("wall_texture", 0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(wallTranslateX, wallTranslateY, wallTranslateZ)); // translate it down so it's at the center of the scene
        shader.setMat4("wallModel", model);
        model = glm::mat4(1.0);
        shader.setVec3("dirLight.ambient", light * 0.2f, light * 0.2f, light * 0.2f);
        shader.setMat4("model", model);
        glBindVertexArray(wallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    //model

    shader.setVec3("cameraPos", camera.Position);
    shader.setFloat("normalFlag", normalMapObjectEnabled);
    shader.setFloat("parralaxFlag", heightMapObjectEnabled);
    shader.setFloat("modelFlag", 1.0f);
    shader.setFloat("toonShading", shadowing);
    shader.setFloat("height_scale", heightLevelsObject);
    shader.setFloat("time", glfwGetTime());
    shader.setFloat("frequency", 1 / deltaTime);
    shader.setFloat("speed", deltaTime);
    shader.setFloat("tremor", 0.000f);
    shader.setFloat("diluteAreaVariable", 1);
    shader.setInt("skybox", cubeBoxEnabled);
    shader.setInt("noise_texture", 3);
    shader.setFloat("density", density);
    shader.setFloat("turbulance", turbulance);
    model = glm::rotate(model, glm::radians(modelYawAngle), glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, glm::radians(modelRollAngle), glm::vec3(0.0, 0.0, 1.0));
    model = glm::rotate(model, glm::radians(modelPitchAngle), glm::vec3(1.0, 0.0, 0.0));
    model = glm::translate(model, glm::vec3(translateX, translateY, translateZ)); // translate it down so it's at the center of the scene
    model = glm::scale(model, modelScale * glm::vec3(scaleX, scaleY, scaleZ));	// it's a bit too big for our scene, so scale it down
    shader.setMat4("model", model);
    shader.setVec3("dirLight.ambient", light * dirLightAmbient[0], light * dirLightAmbient[1], light * dirLightAmbient[2]);
    shader.setVec3("dirLight.diffuse", light * dirLightDiffuse[0], light * dirLightDiffuse[1], light * dirLightDiffuse[2]);
    shader.setVec3("dirLight.specular", light * dirLightSpecular[0], light * dirLightSpecular[1], light * dirLightSpecular[2]);
    shader.setFloat("dirLight.cangiante", dirLightCangiante);
    shader.setFloat("dirLight.dilution", dirLightDilution);
    shader.setFloat("dirLight.enable", dirLightEnabled);
    // point light 1
    shader.setVec3("pointLights[0].position", glm::vec3(pointLight0PositionX, pointLight0PositionY, pointLight0PositionZ));
    shader.setVec3("pointLights[0].ambient", pointLight0Ambient[0], pointLight0Ambient[1], pointLight0Ambient[2]);
    shader.setVec3("pointLights[0].diffuse", pointLight0Diffuse[0], pointLight0Diffuse[1], pointLight0Diffuse[2]);
    shader.setVec3("pointLights[0].specular", pointLight0Specular[0], pointLight0Specular[1], pointLight0Specular[2]);
    shader.setFloat("pointLights[0].constant", pointLight0Constant);
    shader.setFloat("pointLights[0].linear", pointLight0Linear);
    shader.setFloat("pointLights[0].quadratic", pointLight0Quadratic);
    shader.setFloat("pointLights[0].cangiante", pointLight0Cangiante);
    shader.setFloat("pointLights[0].dilution", pointLight0Dilution);
    // point light 2
    shader.setVec3("pointLights[1].position", glm::vec3(pointLight1PositionX, pointLight1PositionY, pointLight1PositionZ));
    shader.setVec3("pointLights[1].ambient", pointLight1Ambient[0], pointLight1Ambient[1], pointLight1Ambient[2]);
    shader.setVec3("pointLights[1].diffuse", pointLight1Diffuse[0], pointLight1Diffuse[1], pointLight1Diffuse[2]);
    shader.setVec3("pointLights[1].specular", pointLight1Specular[0], pointLight1Specular[1], pointLight1Specular[2]);
    shader.setFloat("pointLights[1].constant", pointLight1Constant);
    shader.setFloat("pointLights[1].linear", pointLight1Linear);
    shader.setFloat("pointLights[1].quadratic", pointLight1Quadratic);
    shader.setFloat("pointLights[1].cangiante", pointLight1Cangiante);
    shader.setFloat("pointLights[1].dilution", pointLight1Dilution);
    // point light 3
    shader.setVec3("pointLights[2].position", glm::vec3(pointLight2PositionX, pointLight2PositionY, pointLight2PositionZ));
    shader.setVec3("pointLights[2].ambient", pointLight2Ambient[0], pointLight2Ambient[1], pointLight2Ambient[2]);
    shader.setVec3("pointLights[2].diffuse", pointLight2Diffuse[0], pointLight2Diffuse[1], pointLight2Diffuse[2]);
    shader.setVec3("pointLights[2].specular", pointLight2Specular[0], pointLight2Specular[1], pointLight2Specular[2]);
    shader.setFloat("pointLights[2].constant", pointLight2Constant);
    shader.setFloat("pointLights[2].linear", pointLight2Linear);
    shader.setFloat("pointLights[2].quadratic", pointLight2Quadratic);
    shader.setFloat("pointLights[2].cangiante", pointLight2Cangiante);
    shader.setFloat("pointLights[2].dilution", pointLight2Dilution);
    // point light 4
    shader.setVec3("pointLights[3].position", glm::vec3(pointLight3PositionX, pointLight3PositionY, pointLight3PositionZ));
    shader.setVec3("pointLights[3].ambient", pointLight3Ambient[0], pointLight3Ambient[1], pointLight3Ambient[2]);
    shader.setVec3("pointLights[3].diffuse", pointLight3Diffuse[0], pointLight3Diffuse[1], pointLight3Diffuse[2]);
    shader.setVec3("pointLights[3].specular", pointLight3Specular[0], pointLight3Specular[1], pointLight3Specular[2]);
    shader.setFloat("pointLights[3].constant", pointLight3Constant);
    shader.setFloat("pointLights[3].linear", pointLight3Linear);
    shader.setFloat("pointLights[3].quadratic", pointLight3Quadratic);
    shader.setFloat("pointLights[3].cangiante", pointLight3Cangiante);
    shader.setFloat("pointLights[3].dilution", pointLight3Dilution);
    shader.setVec4("pointLightsEnable", glm::vec4(pointLight0Enabled, pointLight1Enabled, pointLight2Enabled, pointLight3Enabled));
    shader.setVec4("pointLightsWeights", glm::vec4(pointLight0RelativeWeight, pointLight1RelativeWeight, pointLight2RelativeWeight, pointLight3RelativeWeight));
    //// spotLight
    shader.setVec3("spotLight.position", glm::vec3(spotLightPositionX, spotLightPositionY, spotLightPositionZ));
    shader.setVec3("spotLight.direction", camera.Front);
    shader.setVec3("spotLight.ambient", spotLightAmbient[0], spotLightAmbient[1], spotLightAmbient[2]);
    shader.setVec3("spotLight.diffuse", spotLightDiffuse[0], spotLightDiffuse[1], spotLightDiffuse[2]);
    shader.setVec3("spotLight.specular", spotLightSpecular[0], spotLightSpecular[1], spotLightSpecular[2]);
    shader.setFloat("spotLight.constant", spotLightConstant);
    shader.setFloat("spotLight.linear", spotLightLinear);
    shader.setFloat("spotLight.quadratic", spotLightQuadratic);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotLightCutoff)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLightOuterCutoff)));
    shader.setFloat("spotLight.cangiante", spotLightConstant);
    shader.setFloat("spotLight.dilution", spotLightDilution);
    shader.setFloat("spotLight.enable", spotLightEnabled);
    shader.setFloat("material.shininess", 64.0f);
    ourModel.Draw(shader);

}


static void ShowExampleMenuFile()
{
    ImGui::MenuItem("(demo menu)", NULL, false, false);
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            if (ImGui::BeginMenu("Recurse.."))
            {
                ShowExampleMenuFile();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }
}


static void ShowObjectMenu()
{    
    if (ImGui::BeginMenu("texture"))
    {
        ShowObjectTextureMenu();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("material"))
    {
        ShowMaterialMenu();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("transformations"))
    {
        ShowTransformationsMenu();
        ImGui::EndMenu();
    }
}

static void ShowFloorMenu()
{
    if (ImGui::BeginMenu("material"))
    {
        ShowMaterialMenu();
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("transformations"))
    {
        ShowTransformationsMenu();
        ImGui::EndMenu();
    }
}

static void ShowObjectTextureMenu()
{   
    ImGui::Checkbox("texture enabled", &textureObjectEnabled);
    ImGui::SameLine(); ImGui::InputText("diffuse texture", diffuseTextureObject, IM_ARRAYSIZE(diffuseTextureObject));
    ImGui::Checkbox("normal map enabeled", &normalMapObjectEnabled);
    ImGui::SameLine(); ImGui::InputText("normal map", normalMapObject, IM_ARRAYSIZE(normalMapObject));
    ImGui::Checkbox("height map enabled", &heightMapObjectEnabled);
    ImGui::SameLine(); ImGui::InputText("height map", heightMapObject, IM_ARRAYSIZE(heightMapObject));
    ImGui::InputInt("height levels", &heightLevelsObject, 1.0f);
}


static void ShowMaterialMenu()
{
    ImGui::Checkbox("material enabled", &materialEnabled);
    ImGui::ColorEdit3("ambient color", materialAmbient); 
    ImGui::ColorEdit3("diffuse color", materialDiffuse);
    ImGui::ColorEdit3("specular color", materialSpecular);
    ImGui::InputFloat("Shininess", &Shininess, 5.0f);
}

static void ShowTransformationsMenu()
{
    if (ImGui::BeginMenu("translate"))
    {
        ImGui::InputFloat("x", &translateZ, 0.1f); 
        ImGui::InputFloat("y", &translateY, 0.1f); 
        ImGui::InputFloat("z", &translateX, 0.1f);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("scale"))
    {
        ImGui::InputFloat("x", &scaleX, 1.0f);
        ImGui::InputFloat("y", &scaleY, 1.0f);
        ImGui::InputFloat("z", &scaleZ, 1.0f);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("rotate"))
    {
        ImGui::InputFloat("yaw", &modelYawAngle, 1.0f);
        ImGui::InputFloat("pitch", &modelPitchAngle, 1.0f);
        ImGui::InputFloat("roll", &modelRollAngle, 1.0f);
        ImGui::EndMenu();
    }
}

static void ShowCubeBoxMenu()
{
    ImGui::Checkbox("CubeBox enabled", &cubeBoxEnabled);
    ImGui::InputText("right", cubeBoxRight, IM_ARRAYSIZE(cubeBoxRight));
    ImGui::InputText("left", cubeBoxLeft, IM_ARRAYSIZE(cubeBoxLeft));
    ImGui::InputText("back", cubeBoxBack, IM_ARRAYSIZE(cubeBoxBack));
    ImGui::InputText("front", cubeBoxFront, IM_ARRAYSIZE(cubeBoxFront));
    ImGui::InputText("top", cubeBoxTop, IM_ARRAYSIZE(cubeBoxTop));
    ImGui::InputText("bottom", cubeBoxBottom, IM_ARRAYSIZE(cubeBoxBottom));
}

static void ShowCamerasMenu()
{
    if (ImGui::BeginMenu("position"))
    {
        ImGui::InputFloat("x", &cameraPositionX, 1.0f);
        ImGui::InputFloat("y", &cameraPositionY, 1.0f);
        ImGui::InputFloat("z", &cameraPositionZ, 1.0f);
        ImGui::EndMenu();
    }
    ImGui::InputFloat("move right (key D)", &cameraRight, deltaTime);
    ImGui::InputFloat("move left (key A)", &cameraLeft, deltaTime);
    ImGui::InputFloat("move up (key W)", &cameraUp, deltaTime);
    ImGui::InputFloat("move down (key S)", &cameraDown, deltaTime);
    ImGui::InputFloat("move forward (key E)", &cameraForward, deltaTime);
    ImGui::InputFloat("move backward (key Q)", &cameraBackward, deltaTime);
}

static void ShowLightsMenu()
{
    if (ImGui::BeginMenu("direct light"))
    {
        ImGui::Checkbox("direct light enabled", &dirLightEnabled);
        if (ImGui::BeginMenu("direction"))
        {
            ImGui::InputFloat("x", &dirLightDirectionX, 1.0f);
            ImGui::InputFloat("y", &dirLightDirectionY, 1.0f);
            ImGui::InputFloat("z", &dirLightDirectionZ, 1.0f);
            ImGui::EndMenu();
        }
        ImGui::ColorEdit3("ambient color", dirLightAmbient);
        ImGui::ColorEdit3("diffuse color", dirLightDiffuse);
        ImGui::ColorEdit3("specular color", dirLightSpecular);
        ImGui::InputFloat("cangiante", &dirLightCangiante, 0.01f);
        ImGui::InputFloat("dilution", &dirLightDilution, 0.01f);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("point lights"))
    {
        if (ImGui::BeginMenu("point light 0"))
        {
            ImGui::Checkbox("point light 0 enabled", &pointLight0Enabled);
            ImGui::InputFloat("relative weight", &pointLight0RelativeWeight, 0.05f);
            if (ImGui::BeginMenu("position"))
            {
                ImGui::InputFloat("x", &pointLight0PositionX, 1.0f);
                ImGui::InputFloat("y", &pointLight0PositionY, 1.0f);
                ImGui::InputFloat("z", &pointLight0PositionZ, 1.0f);
                ImGui::EndMenu();
            }
            ImGui::ColorEdit3("ambient color", pointLight0Ambient);
            ImGui::ColorEdit3("diffuse color", pointLight0Diffuse);
            ImGui::ColorEdit3("specular color", pointLight0Specular);
            ImGui::InputFloat("cangiante", &pointLight0Cangiante, 0.01f);
            ImGui::InputFloat("dilution", &pointLight0Dilution, 0.01f);
            ImGui::InputFloat("constant", &pointLight0Constant, 0.01f);
            ImGui::InputFloat("linear", &pointLight0Linear, 0.01f);
            ImGui::InputFloat("quadratic", &pointLight0Quadratic, 0.01f);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("point light 1"))
        {
            ImGui::Checkbox("point light 1 enabled", &pointLight1Enabled);
            ImGui::InputFloat("relative weight", &pointLight1RelativeWeight, 0.05f);
            if (ImGui::BeginMenu("position"))
            {
                ImGui::InputFloat("x", &pointLight1PositionX, 1.0f);
                ImGui::InputFloat("y", &pointLight1PositionY, 1.0f);
                ImGui::InputFloat("z", &pointLight1PositionZ, 1.0f);
                ImGui::EndMenu();
            }
            ImGui::ColorEdit3("ambient color", pointLight1Ambient);
            ImGui::ColorEdit3("diffuse color", pointLight1Diffuse);
            ImGui::ColorEdit3("specular color", pointLight1Specular);
            ImGui::InputFloat("cangiante", &pointLight1Cangiante, 0.01f);
            ImGui::InputFloat("dilution", &pointLight1Dilution, 0.01f);
            ImGui::InputFloat("constant", &pointLight1Constant, 0.01f);
            ImGui::InputFloat("linear", &pointLight1Linear, 0.01f);
            ImGui::InputFloat("quadratic", &pointLight1Quadratic, 0.01f);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("point light 2"))
        {
            ImGui::Checkbox("point light 2 enabled", &pointLight2Enabled);
            ImGui::InputFloat("relative weight", &pointLight2RelativeWeight, 0.05f);
            if (ImGui::BeginMenu("position"))
            {
                ImGui::InputFloat("x", &pointLight2PositionX, 1.0f);
                ImGui::InputFloat("y", &pointLight2PositionY, 1.0f);
                ImGui::InputFloat("z", &pointLight2PositionZ, 1.0f);
                ImGui::EndMenu();
            }
            ImGui::ColorEdit3("ambient color", pointLight2Ambient);
            ImGui::ColorEdit3("diffuse color", pointLight2Diffuse);
            ImGui::ColorEdit3("specular color", pointLight2Specular);
            ImGui::InputFloat("cangiante", &pointLight2Cangiante, 0.01f);
            ImGui::InputFloat("dilution", &pointLight2Dilution, 0.01f);
            ImGui::InputFloat("constant", &pointLight2Constant, 0.01f);
            ImGui::InputFloat("linear", &pointLight2Linear, 0.01f);
            ImGui::InputFloat("quadratic", &pointLight2Quadratic, 0.01f);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("point light 3"))
        {
            ImGui::Checkbox("point light 3 enabled", &pointLight3Enabled);
            ImGui::InputFloat("relative weight", &pointLight3RelativeWeight, 0.05f);
            if (ImGui::BeginMenu("position"))
            {
                ImGui::InputFloat("x", &pointLight3PositionX, 1.0f);
                ImGui::InputFloat("y", &pointLight3PositionY, 1.0f);
                ImGui::InputFloat("z", &pointLight3PositionZ, 1.0f);
                ImGui::EndMenu();
            }
            ImGui::ColorEdit3("ambient color", pointLight3Ambient);
            ImGui::ColorEdit3("diffuse color", pointLight3Diffuse);
            ImGui::ColorEdit3("specular color", pointLight3Specular);
            ImGui::InputFloat("cangiante", &pointLight3Cangiante, 0.01f);
            ImGui::InputFloat("dilution", &pointLight3Dilution, 0.01f);
            ImGui::InputFloat("constant", &pointLight3Constant, 0.01f);
            ImGui::InputFloat("linear", &pointLight3Linear, 0.01f);
            ImGui::InputFloat("quadratic", &pointLight3Quadratic, 0.01f);
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("spot light")){
        ImGui::Checkbox("spot light enabled", &spotLightEnabled);
        if (ImGui::BeginMenu("position"))
        {
            ImGui::InputFloat("x", &spotLightPositionX, 1.0f);
            ImGui::InputFloat("y", &spotLightPositionY, 1.0f);
            ImGui::InputFloat("z", &spotLightPositionZ, 1.0f);
            ImGui::EndMenu();
        }
        ImGui::ColorEdit3("ambient color", spotLightAmbient);
        ImGui::ColorEdit3("diffuse color", spotLightDiffuse);
        ImGui::ColorEdit3("specular color", spotLightSpecular);
        ImGui::InputFloat("cangiante", &spotLightCangiante, 0.01f);
        ImGui::InputFloat("dilution", &spotLightDilution, 0.01f);
        ImGui::InputFloat("cutoff", &spotLightCutoff, 0.01f);
        ImGui::InputFloat("outer cutoff", &spotLightOuterCutoff, 0.01f);
        ImGui::InputFloat("constant", &spotLightConstant, 0.01f);
        ImGui::InputFloat("linear", &spotLightLinear, 0.01f);
        ImGui::InputFloat("quadratic", &spotLightQuadratic, 0.01f);
        ImGui::EndMenu();
    }
}

static void ShowWatercolorMenu()
{
    ImGui::Checkbox("Watercolor filter enabled", &watercolorEnabled);
    ImGui::InputFloat("turbulance", &turbulance, 0.01f);
    ImGui::InputFloat("density", &density, 0.01f);
    ImGui::InputFloat("bleed", &bleed, 0.01f);
    ImGui::InputFloat("light", &light, 0.01f);
    ImGui::InputFloat("dark edge", &darkEdge, 0.001f);
    ImGui::InputFloat("paper granulation", &granulation, 0.01f);
    ImGui::InputFloat("tremor", &finalTremor, 0.001f);    
}

static void ShowSettingsMenu()
{
    ImGui::Checkbox("Keyboard control enabled", &keyBoardEnabled);
    ImGui::Checkbox("mouse capture enabled", &mouseCaptureEnabled);
    ImGui::InputFloat("change increase yaw angle jump (key ->)", &yawIncrease, 1.000f);
    ImGui::InputFloat("change decrease yaw angle jump (key <-)", &yawDecrease, 1.000f);
    ImGui::InputFloat("change increase pitch angle jump (key UP)", &pitchIncrease, 1.000f);
    ImGui::InputFloat("change decrease pitch angle jump (key DOWN)", &pitchDecrease, 1.000f);
    ImGui::InputFloat("change increase roll angle jump (key PgDn)", &rollIncrease, 1.000f);
    ImGui::InputFloat("change decrease roll angle jump (key PgUp)", &rollDecrease, 1.000f);
    ImGui::InputFloat("change increase scale jump (key Z)", &scaleIncrease, 0.001f);
    ImGui::InputFloat("change decrease scale jump (key X)", &scaleDecrease, 0.001f);
    ImGui::InputFloat("change increase turbulance jump (key T)", &turbulanceIncrease, 0.001f);
    ImGui::InputFloat("change decrease turbulance jump (key R)", &turbulanceDecrease, 0.001f);
    ImGui::InputFloat("change increase density jump (key G)", &densityIncrease, 0.001f);
    ImGui::InputFloat("change decrease density jump (key F)", &densityDecrease, 0.001f);
    ImGui::InputFloat("change increase bleed jump (key B)", &bleedIncrease, 0.001f);
    ImGui::InputFloat("change decrease bleed jump (key N)", &bleedDecrease, 0.001f);
    ImGui::InputFloat("change increase light jump (key L)", &lightIncrease, 0.001f);
    ImGui::InputFloat("change decrease light jump (key K)", &lightDecrease, 0.001f);
    ImGui::InputFloat("change increase dark edge jump (key C)", &darkEdgeIncrease, 0.001f);
    ImGui::InputFloat("change decrease dark edge jump (key V)", &darkEdgeDecrease, 0.001f);
    ImGui::InputFloat("change increase paper granulation jump (key J)", &granulationIncrease, 0.001f);
    ImGui::InputFloat("change decrease paper granulation jump (key H)", &granulationDecrease, 0.001f);
    ImGui::InputFloat("change increase tremor jump (key U)", &tremorIncrease, 0.001f);
    ImGui::InputFloat("change decrease tremor jump (key Y)", &tremorDecrease, 0.001f);
}


static void ShowExampleAppMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        static imgui_addons::ImGuiFileBrowser file_dialog;

        if (ImGui::BeginMenu("file"))
        {
            if (ImGui::BeginMenu("object"))
            {
                if (ImGui::BeginMenu("model"))
                {
                    ImGui::InputText("obj file", defaultModel, IM_ARRAYSIZE(defaultModel));
                    //if (ImGui::MenuItem("Open", "Ctrl+O")) { show_open_dialog = true; } //not working, permissions problem :/
                    /*if (show_open_dialog)
                    {
                        ImGui::OpenPopup("Open File");
                        
                    }
                    if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(600, 300), "."))
                    {
                        ImGui::InputText(file_dialog.selected_fn.c_str(), defaultModel, IM_ARRAYSIZE(defaultModel));
                        show_open_dialog = false;
                    }*/
                    if (ImGui::BeginMenu("shadowing"))
                    {
                        ImGui::Combo("shadowing", &shadowing, "Phong\0Toon\0Grid\0\0");
                        ImGui::EndMenu();
                    }
                    ShowObjectMenu();
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("primitive"))
                {
                    ImGui::Combo("primitives", &primitive, "Cube\0Sphere\0Pyramid\0\0");
                    if (ImGui::BeginMenu("shadowing"))
                    {
                        ImGui::Combo("shadowing", &shadowing, "Phong\0Toon\0Grid\0\0");
                        ImGui::EndMenu();
                    }
                    ShowObjectMenu();
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("floor"))
            {
                ImGui::Checkbox("floor enabled", &floorEnabled);
                ImGui::Checkbox("texture enabled", &textureFloorEnabled);
                ImGui::SameLine(); ImGui::InputText("diffuse texture", diffuseTextureFloor, IM_ARRAYSIZE(diffuseTextureFloor));
                if (ImGui::BeginMenu("translation"))
                {
                    ImGui::InputFloat("x", &floorTranslateX, 1.0f);
                    ImGui::InputFloat("y", &floorTranslateY, 1.0f);
                    ImGui::InputFloat("z", &floorTranslateZ, 1.0f);
                    ImGui::EndMenu();
                }
                ImGui::Checkbox("water enabled", &waterEnabled);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("wall"))
            {
                ImGui::Checkbox("wall enabled", &wallEnabled);
                ImGui::Checkbox("texture enabled", &textureWallEnabled);
                ImGui::SameLine(); ImGui::InputText("diffuse texture", diffuseTextureWall, IM_ARRAYSIZE(diffuseTextureWall));
                if (ImGui::BeginMenu("translation"))
                {
                    ImGui::InputFloat("x", &wallTranslateX, 1.0f);
                    ImGui::InputFloat("y", &wallTranslateY, 1.0f);
                    ImGui::InputFloat("z", &wallTranslateZ, 1.0f);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("cubeBox"))
            {
                ShowCubeBoxMenu();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("scene"))
        {
            if (ImGui::BeginMenu("cameras"))
            {
                ShowCamerasMenu();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("lights"))
            {
                ShowLightsMenu();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("watercolor"))
        {
            ShowWatercolorMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("settings"))
        {
            ShowSettingsMenu();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

//static void ShowExampleAppMainMenuBar()
//{
//    if (ImGui::BeginMainMenuBar())
//    {
//        if (ImGui::BeginMenu("File"))
//        {
//            ShowExampleMenuFile();
//            ImGui::EndMenu();
//        }
//        if (ImGui::BeginMenu("Edit"))
//        {
//            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
//            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
//            ImGui::Separator();
//            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
//            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
//            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
//            ImGui::EndMenu();
//        }
//        ImGui::EndMainMenuBar();
//    }
//}





