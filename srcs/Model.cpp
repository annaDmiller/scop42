#include <Model.hpp>

Model::Model()
    : _vao(0), _vbo(0), _loaded(false), _uploaded(false)
{
    Material defaultMaterial;
    addMaterial(defaultMaterial);
}

Model::Model(const std::string& pathToFile)
    : _vao(0), _vbo(0), _loaded(false), _uploaded(false),
    _pathToFile(pathToFile)
{
    Material defaultMaterial;
    addMaterial(defaultMaterial);
    load(pathToFile);
}

Model::~Model()
{   }

void Model::clearCPUData()
{
    _positions.clear();
    _texCoords.clear();
    _normals.clear();
    _faces.clear();
    _matLibs.clear();
    _materials.clear();
    _materialLookup.clear();
    _vertices.clear();
    _subMeshes.clear();

    Material defaultMaterial;
    addMaterial(defaultMaterial);

    _loaded = false;
    _error.clear();
}

void Model::setError(const std::string& msg, unsigned int lineNumber)
{
    std::ostringstream out;
    if (lineNumber != 0)
        out << "Line " << lineNumber << ": ";
    out << msg;
    _error = out.str();
}

bool Model::load(const std::string& filename)
{
    releaseGPU();
    clearCPUData();

    _pathToFile = filename;

    if (!parseOBJ(filename))
        return false;
}

typedef bool (Model::*ParserFunction)(std::stringstream&, unsigned int);

bool Model::parseOBJ(const std::string& filename)
{
    std::ifstream file(filename.c_str());

    if (!file.is_open())
    {
        std::cerr << "ERROR: parseOBJ: " << std::endl;
        setError("Could not open OBJ file: " + filename);
        return false;
    }

    std::string line;
    std::string currMaterial;
    unsigned int lineNumber = 0;

    std::map<std::string, ParserFunction> types = {
        {"v", &Model::parseVertex}, // v = Vertex coordinates as floats
        {"vt", &Model::parseTexCoord}, // vt = Texture coordinates as floats
        {"vn", &Model::parseNormal}, // vn = Normals as floats
        {"f", &Model::parseFace}, // f = Faces : collection of indices which creates a face of the model
        {"o", &Model::parseObjectName}, // o = Object name as string
        {"mtllib", &Model::parseMtlLib}, // mtllib = Material Library : path to the file which contains material information
        {"usemtl", &Model::parseUseMtl} // usemtl = Name of the material to use
    };
    
    while (std::getline(file, line))
    {
        ++lineNumber;
        remove_if(line.begin(), line.end(), isspace);
        if (line.empty() || line[0] == '#')
            continue ;

        std::stringstream ss(line);
        std::string type;
        ss >> type;
        
        std::map<std::string, ParserFunction>::iterator it = types.find(type);
        if (it != types.end())
        {
            if (!(this->*(it->second))(ss, lineNumber))
            {
                std::cerr << "ERROR: parseOBJ: " << std::endl;
                return false;
            }
        }
    }

    if (file.bad())
    {
        setError("I/O error while reading OBJ file.");
        return false;
    }

    return true;
}

bool Model::parseVertex(std::stringstream& ss, unsigned int lineNumber)
{
    float x, y, z;
    if (!(ss >> x >> y >> z))
    {
        setError("Vertex has less than 3 coordinates.", lineNumber);
        return false;
    }
    
    Vec3 vertex = Vec3(x, y, z);
    if (!(isValidVector(vertex)))
    {
        setError("Vertex vector has invalid coordinates.", lineNumber);
        return false;
    }

    _positions.push_back(vertex);
    return true;
}

bool Model::parseTexCoord(std::stringstream& ss, unsigned int lineNumber)
{
    float x, y;
    if (!(ss >> x >> y))
    {
        setError("Texture vector has less than 2 coordinates.", lineNumber);
        return false;
    }

    Vec2 texCoord = Vec2(x, y);
    if (!(isValidVector(texCoord)))
    {
        setError("Texture vector has invalid coordinates.", lineNumber);
        return false;
    }

    _texCoords.push_back(texCoord);
    return true;
}

bool Model::parseNormal(std::stringstream& ss, unsigned int lineNumber)
{
    float x, y, z;
    if (!(ss >> x >> y >> z))
    {
        setError("Vertex normal has less than 3 coordinates.", lineNumber);
        return false;
    }
    
    Vec3 normal = Vec3(x, y, z);
    if (!(isValidVector(normal)))
    {
        setError("Vertex normal has invalid coordinates.", lineNumber);
        return false;
    }

    if (normal.length() <= 1.0e-8f)
    {
        setError("Vertex normal has 0 length.", lineNumber);
        return false;
    }

    _normals.push_back(normal);
    return true;
}

bool Model::parseObjectName(std::stringstream& ss, unsigned int lineNumber)
{
    std::string name;
    if (!(ss >> name))
        _objectName = "SCOP";
    else
        _objectName = name;
    
    return true;
}

bool Model::parseMtlLib(std::stringstream& ss, unsigned int lineNumber)
{
    std::string lib;
    while (ss >> lib)
        _matLibs.push_back(lib);

    if (_matLibs.empty())
    {
        setError("mtllib directive does not specify the material library filename.", lineNumber);
        return false;
    }

    return true;
}

bool Model::parseUseMtl(std::stringstream& ss, unsigned int lineNumber)
{
    if (!(ss >> _currMat))
    {
        setError("usemtl directive does not specify a material name to use.", lineNumber);
        return false;
    }
    return true;
}

bool Model::parseFace(std::stringstream& ss, unsigned int lineNumber)
{
    std::string token;
    Face face;
    face.matName = _currMat;

    while (ss >> token)
    {
        if (!token.empty() && token[0] == '#')
            break ;

        FaceVertex faceVertex;
        if (!parseFaceVertex(token, faceVertex, lineNumber))
            return false;

        face.vertices.push_back(faceVertex);
    }
    
    if (face.vertices.size() < 3)
    {
        setError("A face must contain at least three vertices.", lineNumber);
        return false;
    }

    _faces.push_back(face);
    return true;
}

bool Model::Model::parseFaceVertex(const std::string& token, FaceVertex& result, unsigned int lineNumber)
{
    std::string parts[3];
    unsigned int partCount = 0;
    std::size_t start = 0;

    for (std::size_t ind = 0; ind <= token.size(); ind++)
    {
        if (ind == token.size() || token[ind] == '/')
        {
            if (partCount >= 3)
            {
                setError("Invalid face vertex: too many indexes.", lineNumber);
                return false;
            }
            parts[partCount++] = token.substr(start, ind - start);
            start = ind + 1;
        }
    }

    if (partCount == 0 || parts[0].empty())
    {
        setError("Face vertex is missing indexes.", lineNumber);
        return false;
    }

    
}


bool isValidVector(Vec3 vec)
{
    return (vec.isValidVector());
}

bool isValidVector(Vec2 vec)
{
    return (vec.isValidVector());
}
