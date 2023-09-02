// g++ -o main altimeter.cpp -lglfw -lGLESv2 -lEGL

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "includes/texture.cpp"

const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord; // Circle texture coordinates
layout (location = 2) in vec2 saPos;
layout (location = 3) in vec2 sTexCoord; // 2.Circle texture coordinates
uniform mat4 model; // Transformation matrix"
out vec2 TexCoord;
out vec2 ArrowTexCoord; // Output for arrow texture coordinates

void main() 
{
    if (saPos.x > 0.0 || saPos.x < 0.0 ) {
        gl_Position = vec4(saPos.x, -saPos.y, 0.0, 1.0);
    } 
    else {
        gl_Position = model*vec4(aPos.x, -aPos.y, 0.0, 1.0);
        
    }

    TexCoord = aTexCoord;
    ArrowTexCoord = sTexCoord; // Pass arrow texture coordinates to the fragment shader
}
)";

const char *fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D texture1;
uniform sampler2D arrowTexture; // New uniform for the arrow texture

void main()
{
    if (TexCoord.y >= 0.0 && TexCoord.x >  0.0) { 
        // Display the arrow texture if the fragment is inside the arrow's head
        FragColor = texture(arrowTexture, TexCoord);
    }
    else {
        
        FragColor = vec4(0.0f,0.0f,1.0f,1.0f)*texture(texture1, TexCoord);
    }
}
)";

// this fragmentshader source use for arrow because arrow doesnt have a texture ::frag color decide arrows will be what color in program
const char *sfragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.6, 0.58, 0.58,1.0); // GRAY color for the lines
}
)";


// global variable for arrows and circle movement
float currentAltitude = 0.0f;
float currentAltitudelong = 0.0f;
float currentAltitudesmall = 0.0f;
float altitudeValue = 0.0f;
float circleYPosition = 0.0f;  // Vertical position of the circle
float circleRotation = 0.0f;   // Rotation angle of the circle in degrees
float circleYPositions = 0.0f; // Vertical position of the circle
float circleRotations = 0.0f;  // Rotation angle of the circle in degrees
texture tex;
void drawinnercircle()
{
    // specify circle coordinats and their texture coordinat for using in vbo and send to shaders
    const int numSegments = 200;
    float circleVertices[(numSegments + 1) * 2];
    float texCoords[(numSegments + 1) * 2];

    const float radius = 0.82f;
    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = i * 2.0f * M_PI / numSegments;
        circleVertices[i * 2] = radius * std::cos(angle);
        circleVertices[i * 2 + 1] = radius * std::sin(angle);
        texCoords[i * 2] = (circleVertices[i * 2] + radius) / (2 * radius);
        texCoords[i * 2 + 1] = (circleVertices[i * 2 + 1] + radius) / (2 * radius);
    }

    // Create vertex and fragment shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Create VBO for the circle
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices) + sizeof(texCoords), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(circleVertices), circleVertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(circleVertices), sizeof(texCoords), texCoords);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "aPos");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "sTexCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(sizeof(circleVertices)));
    glEnableVertexAttribArray(1);

    // bind texture to the second circle in the program
    GLuint textureid=tex.settextureinner();
   

    glUseProgram(shaderProgram);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureid); 

    // here giving rotation and movement features to circle
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(circleRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  

    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 1);

    glDeleteTextures(1, &textureid);
}

void drawStick(float altitude, float height)
{
    // some equations for draw stick like this : ---->
    const float minAltitude = 0.0f;
    const float maxAltitude = 100000.0f;
    const float minAngle = 0.0f;
    const float maxAngle = 360.0f;

    float altitudeRange = maxAltitude - minAltitude;
    float altitudeRatio = ((altitude)-minAltitude) / altitudeRange;
    float rotationAngle = minAngle + altitudeRatio * 2 * M_PI;

    const float centerX = 0.0f;
    const float centerY = 0.0f;
    const float centerWidth = 0.007f;
    const float frontHeight = -height; // 0.755f
    const float tipHeight = 0.085;

    // rotation matrix help us about location calculation for each rotation
    float rotationMatrix[4] = {
        cosf(rotationAngle), -sinf(rotationAngle),
        sinf(rotationAngle), cosf(rotationAngle)};
    // storing point in vector(X,Y)
    std::vector<float> indicatorVertices = {
        centerX - centerWidth / 2, centerY,  centerX + centerWidth / 2, centerY,
        centerX - centerWidth / 2, centerY + frontHeight, centerX - centerWidth / 2, centerY + frontHeight,
        centerX + centerWidth / 2, centerY + frontHeight, centerX + centerWidth / 2, centerY,
        centerX - centerWidth / 2, centerY + frontHeight,  centerX - centerWidth * 3.8, centerY + frontHeight + tipHeight,
        centerX + centerWidth / 2, centerY + frontHeight,  centerX - centerWidth * 3.8, centerY + frontHeight + tipHeight,
        centerX + centerWidth * 3.8, centerY + frontHeight + tipHeight,centerX + centerWidth / 2, centerY + frontHeight};
    // Circular motion is provided by the operations here
    for (int i = 0; i < 12; ++i)
    {
        float x = indicatorVertices[i * 2];
        float y = indicatorVertices[i * 2 + 1];
        indicatorVertices[i * 2] = x * rotationMatrix[0] + y * rotationMatrix[2];
        indicatorVertices[i * 2 + 1] = x * rotationMatrix[1] + y * rotationMatrix[3];
    }
    
    // shaders compile,linking and creating vertex buffer object(VBO)
    GLuint vertexShaders = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaders, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShaders);

    GLuint fragmentShaders = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaders, 1, &sfragmentShaderSource, nullptr);
    glCompileShader(fragmentShaders);

    // Create shader program
    GLuint shaderPrograms = glCreateProgram();
    glAttachShader(shaderPrograms, vertexShaders);
    glAttachShader(shaderPrograms, fragmentShaders);
    glLinkProgram(shaderPrograms);

    GLuint stickVBO;
    glGenBuffers(1, &stickVBO);
    glBindBuffer(GL_ARRAY_BUFFER, stickVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * indicatorVertices.size(), indicatorVertices.data(), GL_STATIC_DRAW);
    // get location from shaders
    GLuint positionLoc = glGetAttribLocation(shaderPrograms, "saPos");
    glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // according to location set pointer and draw
    glEnableVertexAttribArray(positionLoc);

    glUseProgram(shaderPrograms);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 12);

    glDisableVertexAttribArray(positionLoc);
    // clean
    glDeleteBuffers(1, &stickVBO);
    glDeleteProgram(shaderPrograms);
    glDeleteShader(vertexShaders);
    glDeleteShader(fragmentShaders);
}

