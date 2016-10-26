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
  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();
  glm::fmat4 uploadPlanetTransforms(planet const&) const;
  void uploadMoonTransforms(planet const&, glm::fmat4 const&) const;

  // cpu representation of model
  std::vector<planet> planet_vector;
  model_object planet_object;
  std::vector<star> star_vector;
  model_object star_object;
  std::vector<unsigned> star_ind_vec;
};

#endif