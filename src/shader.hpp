#pragma once
#include <glad/glad.h>

namespace ogl {
  class shader {
  private:
    GLuint handle;
    GLenum type;
    bool compileStatus;
    char *compileLog;
    bool compileLogRequested;
    void compileShader() {
      glCompileShader(this->handle);
      GLint status;
      glGetShaderiv(this->handle, GL_COMPILE_STATUS, &status);
      this->compileStatus = (status == GL_TRUE);
    }
  public:
    shader(const char *source, GLenum type) {
      this->type = type;
      this->compileLog = NULL;
      this->compileLogRequested = false;
      this->handle = glCreateShader(type);
      glShaderSource(this->handle, 1, &source, NULL);
      this->compileShader();
      // no error handling :(
    }
    ~shader() {
      // This marks the shader for deletion if it can't be deleted immediately,
      // so we're okay.
      glDeleteShader(handle);
      if (compileLog != NULL) {
        delete[] compileLog;
      }
    }
    bool isOkay() {
      return compileStatus;
    }
    bool hasShaderInfoLog() {
      return (this->getShaderInfoLog() != NULL);
    }
    char *getShaderInfoLog() { // please DO NOT delte[] returned value; may be null
      if (!compileLogRequested) {
        compileLogRequested = true;
        GLint logLength = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength == 0) {
          compileLog = NULL;
        } else {
          compileLog = new char[logLength]; // Note: LARGE logs can cause problems
          glGetShaderInfoLog(handle, logLength, NULL, compileLog);
        }
      }
      return compileLog;
    }
    GLuint getHandle() {
      return handle;
    }
  };
};
