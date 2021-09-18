//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//
//#include "Shader.h"
//#include "Camera.h"
//#include "Model.h"
//
//#include <iostream>
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//unsigned int loadCubemap(vector<std::string> faces);
//void processInput(GLFWwindow* window);
//
//// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
//// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//// model
//float modelYawAngle = 0.0f;
//float modelRollAngle = 0.0f;
//float modelPitchAngle = 0.0f;
//float modelScale = 0.005f;
//
//int main()
//{
//    // glfw: initialize and configure
//    // ------------------------------
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_SAMPLES, 4);
//
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//    // glfw window creation
//    // --------------------
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Watercolor application", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//
//    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, 1);
//
//    // glad: load all OpenGL function pointers
//    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//    //stbi_set_flip_vertically_on_load(true);
//
//    // configure global opengl state
//    // -----------------------------
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_MULTISAMPLE);
//
//    // build and compile shaders
//    // -------------------------
//    //Shader ourShader("loadModelVshader.glsl", "loadModelFshader.glsl");
//    Shader ourShader("vShader.glsl", "fShader.glsl");
//    Shader skyboxShader("loadSkyBoxVshader.glsl", "loadSkyBoxFshader.glsl");
//    Shader shadeShader("shadeVShader.glsl", "shadeFShader.glsl");
//
//    float planeVertices[] = {
//        // positions            // normals         // texcoords
//         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   25.0f,  0.0f,
//        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
//        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
//
//         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   25.0f,  0.0f,
//        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
//         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   25.0f, 25.0f
//    };
//    // plane VAO
//    unsigned int planeVBO, planeVAO;
//    glGenVertexArrays(1, &planeVAO);
//    glGenBuffers(1, &planeVBO);
//    glBindVertexArray(planeVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//    glBindVertexArray(0);
//
//    // load textures
//    // -------------
//    unsigned int woodTexture = TextureFromFile("wood.png", "textures", false);
//
//    float skyboxVertices[] = {
//        // positions          
//        -1.0f,  1.0f, -1.0f,
//        -1.0f, -1.0f, -1.0f,
//         1.0f, -1.0f, -1.0f,
//         1.0f, -1.0f, -1.0f,
//         1.0f,  1.0f, -1.0f,
//        -1.0f,  1.0f, -1.0f,
//
//        -1.0f, -1.0f,  1.0f,
//        -1.0f, -1.0f, -1.0f,
//        -1.0f,  1.0f, -1.0f,
//        -1.0f,  1.0f, -1.0f,
//        -1.0f,  1.0f,  1.0f,
//        -1.0f, -1.0f,  1.0f,
//
//         1.0f, -1.0f, -1.0f,
//         1.0f, -1.0f,  1.0f,
//         1.0f,  1.0f,  1.0f,
//         1.0f,  1.0f,  1.0f,
//         1.0f,  1.0f, -1.0f,
//         1.0f, -1.0f, -1.0f,
//
//        -1.0f, -1.0f,  1.0f,
//        -1.0f,  1.0f,  1.0f,
//         1.0f,  1.0f,  1.0f,
//         1.0f,  1.0f,  1.0f,
//         1.0f, -1.0f,  1.0f,
//        -1.0f, -1.0f,  1.0f,
//
//        -1.0f,  1.0f, -1.0f,
//         1.0f,  1.0f, -1.0f,
//         1.0f,  1.0f,  1.0f,
//         1.0f,  1.0f,  1.0f,
//        -1.0f,  1.0f,  1.0f,
//        -1.0f,  1.0f, -1.0f,
//
//        -1.0f, -1.0f, -1.0f,
//        -1.0f, -1.0f,  1.0f,
//         1.0f, -1.0f, -1.0f,
//         1.0f, -1.0f, -1.0f,
//        -1.0f, -1.0f,  1.0f,
//         1.0f, -1.0f,  1.0f
//    };
//    unsigned int skyboxVBO, skyboxVAO;
//    glGenVertexArrays(1, &skyboxVAO);
//    glGenBuffers(1, &skyboxVBO);
//
//    glBindVertexArray(skyboxVAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
//
//    // position attribute
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    //make frame buffer for shadows
//
//    unsigned int depthMapFBO;
//    glGenFramebuffers(1, &depthMapFBO);
//    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
//    unsigned int depthMap;
//    glGenTextures(1, &depthMap);
//    glBindTexture(GL_TEXTURE_2D, depthMap);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
//        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
//    glDrawBuffer(GL_NONE);
//    glReadBuffer(GL_NONE);
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    float near_plane = 1.0f, far_plane = 7.5f;
//    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//    glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
//        glm::vec3(0.0f, 0.0f, 0.0f),
//        glm::vec3(0.0f, 1.0f, 0.0f));
//    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
//
//
//    // positions of the point lights
//    glm::vec3 pointLightPositions[] = {
//        glm::vec3(0.7f,  0.2f,  2.0f),
//        glm::vec3(2.3f, -3.3f, -4.0f),
//        glm::vec3(-4.0f,  2.0f, -12.0f),
//        glm::vec3(0.0f,  0.0f, -3.0f)
//    };
//
//    ourShader.use();
//    ourShader.setVec3("viewPos", camera.Position);
//    ourShader.setFloat("material.shininess", 32.0f);
//
//    // load models
//    // -----------
//    Model ourModel("objects/dolphin/dolphin.obj");
//
//    // load skybox
//    vector<std::string> faces = 
//    {
//            "skyboxes/watercolor paper/right.jpg",
//            "skyboxes/watercolor paper/left.jpg",
//            "skyboxes/watercolor paper/top.jpg",
//            "skyboxes/watercolor paper/bottom.jpg",
//            "skyboxes/watercolor paper/front.jpg",
//            "skyboxes/watercolor paper/back.jpg"
//    };
//    unsigned int cubemapTexture = loadCubemap(faces);
//
//    // lighting info
//    // -------------
//    glm::vec3 lightPos(2.0f, -4.0f, -1.0f);
//
//
//    // draw in wireframe
//    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//    // render loop
//    // -----------
//    while (!glfwWindowShouldClose(window))
//    {
//        // per-frame time logic
//        // --------------------
//        float currentFrame = glfwGetTime();
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        // input
//        // -----
//        processInput(window);
//
//        // render
//        // ------
//        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glm::mat4 lightProjection, lightView;
//        glm::mat4 lightSpaceMatrix;
//        float near_plane = 1.0f, far_plane = 7.5f;
//        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
//        lightSpaceMatrix = lightProjection * lightView;
//        // render scene from light's point of view
//        shadeShader.use();
//        shadeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
//
//        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        glClear(GL_DEPTH_BUFFER_BIT);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, woodTexture);
//        glm::mat4 model = glm::mat4(1.0f);
//        shadeShader.setMat4("model", model);
//        glBindVertexArray(planeVAO);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        model = glm::mat4(1.0f);
//        model = glm::rotate(model, glm::radians(modelYawAngle), glm::vec3(0.0, 1.0, 0.0));
//        model = glm::rotate(model, glm::radians(modelRollAngle), glm::vec3(0.0, 0.0, 1.0));
//        model = glm::rotate(model, glm::radians(modelPitchAngle), glm::vec3(1.0, 0.0, 0.0));
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
//        model = glm::scale(model, modelScale * glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
//        shadeShader.setMat4("model", model);
//        ourModel.Draw(shadeShader);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        // reset viewport
//        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        
//
//        // don't forget to enable shader before setting uniforms
//        ourShader.use();
//
//
//        // directional light
//        glm::vec3 n = normalize(glm::vec3(1.0f, -6.0f, 0.0f));
//        ourShader.setVec3("dirLight.direction", n);
//        ourShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
//        ourShader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
//        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
//        // point light 1
//        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
//        ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
//        ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
//        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
//        ourShader.setFloat("pointLights[0].constant", 1.0f);
//        ourShader.setFloat("pointLights[0].linear", 0.09);
//        ourShader.setFloat("pointLights[0].quadratic", 0.032);
//        // point light 2
//        ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
//        ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
//        ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
//        ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
//        ourShader.setFloat("pointLights[1].constant", 1.0f);
//        ourShader.setFloat("pointLights[1].linear", 0.09);
//        ourShader.setFloat("pointLights[1].quadratic", 0.032);
//        // point light 3
//        ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
//        ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
//        ourShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
//        ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
//        ourShader.setFloat("pointLights[2].constant", 1.0f);
//        ourShader.setFloat("pointLights[2].linear", 0.09);
//        ourShader.setFloat("pointLights[2].quadratic", 0.032);
//        // point light 4
//        ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
//        ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
//        ourShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
//        ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
//        ourShader.setFloat("pointLights[3].constant", 1.0f);
//        ourShader.setFloat("pointLights[3].linear", 0.09);
//        ourShader.setFloat("pointLights[3].quadratic", 0.032);
//        // spotLight
//        ourShader.setVec3("spotLight.position", camera.Position);
//        ourShader.setVec3("spotLight.direction", camera.Front);
//        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
//        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
//        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
//        ourShader.setFloat("spotLight.constant", 1.0f);
//        ourShader.setFloat("spotLight.linear", 0.09);
//        ourShader.setFloat("spotLight.quadratic", 0.032);
//        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
//        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
//        
//
//        // view/projection transformations
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        glm::mat4 view = camera.GetViewMatrix();
//        ourShader.setMat4("projection", projection);
//        ourShader.setMat4("view", view);
//
//        // render the loaded model
//        model = glm::mat4(1.0f);
//        model = glm::rotate(model, glm::radians(modelYawAngle), glm::vec3(0.0, 1.0, 0.0));
//        model = glm::rotate(model, glm::radians(modelRollAngle), glm::vec3(0.0, 0.0, 1.0));
//        model = glm::rotate(model, glm::radians(modelPitchAngle), glm::vec3(1.0, 0.0, 0.0));
//        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
//        model = glm::scale(model, modelScale * glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
//        ourShader.setMat4("model", model);
//        ourModel.Draw(ourShader);
//
//        glm::mat4 planeModel = glm::mat4(1.0f);
//        ourShader.setMat4("model", planeModel);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, woodTexture);
//        glBindVertexArray(planeVAO);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//
//        glDepthFunc(GL_LEQUAL);
//        skyboxShader.use();
//        glDepthMask(GL_FALSE);
//        skyboxShader.use();
//
//        // view/projection transformations
//        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
//        skyboxShader.setMat4("projection", projection);
//        skyboxShader.setMat4("view", view);
//
//        // render the loaded model
//        model = glm::mat4(1.0f);
//        skyboxShader.setMat4("model", model);
//
//        glBindVertexArray(skyboxVAO);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
//        //glDrawArrays(GL_TRIANGLES, 0, 36);
//        glDepthMask(GL_TRUE);
//
//
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//        // -------------------------------------------------------------------------------
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    // glfw: terminate, clearing all previously allocated GLFW resources.
//    // ------------------------------------------------------------------
//    glDeleteVertexArrays(1, &planeVAO);
//    glDeleteBuffers(1, &planeVBO);
//    glfwTerminate();
//    return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(UP, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(DOWN, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//
//    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
//        modelPitchAngle = modelPitchAngle - 0.7;
//    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
//        modelPitchAngle = modelPitchAngle + 0.7;
//    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
//        modelYawAngle = modelYawAngle + 0.7;
//    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
//        modelYawAngle = modelYawAngle - 0.7;
//    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
//        modelRollAngle = modelRollAngle + 0.7;
//    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
//        modelRollAngle = modelRollAngle - 0.7;
//    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
//        modelScale = modelScale * 1.01;
//    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
//        modelScale = modelScale * 0.99;
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.ProcessMouseScroll(yoffset);
//}
//
//unsigned int loadCubemap(vector<std::string> faces)
//{
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//    int width, height, nrChannels;
//    for (unsigned int i = 0; i < faces.size(); i++)
//    {
//        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
//        if (data)
//        {
//            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
//                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
//            );
//            stbi_image_free(data);
//        }
//        else
//        {
//            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
//            stbi_image_free(data);
//        }
//    }
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//    return textureID;
//}