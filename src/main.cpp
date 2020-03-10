#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL); // Windowed

  glfwMakeContextCurrent(window);
  int gladInitRes = gladLoadGL();
  if (!gladInitRes) {
     std::cerr << "Unable to initialize glad" << std::endl;
     glfwDestroyWindow(window);
     glfwTerminate();
     return -1;
  }

  while(!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }
  glfwTerminate();
  return 0;
}
