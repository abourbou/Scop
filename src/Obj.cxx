
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
				throw std::invalid_argument( "invalid UV value");
		}
		catch(const std::exception& e)
			{throw FileException("Invalid UV value", lineNb);}
		compt++;
	}
	if (compt != 2)
		throw FileException("Invalid number of arguments for a texture coordinate", lineNb);

	this->vecObjUV.emplace_back(UV);
}

// Parse a face vertice
// Valid format : vertId OR vertId//normalId OR vertId/textCoord/normalId
FaceVertex	Obj::ParseFaceVert(const std::string& faceElem, Vector3f& normal,
								std::set<size_t>& setVertexId)
{
	FaceVertex faceVert;
	size_t id;


	// format vertId
	if (faceElem.find('/') == faceElem.npos)
	{
		id = std::stoul(faceElem);
		if (setVertexId.find(id) != setVertexId.end())
			throw std::invalid_argument("vertex id is duplicated");
		if (id == 0 || id > this->vecObjVertex.size())
			throw std::invalid_argument("vextex id is out of range");
		setVertexId.emplace(id);
		faceVert.vertice = this->vecObjVertex[id-1];
		return faceVert;
	}

	// format vertId//normalId OR vertId/textCoord/normalId
	if (std::count(faceElem.begin(), faceElem.end(), '/') != 2)
		throw std::invalid_argument("invalid number of /");

	std::stringstream tokenStream{faceElem, std::ios_base::in};
	size_t compt = 0;
	std::string token;
	while (getline(tokenStream, token, '/') && compt < 3)
	{
		if (token.empty())
		{
			if (compt == 0 || compt == 2)
				throw std::invalid_argument("empty token in face element");
			++compt;
			continue;
		}
		id = std::stoul(token);
		// Vertex coord
		if (compt == 0)
		{
			if (setVertexId.find(id) != setVertexId.end())
				throw std::invalid_argument("vertex id is duplicated");
			if (id == 0 || id > this->vecObjVertex.size())
				throw std::invalid_argument("vertex id is out of range");
			setVertexId.emplace(id);
			faceVert.vertice = this->vecObjVertex[id-1];
		} // UV coord
		else if (compt == 1)
		{
			if (id == 0 || id > this->vecObjUV.size())
				throw std::invalid_argument("uv id is out of range");
			faceVert.texCoord = this->vecObjUV[id-1];
		} // Normal coord
		else if (compt == 2)
		{
			if (id == 0 || id > this->vecObjNormal.size())
				throw std::invalid_argument("normal id is out of range");
			normal = this->vecObjNormal[id-1];
			break;
		}
		++compt;
	}
	return faceVert;
}

void	Obj::ParseFace(std::stringstream& lineStream, size_t lineNb)
{
	std::string				token;
	std::set<size_t>		setVertexId;
	std::vector<FaceVertex>	faceVertices2;
	Vector3f				normal;

	while (getline(lineStream, token, ' '))
	{
		if (token.empty() || std::isspace(token.front()))
			continue;
		try {faceVertices2.emplace_back(ParseFaceVert(token, normal, setVertexId));}
		catch (const std::exception& e)
		{throw FileException(std::string("invalid face format : ") + e.what(), lineNb);}
	}

	if (faceVertices2.size() < 3)
		throw FileException("Unvalid face : not enough vertices", lineNb);
	this->CreateTriangle(faceVertices2, normal, lineNb);
}

void	Obj::CreateTriangle(const std::vector<FaceVertex>& faceVertices, const Vector3f& normal, size_t lineNb)
{
	std::vector<Vector3f>	vecEdges;
	size_t					nbVert = faceVertices.size();

	for (size_t i = 0; i < nbVert; ++i)
		vecEdges.push_back(faceVertices[(i + 1) % nbVert].vertice - faceVertices[i % nbVert].vertice);

	if (!IsPolygonePlane(vecEdges))
		throw FileException("face is not plane", lineNb);
	else if (!IsPolygoneConvex(vecEdges))
		throw FileException("face is concave", lineNb);
	else
		this->FanTriangulation(faceVertices, normal);
}

void	Obj::FanTriangulation(const std::vector<FaceVertex>& faceVertices, Vector3f normal)
{
	// Use fan triangulation
	for (size_t i = 1; i < faceVertices.size() - 1; ++i)
	{
		// Create triangle
		this->vecTriangle.push_back(faceVertices[0].vertice[0]);
		this->vecTriangle.push_back(faceVertices[0].vertice[1]);
		this->vecTriangle.push_back(faceVertices[0].vertice[2]);
		this->vecTriangle.push_back(faceVertices[i].vertice[0]);
		this->vecTriangle.push_back(faceVertices[i].vertice[1]);
		this->vecTriangle.push_back(faceVertices[i].vertice[2]);
		this->vecTriangle.push_back(faceVertices[i + 1].vertice[0]);
		this->vecTriangle.push_back(faceVertices[i + 1].vertice[1]);
		this->vecTriangle.push_back(faceVertices[i + 1].vertice[2]);

		// Create triangle normal for each vertex
		if (normal[0] == 0.f && normal[1] == 0.f && normal[2] == 0.f)
		{
			normal = (faceVertices[i].vertice - faceVertices[0].vertice)
									.crossProduct(faceVertices[i + 1].vertice - faceVertices[i].vertice);
			normal.normalize();
		}
		for (auto _i = 0; _i < 3; ++_i)
		{
			this->vecNormal.push_back(normal[0]);
			this->vecNormal.push_back(normal[1]);
			this->vecNormal.push_back(normal[2]);
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
