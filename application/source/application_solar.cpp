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

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
	:Application{ resource_path }
	, planet_object{}, planet_vector{}, star_object{}
{
	create_scene();

	initializeGeometry();
	initializeShaderPrograms();
}

float random_number(float min, float max) {
	float rand_num = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	return (max-min) * rand_num;
}

void ApplicationSolar::create_scene() {
	planet Moon{ 0.7f, 8.0f, glm::fvec3{ 5.0f, 0.0f, 1.0f } };

	planet Sun{ 0.5f, 5.5f, glm::fvec3{ 0.0, 0.0f, 0.0f } };
	planet Mercury{ 0.1f, 15.0f, glm::fvec3{ 15.0f, 0.0f, 15.0f } };
	planet Venus{ 0.1f, 0.8f, glm::fvec3{ 10.6f, 0.0f, 10.6f } };
	planet Earth{ 0.07f, 1.5f, glm::fvec3{ 10.0f, 0.0f, 10.0f } };
	planet Mars{ 0.5f, 2.0f, glm::fvec3{ 25.1f, 0.0f, 25.1f } };
	planet Jupiter{ 0.1f, 0.7f, glm::fvec3{ 35.2f, 0.0f, 35.5f } };
	planet Saturn{ 1.5f, 3.0f, glm::fvec3{ 20.0f, 0.0f, 20.0f } };
	planet Uranus{ 0.2f, 4.5f, glm::fvec3{ 22.0f, 0.1f, 23.45f } };
	planet Neptun{ 0.6f, 0.3f, glm::fvec3{ 30.5f, 0.0f, 30.5f } };

	planet_vector.push_back(Earth);
	planet_vector.push_back(Moon);
	planet_vector.push_back(Sun);
	planet_vector.push_back(Mercury);
	planet_vector.push_back(Venus);
	planet_vector.push_back(Mars);
	planet_vector.push_back(Jupiter);
	planet_vector.push_back(Saturn);
	planet_vector.push_back(Uranus);


	// initialise 10000 stars at pseudo-random positions.
	// star colors depend on their position
	// lambda returns random value between 0.0f and 1.0f

	float x, y, z, r, g, b;
	std::srand(std::time(nullptr));
	const float star_range = 100.0f;

	for(int i=0; i < 10000; ++i) {
		x = random_number(-star_range, star_range);
		y = random_number(-star_range, star_range);
		z = random_number(-star_range, star_range);

		r = std::abs(std::sin(x));
		g = std::abs(std::cos(y));
		b = std::abs(std::sin(z));
		star_vector.push_back({x, y, z, r, g, b});
	}
}

void ApplicationSolar::render() const {
	// bind shader to upload uniforms
	glUseProgram(m_shaders.at("planet").handle);

	// earth and moon get a special treatment for the moment
	glm::fmat4 earth_mat = uploadPlanetTransforms(planet_vector.front());
	planet Moon = planet_vector.at(1);
	uploadMoonTransforms(Moon, earth_mat);

	// Draw for all predefined planets in planet_vector depending on their attributes
	for (auto i = planet_vector.begin()+2; i != planet_vector.end(); ++i) {
		uploadPlanetTransforms(*i);
	}

	//Stars:
	//use star shader
	glUseProgram(m_shaders.at("star").handle);
	glm::fmat4 star_mat = glm::scale(glm::fmat4{}, glm::fvec3{15.0});
	star_mat = glm::translate(star_mat, glm::fvec3{-0.5, -0.5, -0.5});
	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(star_mat));

	glBindVertexArray(star_object.vertex_AO);
	glDrawArrays(star_object.draw_mode, 0, star_vector.size());
}

void ApplicationSolar::updateView() {
	// vertices are transformed in camera space, so camera transform must be inverted
	glm::fmat4 view_matrix = glm::inverse(m_view_transform);
	glm::fvec3 sun_pos = glm::fvec3(view_matrix * glm::fvec4{0.0, 0.0, 0.0, 1.0});

	// upload matrix to gpu
	glUseProgram(m_shaders.at("planet").handle);
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
		1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniform3f(m_shaders.at("planet").u_locs.at("SunViewPos"), sun_pos.x, sun_pos.y, sun_pos.z);

	glUseProgram(m_shaders.at("star").handle);
	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
		1, GL_FALSE, glm::value_ptr(view_matrix));

	glUseProgram(m_shaders.at("planet").handle);
}

void ApplicationSolar::updateProjection() {
	// upload matrix to gpu
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
		1, GL_FALSE, glm::value_ptr(m_view_projection));

	glUseProgram(m_shaders.at("star").handle);
	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
		1, GL_FALSE, glm::value_ptr(m_view_projection));
	glUseProgram(m_shaders.at("planet").handle);
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
	updateUniformLocations();

	glUseProgram(m_shaders.at("planet").handle);
	updateView();
	updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, -0.1f });
		updateView();
	}	else if (key == GLFW_KEY_S) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, 0.1f });
		updateView();
	}	else if (key == GLFW_KEY_A) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -0.1f, 0.0f, 0.0f });
		updateView();
	}	else if (key == GLFW_KEY_D) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.1f, 0.0f, 0.0f });
		updateView();
	}
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
	// mouse handling

	float x_mov = 0.001f * static_cast<float>(pos_x);
	float y_mov = 0.001f * static_cast<float>(pos_y);
	m_view_transform = glm::rotate(m_view_transform, x_mov, glm::fvec3{ 0.0f, -1.0f, 0.0f });
	m_view_transform = glm::rotate(m_view_transform, y_mov, glm::fvec3{ -1.0f, 0.0f, 0.0f });
	updateView();
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
	m_shaders.at("planet").u_locs["SunViewPos"] = -1;


	// star uniforms:
	m_shaders.emplace("star", shader_program{ m_resource_path + "shaders/star.vert",
		m_resource_path + "shaders/star.frag" });
	m_shaders.at("star").u_locs["ViewMatrix"] = -1;
	m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
	m_shaders.at("star").u_locs["ModelMatrix"] = -1;
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

	// Stars:
	glGenVertexArrays(1, &star_object.vertex_AO);
	glBindVertexArray(star_object.vertex_AO);

	glGenBuffers(1, &star_object.vertex_BO);
	glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(star) * star_vector.size(), star_vector.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, NULL);

	star_object.draw_mode = GL_POINTS;
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

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));

	// extra matrix for normal transformation to keep them orthogonal to surface
	glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
		1, GL_FALSE, glm::value_ptr(normal_matrix));

	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

	return model_matrix;
}


void ApplicationSolar::uploadMoonTransforms(planet const& mo, glm::fmat4 const& parent_mat) const {

	glm::fmat4 moon_mat = glm::rotate(parent_mat, float(glfwGetTime()) * mo.rotation_velocity, glm::fvec3{0.0f, 1.0f, 0.0f});
	moon_mat = glm::scale(moon_mat, glm::fvec3{mo.size});
	moon_mat = glm::translate(moon_mat, mo.distance_to_origin);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(moon_mat));

	// extra matrix for normal transformation to keep them orthogonal to surface
	glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * moon_mat);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
		1, GL_FALSE, glm::value_ptr(normal_matrix));

	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}


// exe entry point
int main(int argc, char* argv[]) {
	Launcher::run<ApplicationSolar>(argc, argv);
}