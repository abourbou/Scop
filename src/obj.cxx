
#include "obj.hpp"

Obj::Obj(std::string fileName)
{
	std::fstream file;
	size_t lineNb = 1;

	file.open(fileName, std::ios::in);
	if (file.is_open())
	{
		std::string line;
		while(getline(file, line))
		{
			ParseLine({line}, lineNb);
			++lineNb;
		}
		file.close();
   }
   else
	throw FileException("can not read file");
}

void Obj::ParseLine(std::string line, size_t lineNb)
{
	std::stringstream lineStream{line, std::ios_base::in};
	std::string	token;

	if (!getline(lineStream, token, ' '))
		return;
	if (token == "v")
		ParseVertex(lineStream, lineNb);
	else if (token == "f")
		ParseFace(lineStream, lineNb);
	else if (token.front() != '#')
		std::cout << "Unknown element at l" << lineNb << std::endl;
}

void	Obj::ParseVertex(std::stringstream& lineStream, size_t lineNb)
{
	std::string	token;
	Vector3d	vertex;
	size_t		compt = 0;

	while (getline(lineStream, token, ' '))
	{
		if (token.size() == 0)
			continue;
		try
			{vertex[compt] = std::stod(token);}
		catch(const std::exception& e)
			{throw FileException("Invalid vertex value", lineNb);}
		compt++;
	}
	if (compt != 3)
		throw FileException("Invalid number of arguments for a vertex", lineNb);

	std::cout << "vertex " << vecVertex.size() + 1 << " : " << transpose(vertex) << std::endl;
	vecVertex.emplace_back(vertex);
}

void	Obj::ParseFace(std::stringstream& lineStream, size_t lineNb)
{
	std::string				token;
	std::set<size_t>		setVertexId;
	std::vector<Vector3d>	faceVertices;

	while (getline(lineStream, token, ' '))
	{
		size_t	id;
		try
			{id = std::stoul(token);}
		catch (const std::exception& e)
			{throw FileException("invalid face value", lineNb);}

		if (setVertexId.find(id) != setVertexId.end())
			throw FileException("duplicate vertex id for a face", lineNb);
		if (id > this->vecVertex.size())
			throw FileException("non existing vertex id for a face", lineNb);
		setVertexId.emplace(id);
		faceVertices.push_back(this->vecVertex[id - 1]);
	}

	//!
	std::cout << std::endl;
	//!
	static size_t compt = 1;
	std::cout << "face " << compt << std::endl;
	++compt;
	if (faceVertices.size() < 3)
		throw FileException("Unvalid face : not enough vertices", lineNb);
	Triangulation(faceVertices, lineNb);
}

void	Obj::Triangulation(const std::vector<Vector3d>& faceVertices, size_t lineNb)
{
	std::vector<Vector3d>	vecEdges;
	size_t					nbVert = faceVertices.size();

	if (nbVert < 3)
		return;
	std::cout << "nbVert : " << nbVert << std::endl;
	for (size_t i = 0; i < nbVert; ++i)
	{
		vecEdges.push_back(faceVertices[(i + 1) % nbVert] - faceVertices[i % nbVert]);
		std::cout << "vecEdges[" << i << "] : " << transpose(vecEdges.back()) << std::endl;
	}

	if (!IsPolygonePlane(vecEdges))
		std::cerr << "l" << lineNb << ": face is not plane" << std::endl;
	else if (!IsPolygoneConvex(vecEdges))
		std::cerr << "l" << lineNb << ": face is concave" << std::endl;
	else
		CreateTriangle(faceVertices);
}

void	Obj::CreateTriangle(const std::vector<Vector3d>& faceVertices)
{
	std::cout << "Create Triangle!" << std::endl;
	static int id = 0;

	++id;
	for (size_t i = 1; i < faceVertices.size() - 1; ++i)
	{
		Triangle triangle;
		triangle.vertices[0] = faceVertices[0];
		triangle.vertices[1] = faceVertices[i];
		triangle.vertices[2] = faceVertices[i + 1];
		triangle.vertexNormal = (triangle.vertices[1] - triangle.vertices[0]).crossProduct(triangle.vertices[2] - triangle.vertices[1]);
		triangle.vertexNormal.normalize();

		this->mapTriangle.insert(std::pair<int, Triangle>(id, triangle));
		std::cout << "Triangle(" << id << ") : " << transpose(triangle.vertices[0])
				  << ", " << transpose(triangle.vertices[1])
				  << ", " << transpose(triangle.vertices[2]) << std::endl;
		std::cout << "normal(" << triangle.vertexNormal.euclidNorm() << ") : " << transpose(triangle.vertexNormal) << std::endl;
	}
}


//*********************
//******* Utils *******
//*********************

bool	IsPolygonePlane(const std::vector<Vector3d>& vecEdges)
{
	Vector3d	normal = vecEdges[0].crossProduct(vecEdges[1]);

	for (size_t i = 2; i < vecEdges.size(); ++i)
	{
		if (std::abs(normal.dotProduct(vecEdges[i])) > 1e-8)
			return false;
	}

	return true;
}

bool	IsPolygoneConvex(const std::vector<Vector3d>& vecEdges)
{
	Vector3d	normal = vecEdges[0].crossProduct(vecEdges[1]);

	for (size_t i = 1; i < vecEdges.size(); ++i)
	{
		auto cross = vecEdges[i].crossProduct(vecEdges[(i + 1) % vecEdges.size()]);
		auto dot = normal.dotProduct(cross);
		if (dot < 0)
			return false;
	}

	return true;
}


