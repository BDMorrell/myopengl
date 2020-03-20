#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "shader.hpp"
#include "getFileContents.hpp"
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const float verticies[] = { // 0.866025403784 = sqrt(3)/2 // XYRGBST
   .75,  .75, 1., 1., 1., 1., 1.,
  -.75,  .75, 1., 1., 1., 0., 1.,
  -.75, -.75, 1., 1., 1., 0., 0.,
   .75, -.75, 1., 1., 1., 1., 0.
};

const GLuint squareElements[] = {
  0, 1, 2,
  0, 2, 3
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
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareElements), squareElements, GL_STATIC_DRAW);

  GLuint textureA, textureB;
  glGenTextures(1, &textureA);
  glGenTextures(1, &textureB); // I know, I know. I just don't want to worry about an array right now; which is silly of me
  int width, height;
  unsigned char *pixels = stbi_load("assets/A.jpg", &width, &height, NULL, 3);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureA);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  stbi_image_free(pixels);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textureB);
  pixels = stbi_load("assets/B.jpg", &width, &height, NULL, 3);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  stbi_image_free(pixels);
  // TODO: FINISH LOADING IN TEXTURES, AND DISPLAY THEM!!!

  char *compileLog = NULL;

  struct rawData *fileSource;

  if ((fileSource = getFileContents("shaders/pos2col3tex2.vert")) == NULL) {
    std::cerr << "filed to load *.vert" << std::endl;
    return 5;
  }
  ogl::shader *vertexShader = new ogl::shader(fileSource->value, GL_VERTEX_SHADER, fileSource->size);
  delete[] fileSource->value;
  delete fileSource;
  if (vertexShader->hasShaderInfoLog()) {
    std::cout << "vertexShader:" << std::endl << vertexShader->getShaderInfoLog() << std::endl;
  }

  if ((fileSource = getFileContents("shaders/doubleTextureColor.frag")) == NULL) {
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
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void *)(2*sizeof(float)));
  GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void *)(5*sizeof(float)));

  glUniform1i(glGetUniformLocation(shaderProgram, "texA"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "texB"), 1);

  // glfwSwapInterval(500); // v-sync
  while(!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    static char time = 0; // keep track of framerate in a sense. TODO: LIMIT FRAMERATE! glfwSwapInterval isn't working.
    glClearColor(((float) time++/255.), 0., 0., 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
