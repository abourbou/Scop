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

#include <GL/glew.h>

#include "Matrix.hpp"

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

		//every Triangle are 3 float
		std::vector<GLfloat>		vecTriangle;
		//every normal are 3 float
		std::vector<GLfloat>		vecNormal;
		// Center point of the polygon
		Vector3<GLfloat>			centerPoint;
		// Mean distance from centerPoint
		GLfloat						meanDistCenter;
		// Max distance from centerPoint
		GLfloat						maxDistCenter;

	private:
		std::vector<Vector3<float>>			vecVertex;

	// Utils
	void	ParseLine(std::string line, size_t compt);
	void	ParseVertex(std::stringstream& lineStream, size_t compt);
	void	ParseFace(std::stringstream& lineStream, size_t compt);
	void	CreateTriangle(const std::vector<Vector3f>& vecVertex, size_t lineNb);
	void	FanTriangulation(const std::vector<Vector3f>& faceVertices);
	void	ComputeCenter();
};

// Utils
bool	IsPolygonePlane(const std::vector<Vector3f>& vecEdges);
bool	IsPolygoneConvex(const std::vector<Vector3f>& vecEdges);
