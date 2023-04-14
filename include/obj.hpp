#pragma once

#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "matrix.hpp"

//? Obj type handles :
//* v vertex
//TODO vt maybe later
//! vn vertex normal
//* f face
//! mtlib
//! usemtfl
//! s
//TODO o maybe later
//TODO g maybe later

struct Triangle
{
	Vector3d vertices[3];
	Vector3d vertexNormal;
};

class FileException : public std::exception {
	public:
		FileException() = default;
		FileException(std::string msg, size_t lineNb = 0)
		{
			if (lineNb  != 0)
			{
				Msg += " l ";
				Msg += std::to_string(lineNb);
			}
			Msg += " : ";
			Msg += msg;
		}

		const char * what () const throw ()
		{
			return this->Msg.c_str();
		}

	private:
		std::string Msg = "Error File";
};

class Obj
{
	public :
		Obj() = delete;
		Obj(std::string fileName);

	private:
		std::vector<Vector3d>			vecVertex;
		std::multimap<int, Triangle>	mapTriangle;

	// Utils
	void	ParseLine(std::string line, size_t compt);
	void	ParseVertex(std::stringstream& lineStream, size_t compt);
	void	ParseFace(std::stringstream& lineStream, size_t compt);
	void	Triangulation(const std::vector<Vector3d>& vecVertex, size_t lineNb);
	void	CreateTriangle(const std::vector<Vector3d>& faceVertices);
};

// Utils
bool	IsPolygonePlane(const std::vector<Vector3d>& vecEdges);
bool	IsPolygoneConvex(const std::vector<Vector3d>& vecEdges);