void updateAltitude(GLFWwindow *window)
{
    //listen the window and do something according to event
    //here allow using arrows from keybord and make some calculations
    //arrows can move with this assignmnet and calculations
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        currentAltitude -= 1000.0f;     // uzungri
        currentAltitudesmall -= 100.0f; // kısa gri
        currentAltitudelong -= 10.0f;   // kırmızı
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        currentAltitude += 1000.0f;     // uzungri
        currentAltitudesmall += 100.0f; // kısa gri
        currentAltitudelong += 10.0f;   // kırmızı
    }


    //rotation features for inner circle
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        if (circleRotation < 92)//upper bounder for inner circle
        {
            circleRotation += 1.0f;
          
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        if (circleRotation > -180)//lower bounder for inner circle
        {
            circleRotation -= 1.0f;
            std::cout << "rot:" << circleRotation << std::endl;
        }
    }
    //
}
void drawoutercircle()
{
//calculate circle and texture coordinats
    const int numSegments = 200;
    float circleVertices[(numSegments + 1) * 2];
    float texCoords[(numSegments + 1) * 2];

    const float radius = 1.0f;
    for (int i = 0; i <= numSegments; ++i)
    {
        float angle = i * 2.0f * M_PI / numSegments;
        circleVertices[i * 2] = radius * std::cos(angle);
        circleVertices[i * 2 + 1] = radius * std::sin(angle);
        texCoords[i * 2] = (circleVertices[i * 2] + radius) / (2 * radius);
        texCoords[i * 2 + 1] = (circleVertices[i * 2 + 1] + radius) / (2 * radius);
    }

    // Create vertex and fragment shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Create VBO for the circle
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices) + sizeof(texCoords), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(circleVertices), circleVertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(circleVertices), sizeof(texCoords), texCoords);

    GLint posAttrib = glGetAttribLocation(shaderProgram, "aPos");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "aTexCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(sizeof(circleVertices)));
    glEnableVertexAttribArray(1);

    // Load texture
    GLuint texture=tex.settextureouter();
    
//calculate rotations
    glUseProgram(shaderProgram);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, circleYPositions, 0.0f));
    model = glm::rotate(model, glm::radians(circleRotations), glm::vec3(0.0f, 0.0f, 1.0f));
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture); 

    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 1);
}

GLFWwindow *init()
{
//create window and setup  window with rules of Opengl es 2.0
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL ES 2.0 Altimeter", nullptr, nullptr);//setup phase 
    glfwMakeContextCurrent(window);//

    // Initialize EGL
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint major, minor;
    eglInitialize(display, &major, &minor);
    return window;
}

int main()
{
    // Initialize GLFW and create a window

    GLFWwindow *window = init();
    // Define vertices and texture coordinates for the circle


    // Main loop
    while (!glfwWindowShouldClose(window))
    {

        glEnable(GL_BLEND);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//set background color 
        glClear(GL_COLOR_BUFFER_BIT);//and clean
        // Enable alpha blending to draw lines on top of the texture
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        updateAltitude(window);
        drawinnercircle();
        drawoutercircle();
        drawStick(currentAltitude, 0.555f);
        drawStick(currentAltitudesmall, 0.355f);
        drawStick(currentAltitudelong, 0.855f);

        glDisable(GL_BLEND);

        glfwSwapBuffers(window);//swap showing window to after window for showing in screen
        glfwPollEvents();
    }

    // Cleanup

    glfwTerminate();
    return 0;
}