#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <map>
#include <glbinding/gl/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <iostream>

// use gl definitions from glbinding
using namespace gl;

struct moon {
  float size;
  float rotation_velocity;
  glm::fvec3 distance_to_planet;
  glm::fmat4 parent_model_matrix;
  ~moon() { std::cout << "moon was destroyed"; }
};

//planet representation
struct planet {
  float size;
  float rotation_velocity;
  glm::fvec3 distance_to_origin;
  std::vector<std::shared_ptr<moon>> moons;
};

// gpu representation of model
struct model_object {
  // vertex array object
  GLuint vertex_AO = 0;
  // vertex buffer object
  GLuint vertex_BO = 0;
  // index buffer object
  GLuint element_BO = 0;
  // primitive type to draw
  GLenum draw_mode = GL_NONE;
  // indices number, if EBO exists
  GLsizei num_elements = 0;
};

// gpu representation of texture
struct texture_object {
  // handle of texture object
  GLuint handle = 0;
  // binding point
  GLenum target = GL_NONE;
};

// shader handle and uniform storage
struct shader_program {
  shader_program(std::string const& vertex, std::string const& fragment)
   :vertex_path{vertex}
   ,fragment_path{fragment}
   ,handle{0}
   {}

  // path to shader source
  std::string vertex_path;
  std::string fragment_path;
  // object handle
  GLuint handle;
  // uniform locations mapped to name
  std::map<std::string, GLint> u_locs{};
};
#endif