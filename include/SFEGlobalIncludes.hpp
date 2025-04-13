#pragma once

// Prevent GLFW from including gl.h
#define GLFW_INCLUDE_NONE

// Include GLAD first
#include <glad/glad.h>

// Then include GLFW
#include <GLFW/glfw3.h>

// Then include GLEW if needed
#ifdef USE_GLEW
#include <GL/glew.h>
#endif 