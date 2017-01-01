#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  void create_scene();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  // draw all objects
  void render() const;
  void set_projectionCamBuffer();

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void initializeTextures() const;
  void initializeFramebuffer();
  void initializeUniformBuffer();

  void updateView();
  glm::fmat4 uploadPlanetTransforms(planet const&, const unsigned index, glm::fmat4 const&) const;
  void uploadStarTransforms() const;

  // cpu representation of model

  GLuint ubo_cam;
  camera_buffer cam_buffer;
  std::vector<planet_ptr> planet_vector;
  model_object planet_object;
  std::vector<star> star_vector;
  model_object star_object; //gpu representation of star
  texture_object quad_tex;
  GLuint renderbuffer;
  GLuint framebuffer;
  model_object quad_object;
  int screen_width = 640;
  int screen_height = 480;
  std::vector<float> post_process = {0.0f,0.0f,0.0f,0.0f }; //Save state of input for a post_processing effect
};

#endif