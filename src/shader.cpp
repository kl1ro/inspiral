#include "shader.hpp"

#include "file.hpp"
#include "globals.hpp"

GLuint compileShader(GLenum type, const std::string& src) {
  const char* cstr = src.c_str();
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &cstr, nullptr);
  glCompileShader(shader);
  GLint ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

  if (!ok) {
    char log[512];
    glGetShaderInfoLog(shader, 512, nullptr, log);
    std::cerr << "Shader error: " << log << "\n";
    std::exit(1);
  }

  return shader;
}

GLuint linkProgram(GLuint vert, GLuint frag) {
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert);
  glAttachShader(prog, frag);
  glLinkProgram(prog);
  GLint ok;
  glGetProgramiv(prog, GL_LINK_STATUS, &ok);

  if (!ok) {
    char log[512];
    glGetProgramInfoLog(prog, 512, nullptr, log);
    std::cerr << "Link error: " << log << "\n";
    std::exit(1);
  }

  return prog;
}

void setupGlobalProgram() {
  GLuint& program = Globals::program;

  if (program)
    return;

  ShadersConfig& shaders = Globals::config.triangle.shaders;

  std::string vertSrc = readFile(shaders.vertex);
  std::string fragSrc = readFile(shaders.fragment);

  GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
  GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

  program = linkProgram(vert, frag);

  glDeleteShader(vert);
  glDeleteShader(frag);
}
