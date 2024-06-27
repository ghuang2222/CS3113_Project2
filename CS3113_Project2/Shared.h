#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

constexpr int WINDOW_WIDTH = 800,
			  WINDOW_HEIGHT = 500;

constexpr glm::vec3 ZERO_VEC = glm::vec3(0.0f, 0.0f, 0.0f);

constexpr float X_LIMIT = 5, Y_LIMIT = 3.25f;

