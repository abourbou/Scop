
#include "Obj.hpp"

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

	vecVertex.clear();
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

	if (faceVertices.size() < 3)
		throw FileException("Unvalid face : not enough vertices", lineNb);
	CreateTriangle(faceVertices, lineNb);
}

void	Obj::CreateTriangle(const std::vector<Vector3d>& faceVertices, size_t lineNb)
{
	std::vector<Vector3d>	vecEdges;
	size_t					nbVert = faceVertices.size();

	if (nbVert < 3)
		return;
	for (size_t i = 0; i < nbVert; ++i)
		vecEdges.push_back(faceVertices[(i + 1) % nbVert] - faceVertices[i % nbVert]);

	if (!IsPolygonePlane(vecEdges))
		std::cerr << "l" << lineNb << ": face is not plane" << std::endl;
	else if (!IsPolygoneConvex(vecEdges))
		std::cerr << "l" << lineNb << ": face is concave" << std::endl;
	else
		FanTriangulation(faceVertices);
}

void	Obj::FanTriangulation(const std::vector<Vector3d>& faceVertices)
{
	// Use fan triangulation
	for (size_t i = 1; i < faceVertices.size() - 1; ++i)
	{
		// Create triangle
		this->vecTriangle.push_back(faceVertices[0][0]);
		this->vecTriangle.push_back(faceVertices[0][1]);
		this->vecTriangle.push_back(faceVertices[0][2]);
		this->vecTriangle.push_back(faceVertices[i][0]);
		this->vecTriangle.push_back(faceVertices[i][1]);
		this->vecTriangle.push_back(faceVertices[i][2]);
		this->vecTriangle.push_back(faceVertices[i + 1][0]);
		this->vecTriangle.push_back(faceVertices[i + 1][1]);
		this->vecTriangle.push_back(faceVertices[i + 1][2]);

		// Create triangle normal
		auto triangleNormal = (faceVertices[i] - faceVertices[0]).crossProduct(faceVertices[i + 1] - faceVertices[i]);
		triangleNormal.normalize();
		this->vecNormal.push_back(triangleNormal[0]);
		this->vecNormal.push_back(triangleNormal[1]);
		this->vecNormal.push_back(triangleNormal[2]);
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


