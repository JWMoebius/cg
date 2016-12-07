#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"

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

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>


ApplicationSolar::ApplicationSolar(std::string const& resource_path)
	:Application{ resource_path }
	, planet_object{}, planet_vector{}, star_object{}
{
	create_scene();

	initializeTextures();
	initializeGeometry();
	initializeShaderPrograms();
}

ApplicationSolar::~ApplicationSolar() {
	glDeleteBuffers(1, &planet_object.vertex_BO);
	glDeleteBuffers(1, &planet_object.element_BO);
	glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::create_scene() {


	planet_ptr Sun = std::make_shared<planet>(planet{ 0.5f, 0.0f,  glm::fvec3{ 0.0, 0.0f, 0.0f }, nullptr, glm::fmat4{}, "textures/sun.png" });
	planet_ptr Mercury = std::make_shared<planet>(planet{ 0.2f, 3.9f, glm::fvec3{ 20.0f, 0.0f, 20.0f }, Sun, glm::fmat4{}, "textures/mercury.png" });
	planet_ptr Venus = std::make_shared<planet>(planet{ 0.2f, 0.1f,  glm::fvec3{ 17.24f, 0.0f, 17.24f }, Sun, glm::fmat4{}, "textures/venus.png" });
	planet_ptr Earth = std::make_shared<planet>(planet{ 0.14f, 0.9f, glm::fvec3{ 14.0f, 0.0f, 14.0f }, Sun, glm::fmat4{}, "textures/earth.png" });
	planet_ptr Moon = std::make_shared<planet>(planet{ 0.4f, 3.0f,  glm::fvec3{ 6.0f, 0.0f, 6.0f }, Earth, glm::fmat4{}, "textures/moon.png" });
	planet_ptr Mars = std::make_shared<planet>(planet{ 1.0f, 2.0f,  glm::fvec3{ 50.2f, 0.0f, 50.2f }, Sun, glm::fmat4{}, "textures/mars.png" });
	planet_ptr Jupiter = std::make_shared<planet>(planet{ 0.2f, 0.7f,  glm::fvec3{ 33.4f, 0.0f, 33.4f }, Sun, glm::fmat4{}, "textures/jupiter.png" });
	planet_ptr Saturn = std::make_shared<planet>(planet{ 3.0f, 3.0f,  glm::fvec3{ 40.0f, 0.0f, 40.0f }, Sun, glm::fmat4{}, "textures/saturn.png" });
	planet_ptr Uranus = std::make_shared<planet>(planet{ 0.4f, 2.8f,  glm::fvec3{ 44.0f, 0.2f, 46.9f }, Sun, glm::fmat4{}, "textures/uranus.png" });
	planet_ptr Neptune = std::make_shared<planet>(planet{ 1.2f, 0.3f,  glm::fvec3{ 49.0f, 0.0f, 49.0f }, Sun, glm::fmat4{}, "textures/neptune.png" });
	planet_ptr Skybox = std::make_shared<planet>(planet{ 200.0f, 0.0f,  glm::fvec3{ 0.0f, 0.0f, 0.0f }, Sun, glm::fmat4{}, "textures/skybox.png" });

	planet_vector.push_back(Sun);
	planet_vector.push_back(Mercury);
	planet_vector.push_back(Venus);
	planet_vector.push_back(Earth);
	planet_vector.push_back(Moon);
	planet_vector.push_back(Mars);
	planet_vector.push_back(Jupiter);
	planet_vector.push_back(Saturn);
	planet_vector.push_back(Uranus);
	planet_vector.push_back(Skybox);

	// initialise 100000 stars at pseudo-random positions.
	// star colors depend on their position

	float x, y, z, r, g, b;
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	const float star_range = 100.0f;

	for (int i = 0; i < 100000; ++i) {
		// this should work theoretically but doesn't.
		do {
			x = utils::random_number(-star_range, star_range);
			y = utils::random_number(-star_range, star_range);
			z = utils::random_number(-star_range, star_range);
		} while (x*x + y*y + z*z > star_range*star_range); // toss out point if it's not in the sphere

		r = std::abs(std::sin(x));
		g = std::abs(std::cos(y));
		b = std::abs(std::sin(z));
		star_vector.push_back({ x, y, z, r, g, b });
	}
}

void ApplicationSolar::render() const {

	// Draw for all predefined planets in planet_vector depending on their attributes
	unsigned i = 0;
	for (auto pl : planet_vector) {
		if (pl->parent != nullptr) {
			pl->transform = uploadPlanetTransforms(*pl, i + 1, pl->parent->transform);
		}
		else {
			pl->transform = uploadPlanetTransforms(*pl, i + 1, glm::fmat4{});
		}
		++i;
	}

	//Stars:
	uploadStarTransforms();
	glUseProgram(m_shaders.at("planet").handle);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(planet_object.vertex_AO);
	glDisable(GL_DEPTH_TEST);

	//glUseProgram(m_shaders.at("quad").handle);

}

void ApplicationSolar::updateView() {
	// vertices are transformed in camera space, so camera transform must be inverted
	glm::fmat4 view_matrix = glm::inverse(m_view_transform);
	glm::fvec3 sun_pos = glm::fvec3(view_matrix * glm::fvec4{ 0.0, 0.0, 0.0, 1.0 });

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
	// updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, -0.1f });
		updateView();
	}
	else if (key == GLFW_KEY_S) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, 0.1f });
		updateView();
	}
	else if (key == GLFW_KEY_A) {
		m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -0.1f, 0.0f, 0.0f });
		updateView();
	}
	else if (key == GLFW_KEY_D) {
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
	m_shaders.emplace("quad", shader_program{ m_resource_path + "shaders/quad.vert",
		m_resource_path + "shaders/quad.frag" });

	// store shader program objects in container
	m_shaders.emplace("planet", shader_program{ m_resource_path + "shaders/simple.vert",
		m_resource_path + "shaders/simple.frag" });
	// request uniform locations for shader program
	m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
	m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
	m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
	m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
	m_shaders.at("planet").u_locs["SunViewPos"] = -1;
	m_shaders.at("planet").u_locs["ColorTex"] = -1;
	m_shaders.at("planet").u_locs["NumPlanet"] = -1;

	// star uniforms:
	m_shaders.emplace("star", shader_program{ m_resource_path + "shaders/star.vert",
		m_resource_path + "shaders/star.frag" });
	m_shaders.at("star").u_locs["ViewMatrix"] = -1;
	m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
	m_shaders.at("star").u_locs["ModelMatrix"] = -1;
}
/* Wait till confirmed working
void ApplicationSolar::initializeFramebuffer() {
	GLuint framebuffer;
	GLuint renderbuffer;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 800);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT4,
		framebuffer,
		0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, renderbuffer);

	GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(1, draw_buffers);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "It hits the fan! Framebuffer is faulty.";
	}
}
*/
// load models
void ApplicationSolar::initializeGeometry() {
	model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);

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

	// texture coords:
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);

	star_object.draw_mode = GL_POINTS;
}


