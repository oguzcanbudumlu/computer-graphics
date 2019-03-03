#include "helper.h"
#include "entities.cpp"

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;
int recentWidth, recentHeight, currentWidth = 600, currentHeight = 600;
bool fullScreen = false;
Space space;

void resizeWindow(GLFWwindow *window) {
    if (!fullScreen) {
        glfwGetFramebufferSize(window, &recentWidth, &recentHeight);
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primary);
        glfwSetWindowMonitor(window, primary, 0, 0, mode->width, mode->height, mode->refreshRate);
        glfwGetFramebufferSize(window, &currentWidth, &currentHeight);
        fullScreen = true;
    }
    else {
        currentWidth = recentWidth;
        currentHeight = recentHeight;
        glfwSetWindowMonitor(window, nullptr, 0, 0, currentWidth, currentHeight, 0);
        fullScreen = false;
    }
}

static void errorCallback(int error, const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) space.camera->speedUp();
    else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) space.camera->speedDown();
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) space.camera->goUp();
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) space.camera->goDown();
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) space.camera->turnLeft();
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) space.camera->turnRight();
    else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) space.terrain->increaseHeightFactor();
    else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) space.terrain->decreaseHeightFactor();
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
            resizeWindow(window);
        }
    }
}
void render(Space space) {
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, space.terrain->vertices);

    glDrawArrays(GL_TRIANGLES, 0, space.terrain->triangleCount * 3);

    glDisableClientState(GL_VERTEX_ARRAY);  
}

void sendShaderData(Space space) {
    glViewport(0, 0, currentWidth, currentHeight);

    GLint shaderLocation;

    shaderLocation = glGetUniformLocation(idProgramShader, "widthTexture");
    glUniform1i(shaderLocation, widthTexture);

    shaderLocation = glGetUniformLocation(idProgramShader, "heightTexture");
    glUniform1i(shaderLocation, heightTexture);

    shaderLocation = glGetUniformLocation(idProgramShader, "heightFactor");
    glUniform1f(shaderLocation, space.terrain->heightFactor);

    shaderLocation = glGetUniformLocation(idProgramShader, "cameraPosition");
    glUniform4fv(shaderLocation, 1, &(space.camera->position).x);

    shaderLocation = glGetUniformLocation(idProgramShader, "lightPosition");
    glUniform4fv(shaderLocation, 1, &(space.light->position).x);

    shaderLocation = glGetUniformLocation(idProgramShader, "MV");
    glUniformMatrix4fv(shaderLocation, 1, GL_FALSE, &(space.getMVMatrix())[0][0]);

    shaderLocation = glGetUniformLocation(idProgramShader, "MVP");
    glUniformMatrix4fv(shaderLocation, 1, GL_FALSE, &(space.getMVPMatrix())[0][0]);
}

int main(int argc, char * argv[]) {

  if (argc != 2) {
    printf("Only one texture image expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

  win = glfwCreateWindow(600, 600, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    glfwTerminate();
    exit(-1);
  }

  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);

  space = Space(widthTexture, heightTexture);
  
  glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE);

  sendShaderData(space);
  
  glEnable(GL_DEPTH_TEST);
  
  while (!glfwWindowShouldClose(win)) {
    space.camera->go();
    render(space);
    processInput(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
    sendShaderData(space);
  }

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}