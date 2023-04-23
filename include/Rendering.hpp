
#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Obj.hpp"

void	rendering(Obj& obj);

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);