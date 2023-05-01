#pragma once

#include <algorithm>
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
//* vt maybe later
//* vn vertex normal
//* f face
//! mtlib
//! usemtfl
//! s
//TODO o maybe later
//TODO g maybe later

#define DEFAULT_COLOR "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

#define PRINT_INFO(s) std::cout << GREEN << s << DEFAULT_COLOR << std::endl;
#define PRINT_WARNING(s) std::cerr << YELLOW << "[WARNING] " << s << DEFAULT_COLOR << std::endl;
#define PRINT_ERROR(s)   std::cerr << RED    << "[ERROR] "   << s << DEFAULT_COLOR << std::endl;

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

struct FaceVertex
{
	Vector3f			vertice;
	Vector<float, 2>	texCoord{-1};
};

class Obj
{
	public :
		Obj() = delete;
		Obj(std::string fileName);

		// Every Triangle are 3 float
		std::vector<GLfloat>		vecTriangle;
		// Every normal are 3 float
		std::vector<GLfloat>		vecNormal;
		// Every UV are 2 float
		std::vector<GLfloat>		vecUV;

		// Center point of the polygon
		Vector3<GLfloat>			centerPoint;
		// Mean distance from centerPoint
		GLfloat						meanDistCenter;
		// Max distance from centerPoint
		GLfloat						maxDistCenter;

	private:
		std::vector<Vector3f>			vecObjVertex;
		std::vector<Vector3f>			vecObjNormal;
		std::vector<Vector<float,2>>	vecObjUV;

	// Utils
	void	ParseLine(std::string line, size_t compt);
	void	ParseVertex(std::stringstream& lineStream, size_t compt);
	void	ParseNormal(std::stringstream& lineStream, size_t compt);
	void	ParseTextureCoord(std::stringstream& lineStream, size_t compt);
	void	ParseFace(std::stringstream& lineStream, size_t compt);
	FaceVertex	ParseFaceVert(const std::string& faceElem, Vector3f& normal,
								std::set<size_t>& setVertexId);
	void	CreateTriangle(const std::vector<FaceVertex>& vecObjVertex, const Vector3f& normal, size_t lineNb);
	void	FanTriangulation(const std::vector<FaceVertex>& faceVertices, Vector3f normal);
	void	ComputeCenter();
};

// Utils
bool	IsPolygonePlane(const std::vector<Vector3f>& vecEdges);
bool	IsPolygoneConvex(const std::vector<Vector3f>& vecEdges);