void ApplicationSolar::initializeTextures() const {
	unsigned i = 1;
	for (auto pl : planet_vector) {
		pixel_data planet_pxdat = texture_loader::file(m_resource_path + pl->texture_path);

		glActiveTexture(GL_TEXTURE0 + i);
		texture_object planet_tex = utils::create_texture_object(planet_pxdat);
		GLuint framebuffer;
		GLuint renderbuffer;
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT4,
			framebuffer,
			0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, renderbuffer);

		GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(1, draw_buffers);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "It hits the fan! Framebuffer is faulty.";
		}


		++i;
	}
}


glm::fmat4 ApplicationSolar::uploadPlanetTransforms(planet const& pl, const unsigned index, glm::fmat4 const& parent_mat) const {
	
	// glm::fmat4 parent_pos = glm::translate(glm::fmat4{}, glm::fvec3(parent_mat[3]));
	glm::fmat4 model_matrix = glm::rotate(parent_mat, float(glfwGetTime()) * pl.rotation_velocity, glm::fvec3{ 0.0f, 1.0f, 0.0f });
	model_matrix = glm::scale(model_matrix, glm::fvec3{ pl.size }); // Scales the matrix depending on the size of the planet
	model_matrix = glm::translate(model_matrix, pl.distance_to_origin);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));

	// extra matrix for normal transformation to keep them orthogonal to surface
	glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
		1, GL_FALSE, glm::value_ptr(normal_matrix));

	// give index of planet texture to fragment shader
	glUniform1i(m_shaders.at("planet").u_locs.at("ColorTex"), index);
	//give current planet numbber
	glUniform1i(m_shaders.at("planet").u_locs.at("NumPlanet"), index);

	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	return model_matrix;
}


void ApplicationSolar::uploadStarTransforms() const {
	//use star shader
	
	glm::fmat4 star_mat = glm::scale(glm::fmat4{}, glm::fvec3{ 100.0 }); // spread out the stars
	star_mat = glm::translate(star_mat, glm::fvec3{ -0.5, -0.5, -0.5 });
	glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(star_mat));

	glBindVertexArray(star_object.vertex_AO);
	glDrawArrays(star_object.draw_mode, 0, star_vector.size());
	glUseProgram(m_shaders.at("star").handle);
}

// exe entry point
int main(int argc, char* argv[]) {
	Launcher::run<ApplicationSolar>(argc, argv);
}