
#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Obj.hpp"
#include "Transformations.hpp"

void	rendering(Obj& obj);

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);