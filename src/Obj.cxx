
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
			try {this->ParseLine({line}, lineNb);}
			catch(const std::exception& e)
			{PRINT_WARNING(e.what());}
			++lineNb;
		}
		file.close();
	}
	else
		throw FileException("can not read file");

	this->ComputeCenter();

	// Free obj buffer memory
	vecObjVertex.clear();
	vecObjNormal.clear();
	vecObjUV.clear();
}

void Obj::ParseLine(std::string line, size_t lineNb)
{
	std::stringstream lineStream{line, std::ios_base::in};
	std::string	token;

	if (!getline(lineStream, token, ' '))
		return;
	if (token == "v")
		this->ParseVertex(lineStream, lineNb);
	else if (token == "vn")
		this->ParseNormal(lineStream, lineNb);
	else if (token == "vt")
		this->ParseTextureCoord(lineStream, lineNb);
	else if (token == "f")
		this->ParseFace(lineStream, lineNb);
	else if (token.front() != '#')
		throw FileException("Unknown element \"" + token + "\"", lineNb);
}

void	Obj::ParseVertex(std::stringstream& lineStream, size_t lineNb)
{
	std::string	token;
	Vector3f	vertex;
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

	this->vecObjVertex.emplace_back(vertex);
}

void	Obj::ParseNormal(std::stringstream& lineStream, size_t lineNb)
{
	std::string	token;
	Vector3f	normal;
	size_t		compt = 0;

	while (getline(lineStream, token, ' '))
	{
		if (token.size() == 0)
			continue;
		try
			{normal[compt] = std::stod(token);}
		catch(const std::exception& e)
			{throw FileException("Invalid normal value", lineNb);}
		compt++;
	}
	if (compt != 3)
		throw FileException("Invalid number of arguments for a normal", lineNb);

	this->vecObjNormal.emplace_back(normal);
}

void	Obj::ParseTextureCoord(std::stringstream& lineStream, size_t lineNb)
{
	std::string		token;
	Vector<float,2>	UV;
	size_t		compt = 0;

	while (getline(lineStream, token, ' '))
	{
		if (token.size() == 0)
			continue;
		try
		{
			UV[compt] = std::stod(token);
			if (UV[compt] < 0. || UV[compt] > 1.)
				throw std::invalid_argument( "invalid value for UV");
		}
		catch(const std::exception& e)
			{throw FileException("Invalid UV value", lineNb);}
		compt++;
	}
	if (compt != 2)
		throw FileException("Invalid number of arguments for a texture coordinate", lineNb);

	this->vecObjUV.emplace_back(UV);
}

void	Obj::ParseFace(std::stringstream& lineStream, size_t lineNb)
{
	std::string				token;
	std::set<size_t>		setVertexId;
	std::vector<Vector3f>	faceVertices;

	while (getline(lineStream, token, ' '))
	{
		size_t	id;

		if (token.size() == 0)
			continue;
		try
			{id = std::stoul(token);}
		catch (const std::exception& e)
			{throw FileException("invalid face value", lineNb);}

		if (setVertexId.find(id) != setVertexId.end())
			throw FileException("duplicate vertex id for a face", lineNb);
		if (id > this->vecObjVertex.size())
			throw FileException("non existing vertex id for a face", lineNb);
		setVertexId.emplace(id);
		faceVertices.push_back(this->vecObjVertex[id - 1]);
	}

	if (faceVertices.size() < 3)
		throw FileException("Unvalid face : not enough vertices", lineNb);
	this->CreateTriangle(faceVertices, lineNb);
}

void	Obj::CreateTriangle(const std::vector<Vector3f>& faceVertices, size_t lineNb)
{
	std::vector<Vector3f>	vecEdges;
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
		this->FanTriangulation(faceVertices);
}

void	Obj::FanTriangulation(const std::vector<Vector3f>& faceVertices)
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

		// Create triangle normal for each vertex
		auto triangleNormal = (faceVertices[i] - faceVertices[0]).crossProduct(faceVertices[i + 1] - faceVertices[i]);
		triangleNormal.normalize();
		for (auto _i = 0; _i < 3; ++_i)
		{
			this->vecNormal.push_back(triangleNormal[0]);
			this->vecNormal.push_back(triangleNormal[1]);
			this->vecNormal.push_back(triangleNormal[2]);
		}
	}
}


//*********************
//******* Utils *******
//*********************

bool	IsPolygonePlane(const std::vector<Vector3f>& vecEdges)
{
	Vector3f	normal = vecEdges[0].crossProduct(vecEdges[1]);

	for (size_t i = 2; i < vecEdges.size(); ++i)
	{
		if (std::abs(normal.dotProduct(vecEdges[i])) > 5e-2)
			return false;
	}

	return true;
}

bool	IsPolygoneConvex(const std::vector<Vector3f>& vecEdges)
{
	Vector3f	normal = vecEdges[0].crossProduct(vecEdges[1]);

	for (size_t i = 1; i < vecEdges.size(); ++i)
	{
		auto cross = vecEdges[i].crossProduct(vecEdges[(i + 1) % vecEdges.size()]);
		auto dot = normal.dotProduct(cross);
		if (dot < 0)
			return false;
	}

	return true;
}

void	Obj::ComputeCenter()
{
	Vector3<GLfloat>	vecCenter(0);

	for (const auto& vertex : this->vecObjVertex)
		vecCenter = vecCenter + vertex;
	vecCenter = vecCenter * (1 / (GLfloat)this->vecObjVertex.size());
	this->centerPoint[0] = vecCenter[0];
	this->centerPoint[1] = vecCenter[1];
	this->centerPoint[2] = vecCenter[2];

	this->maxDistCenter = 0;
	this->meanDistCenter = 0;
	for (const auto& vertex : this->vecObjVertex)
	{
		auto distCenter = (vertex - vecCenter).euclidNorm();
		meanDistCenter += distCenter;
		if (distCenter > maxDistCenter)
			maxDistCenter = distCenter;
	}
	meanDistCenter /= (GLfloat)this->vecObjVertex.size();
}
