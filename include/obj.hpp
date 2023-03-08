#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "matrix.hpp"

//? Obj type handles :
//* v vertex
//TODO vt maybe later
//* vn vertex normal
//* f face
//! mtlib
//! usemtfl
//! s
//TODO o maybe later
//TODO g maybe later

struct Triangle
{
	Vector3d vertices;
	Vector3d vertexNormal;
};

class FileException : public std::exception {
	public:
		const char * what () const throw ()
		{
			return "Error reading the file";
		}
};

class Obj
{
	public :
		Obj() = delete;
		Obj(std::string fileName);

	private:
		std::multimap<int, Triangle> mapTriangle;
};
