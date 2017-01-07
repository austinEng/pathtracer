#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void createShader(GLuint &program, GLuint &texture_location) {
  static std::string vertex_source =
    "#version 330\n"
    "layout(location = 0) in vec4 vposition;\n"
    "layout(location = 1) in vec2 vtexcoord;\n"
    "out vec2 ftexcoord;\n"
    "void main() {\n"
    "   ftexcoord = vtexcoord;\n"
    "   gl_Position = vposition;\n"
    "}\n";

  static std::string fragment_source =
    "#version 330\n"
    "uniform sampler2D tex;\n" // texture uniform
    "in vec2 ftexcoord;\n"
    "layout(location = 0) out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = texture(tex, ftexcoord);\n"
    "}\n";

  const char *source;
  int length;

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  source = vertex_source.c_str();
  length = (int) vertex_source.size();
  glShaderSource(vertex_shader, 1, &source, &length);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  source = fragment_source.c_str();
  length = (int) fragment_source.size();
  glShaderSource(fragment_shader, 1, &source, &length);
  glCompileShader(fragment_shader);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  texture_location = glGetUniformLocation(program, "tex");
}

GLuint createTexture(unsigned int width, unsigned int height) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  return id;
}

void createQuadVBO(GLuint &buffer, GLuint &indices) {
  static const GLfloat vbo_data  [] = {
    1.0f, 1.0f, 0.0f,       1.0f, 1.0f, // vertex 0
    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, // vertex 1
    1.0f,-1.0f, 0.0f,       1.0f, 0.0f, // vertex 2
    -1.0f,-1.0f, 0.0f,       0.0f, 0.0f, // vertex 3
  };
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));

  glGenBuffers(1, &indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);

  GLuint indexData[] = {
    0,1,2,
    2,1,3,
  };

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, indexData, GL_STATIC_DRAW);
}

int main(int argc, char** argv) {
  int width = atoi(argv[1]);
  int height = atoi(argv[2]);

  GLFWwindow* window;
  // GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  // GLint mvp_location, vpos_location, vcol_location;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  window = glfwCreateWindow(width, height, "Simple example", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);

  glewExperimental = GL_FALSE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  // gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);
  glClearColor(0.f, 0.f, 0.f, 0.f);

  GLuint vao;
  glCreateVertexArrays(1, &vao);
  // glBindVertexArray(vao);

  // GLuint shader_program, texture_location;
  // createShader(shader_program, texture_location);

  // GLuint buffer, indices;
  // createQuadVBO(buffer, indices);

  // GLuint textureId = createTexture(width, height);


  while (!glfwWindowShouldClose(window)) {
    float ratio;
    int w, h;

    glfwGetFramebufferSize(window, &w, &h);
    ratio = w / (float) h;
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    // glUseProgram(shader_program);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, textureId);
    // glUniform1i(texture_location, 0);

    // glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}