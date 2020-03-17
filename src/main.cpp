#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "shader.hpp"
#include "getFileContents.hpp"

const float triangle[] = { // 0.866025403784 = sqrt(3)/2 // XYRGB
  0.866025403784, -0.5, 1., 0., 0.,
  0, 1, 0., 1., 0.,
  -0.866025403784, -0.5, 0., 0., 1.
};

char *getProgramInfoLog(GLuint shader); // please delete[] the returned value, may be NULL
void dumpErrors(const char *id = NULL); // handy debugging function

int main()
{
  if(!glfwInit()) {
    std::cerr << "GLFW failed to initalize!" << std::endl;
    return 1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL); // Windowed

  glfwMakeContextCurrent(window);
  int gladInitRes = gladLoadGL();
  if (!gladInitRes) {
     std::cerr << "Unable to initialize glad" << std::endl;
     glfwDestroyWindow(window);
     glfwTerminate();
     return -1;
  }
  std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

  char *compileLog = NULL;

  struct rawData *fileSource;

  if ((fileSource = getFileContents("shaders/pos2col3.vert")) == NULL) {
    std::cerr << "filed to load *.vert" << std::endl;
    return 5;
  }
  ogl::shader *vertexShader = new ogl::shader(fileSource->value, GL_VERTEX_SHADER, fileSource->size);
  delete[] fileSource->value;
  delete fileSource;
  if (vertexShader->hasShaderInfoLog()) {
    std::cout << "vertexShader:" << std::endl << vertexShader->getShaderInfoLog() << std::endl;
  }

  if ((fileSource = getFileContents("shaders/colorPass.frag")) == NULL) {
    std::cerr << "filed to load *.frag" << std::endl;
    return 5;
  }
  ogl::shader *fragmentShader = new ogl::shader(fileSource->value, GL_FRAGMENT_SHADER, fileSource->size);
  delete[] fileSource->value;
  delete fileSource;
  if (fragmentShader->hasShaderInfoLog()) {
    std::cout << "fragmentShader:" << std::endl << fragmentShader->getShaderInfoLog() << std::endl;
  }

  if (!(vertexShader->isOkay() && fragmentShader->isOkay())) {
    std::cerr << "Main shaders failed to compile." << std::endl;
    std::cerr << "v:" << vertexShader->isOkay() << " f:" << fragmentShader->isOkay() << std::endl;
    return 5;
  }


  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader->getHandle());
  glAttachShader(shaderProgram, fragmentShader->getHandle());
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  GLint programIV = 0; // did it link?
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programIV);
  std::cout << "programIV: " << programIV << " ("<< (programIV == GL_TRUE) <<")" << std::endl;

  compileLog = getProgramInfoLog(shaderProgram);
  if (compileLog != NULL) {
    std::cout << "shaderProgram: " << std::endl << compileLog << std::endl;
    delete[] compileLog;
  }
  glUseProgram(shaderProgram);

  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  dumpErrors();
  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  dumpErrors();
  glEnableVertexAttribArray(posAttrib);
  dumpErrors();
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
  dumpErrors();
  glEnableVertexAttribArray(colAttrib);
  dumpErrors();
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(2*sizeof(float)));
  dumpErrors();


  // glfwSwapInterval(500); // v-sync
  while(!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    static char time = 0; // keep track of framerate in a sense. TODO: LIMIT FRAMERATE! glfwSwapInterval isn't working.
    glClearColor(((float) time++/255.), 0., 0., 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

char *getProgramInfoLog(GLuint program) { // please delete[] the returned value, may be NULL
  GLint length = 0;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
  if (length == 0) {
    return NULL;
  }
  char *buffer = new char[length]; // Note: LARGE logs can cause problems
  glGetProgramInfoLog(program, length, NULL, buffer);
  return buffer;
}

void dumpErrors(const char *id) { // handy debugging function
  static int dumpNumber = 0;
  std::cerr << "dump #" << dumpNumber++;
  if (id) {
    std::cerr << " (" << id << ")";
  }
  std::cerr << ":" << std::endl;
  GLenum e;
  while ((e = glGetError()) != GL_NO_ERROR) {
    std::cerr << "\tError " << e << std::endl;
  }
}
