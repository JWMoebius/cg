#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
	:Application{ resource_path }
	, planet_object{}, planet_vector{}
{
	planet Sun{ 0.5f, 5.5f, glm::fvec3{ 0.0, 0.0f, 0.0f }, {} };
	planet Mercury{ 0.1f, 1.5f, glm::fvec3{ 15.0f, 0.0f, 15.0f }, {} };
	planet Venus{ 0.1f, 1.5f, glm::fvec3{ 10.6f, 0.0f, 10.6f }, {} };
	planet Earth{ 0.1f, 1.5f, glm::fvec3{ 5.0f, 2.0f, 5.0f }, {} };
	planet Mars{ 0.5f, 1.5f, glm::fvec3{ 25.1f, 0.0f, 25.1f }, {} };
	planet Jupiter{ 0.1f, 1.5f, glm::fvec3{ 35.2f, 0.0f, 35.5f }, {} };
	planet Saturn{1.5f, 1.5f, glm::fvec3{ 20.0f, 0.0f, 20.0f }, {} };
	planet Uranus{ 0.2f, 1.5f, glm::fvec3{ 30.5f, 0.0f, 30.5f }, {} };

	std::shared_ptr<moon> Moon =
		std::make_shared<moon>(moon{ 1.0f, 8.0f, glm::fvec3{ 1.0f, 0.0f, 1.0f }, glm::fmat4{} });
	Earth.moons.push_back(Moon);

	// planet_vector.push_back(Sun);
//	planet_vector.push_back(Mercury);
//	planet_vector.push_back(Venus);
	planet_vector.push_back(Earth);
//	planet_vector.push_back(Mars);
//	planet_vector.push_back(Jupiter);
//	planet_vector.push_back(Saturn);
//	planet_vector.push_back(Uranus);

	initializeGeometry();
	initializeShaderPrograms();
}

void ApplicationSolar::render() const {
	// bind shader to upload uniforms
	glUseProgram(m_shaders.at("planet").handle);

	// Draw for all predefined planets in planet_vector depending on their attributes
	glm::fmat4 planet_mat;
	for (auto planet : planet_vector) {
		planet_mat = uploadPlanetTransforms(planet);

		// bind the VAO to draw
		glBindVertexArray(planet_object.vertex_AO);

		// draw bound vertex array using bound shader
		glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	}

	// Draw all predefined moons depending on their attributes
	for (auto planet : planet_vector) {
		for (auto moon : planet.moons) {
			uploadMoonTransforms(*moon, planet);
			glBindVertexArray(planet_object.vertex_AO);
			glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
		}
	}
}

void ApplicationSolar::updateView() {
	// vertices are transformed in camera space, so camera transform must be inverted
	glm::fmat4 view_matrix = glm::inverse(m_view_transform);
	// upload matrix to gpu
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
		1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {
	// upload matrix to gpu
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
		1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
	updateUniformLocations();

	// bind new shader
	glUseProgram(m_shaders.at("planet").handle);

	updateView();
	updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, -0.1f });
		updateView();
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, 0.1f });
		updateView();
	}
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
	// store shader program objects in container
	m_shaders.emplace("planet", shader_program{ m_resource_path + "shaders/simple.vert",
		m_resource_path + "shaders/simple.frag" });
	// request uniform locations for shader program
	m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
	m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
	m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
	m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
	model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

	// generate vertex array object
	glGenVertexArrays(1, &planet_object.vertex_AO);
	// bind the array for attaching buffers
	glBindVertexArray(planet_object.vertex_AO);

	// generate generic buffer
	glGenBuffers(1, &planet_object.vertex_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
	// configure currently bound array buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

	// activate first attribute on gpu
	glEnableVertexAttribArray(0);
	// first attribute is 3 floats with no offset & stride
	glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
	// activate second attribute on gpu
	glEnableVertexAttribArray(1);
	// second attribute is 3 floats with no offset & stride
	glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

	// generate generic buffer
	glGenBuffers(1, &planet_object.element_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
	// configure currently bound array buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

	// store type of primitive to draw
	planet_object.draw_mode = GL_TRIANGLES;
	// transfer number of indices to model object
	planet_object.num_elements = GLsizei(planet_model.indices.size());
}

ApplicationSolar::~ApplicationSolar() {
	glDeleteBuffers(1, &planet_object.vertex_BO);
	glDeleteBuffers(1, &planet_object.element_BO);
	glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

glm::fmat4 ApplicationSolar::uploadPlanetTransforms(planet const& pl) const {
	glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()) * pl.rotation_velocity, glm::fvec3{ 0.0f, 1.0f, 0.0f });
	model_matrix = glm::scale(model_matrix, glm::fvec3{pl.size}); // Scales the matrix depending on the size of the planet
	model_matrix = glm::translate(model_matrix, pl.distance_to_origin);

	for (auto child_moon : pl.moons) {
		// std::cout << "Before: " << glm::to_string(model_matrix) << std::endl;
		child_moon->parent_model_matrix = model_matrix;
	}

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));

	// extra matrix for normal transformation to keep them orthogonal to surface
	glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
		1, GL_FALSE, glm::value_ptr(normal_matrix));

	return model_matrix;
}

void ApplicationSolar::uploadMoonTransforms(moon const& mo, planet const& pl) const {

  glm::fvec3 y_axis = glm::fvec3{0.0f, 1.0f, 0.0f};
  float planet_rot = float(glfwGetTime()) * pl.rotation_velocity;
  float moon_rot = mo.rotation_velocity;

  glm::fmat4 model_matrix{};
	model_matrix = glm::scale(glm::fvec3{mo.size});
	model_matrix = glm::rotate(model_matrix, moon_rot, y_axis);
	model_matrix = glm::translate(model_matrix, mo.distance_to_planet);

	std::cout << "Parent mat: " << glm::to_string(mo.parent_model_matrix) << std::endl;
	model_matrix *= mo.parent_model_matrix;
	// std::cout << "After: " << glm::to_string(model_matrix) << std::endl;

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));

	// extra matrix for normal transformation to keep them orthogonal to surface
	glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
		1, GL_FALSE, glm::value_ptr(normal_matrix));
}


// exe entry point
int main(int argc, char* argv[]) {
	Launcher::run<ApplicationSolar>(argc, argv);
}