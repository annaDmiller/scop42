#include <Model.hpp>

namespace {
    void stripComment(std::string& line)
    {
        const std::size_t pos = line.find('#');
        if (pos != std::string::npos)
            line.erase(pos);
    }
}

Model::Model()
    : _vao(0), _vbo(0), _loaded(false), _uploaded(false)
{
    clearCPUData();
}

Model::Model(const std::string& pathToFile)
    : _boundsMin(), _boundsMax(),
    _vao(0), _vbo(0), _loaded(false), _uploaded(false),
    _pathToFile(pathToFile)
{
    load(pathToFile);
}

Model::~Model()
{
    releaseGPU();
}

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

    Material defaultMaterial;
    addMaterial(defaultMaterial);

    _loaded = false;
    _currMat.clear();
    _objectName = "SCOP";
    _boundsMin = Vec3();
    _boundsMax = Vec3();
    _center = Vec3();
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
    if (_uploaded)
    {
        setError("Release GPU resources before reloading the model.");
        return false;
    }
    clearCPUData();

    _pathToFile = filename;
    _objDirectory = directoryOf(filename);

    if (!parseOBJ(filename))
        return false;

    for (std::size_t ind = 0; ind < _matLibs.size(); ind++)
    {
        const std::string mtlPath = joinPath(_objDirectory, _matLibs[ind]);
        if (!parseMTL(mtlPath))
            return false;
    }

    if (!validate())
        return false;

    if (!calculateBounds())
        return false;

    centerAndNormalize();
    
    if (!buildRenderData())
        return false;

    if (_vertices.empty())
    {
        setError("The OBJ file does not contain renderable triangles.");
        return false;
    }

    _loaded = true;
    return true;
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
        stripComment(line);
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
    const std::size_t prevCount = _matLibs.size();
    while (ss >> lib)
        _matLibs.push_back(lib);

    if (_matLibs.size() == prevCount)
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
    face.matIndex = 0;

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

bool Model::parseFaceVertex(const std::string& token, FaceVertex& result, unsigned int lineNumber)
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

    int rawIndex;
    if (!stringToInteger(parts[0], rawIndex) ||
        !resolveOBJIndex(rawIndex, _positions.size(), result.positionIndex))
    {
        setError("Invalid or out-of-range position index in face.", lineNumber);
        return false;
    }

    result.texCoordIndex = -1;
    result.normalIndex = -1;

    if (partCount >= 2 && !parts[1].empty())
    {
        if (!stringToInteger(parts[1], rawIndex) ||
            !resolveOBJIndex(rawIndex,_texCoords.size(), result.texCoordIndex))
        {
            setError("Invalid or out-of-range texture index in face.", lineNumber);
            return false;
        }
    }

    if (partCount == 3 && !parts[2].empty())
    {
        if (!stringToInteger(parts[2], rawIndex) ||
            !resolveOBJIndex(rawIndex, _normals.size(), result.normalIndex))
        {
            setError("Invalid or out-of-range normal index in face.", lineNumber);
            return false;
        }
    }

    if ((partCount == 2 && parts[1].empty()) ||
        (partCount == 3 && parts[2].empty() && parts[1].empty()))
    {
        setError("Malformed face vertex index.", lineNumber);
        return false;
    }

    return true;
}

bool Model::parseMTL(const std::string& libPath)
{
    std::ifstream file(libPath.c_str());
    if (!file.is_open())
    {
        setError("Could not open material library: " + libPath);
        return false;
    }

    std::string line;
    unsigned int lineNumber = 0;
    int currMat = -1;

    while (std::getline(file, line))
    {
        ++lineNumber;
        stripComment(line);
        if (line.empty() || line[0] == '#')
            continue ;

        std::stringstream stream;
        std::string directive;
        stream >> directive;
        if (directive == "newmtl")
        {
            std::string matName;
            if (!(stream >> matName))
            {
                setError("newmtl directive is missing a material name in " + libPath + ".", lineNumber);
                return false;
            }

            Material newMat;
            newMat.setName(matName);
            currMat = static_cast<int>(addMaterial(newMat));
        }
        else if (currMat >= 0)
        {
            std::string errMsg = "";
            if (!_materials[currMat].parseMTLProperty(directive, stream, errMsg))
            {
                setError(errMsg, lineNumber);
                return false;
            }
        }
    }

    if (file.bad())
    {
        setError("I/O error while reading material library: " + libPath);
        return false;
    }

    return true;
}

std::string Model::directoryOf(const std::string& filename)
{
    const std::string::size_type pos = filename.find_last_of("/\\");
    if (pos == std::string::npos)
        return ".";

    if (pos == 0)
        return filename.substr(0, 1);

    return filename.substr(0, pos);
}

std::string Model::joinPath(const std::string& directory, const std::string& filename)
{
    if (filename.empty())
        return directory;

    if (filename[0] == '/' || filename[0] == '\\' ||
        (filename.size() > 1 && filename[1] == ':'))
        return filename;

    if (directory.empty() || directory == ".")
        return filename;

    const char lastC = directory[directory.size() - 1];
    if (lastC == '/' || lastC == '\\')
        return directory + filename;
    return directory + "/" + filename;
}

bool Model::stringToInteger(const std::string& text, int& value)
{
    if (text.empty())
        return false;

    char* end = NULL;
    errno = 0;
    long parsed = std::strtol(text.c_str(), &end, 10);

    if (errno != 0 || end == text.c_str() || *end != '\0' ||
        parsed < INT_MIN || parsed > INT_MAX || parsed == 0)
        return false;

    value = static_cast<int>(parsed);
    return true;
}

bool Model::resolveOBJIndex(int objIndex, std::size_t count, int& result)
{
    if (objIndex == 0 || count > static_cast<std::size_t>(INT_MAX))
        return false;

    long resolved;
    if (objIndex > 0)
        resolved = static_cast<long>(objIndex) - 1;
    else
        resolved = static_cast<long>(count) + objIndex;

    if (resolved < 0 || resolved >= static_cast<long>(count))
        return false;

    result = static_cast<int>(resolved);
    return true;
}

bool Model::validate()
{
    if (_positions.empty())
    {
        setError("OBJ file contains no valid vertex positions.");
        return false;
    }

    if (_faces.empty())
    {
        setError("OBJ file contaians no faces information.");
        return false;
    }

    for (size_t ind = 0; ind < _faces.size(); ind++)
    {
        if (!validateFace(_faces[ind], static_cast<unsigned int>(ind + 1)))
            return false;
    }

    return true;
}

bool Model::validateFace(const Face& face, unsigned int faceNumber) 
{
    if (face.vertices.size() < 3)
    {
        setError("Face can't have less than 3 vertices in 3D model.");
        return false;
    }

    for (std::size_t ind = 0; ind < face.vertices.size(); ind++)
    {
        const FaceVertex& v = face.vertices[ind];
        std::string faceNumberStr = std::to_string(faceNumber);
        if (v.positionIndex < 0 || 
            static_cast<std::size_t>(v.positionIndex) >= _positions.size())
        {
            setError(
                "Face " + faceNumberStr + " has an invalid position index.");
            return false;
        }

        if (v.texCoordIndex != -1 &&
            (v.texCoordIndex < 0 ||
            static_cast<std::size_t>(v.texCoordIndex) >= _texCoords.size()))
        {
            setError(
                "Face " + faceNumberStr + " has an invalid texture coordinate index.");
            return false;
        }

        if (v.normalIndex != -1 &&
            (v.normalIndex < 0 || 
            static_cast<std::size_t>(v.normalIndex) >= _normals.size()))
        {
            setError(
                "Face " + faceNumberStr + " has an invalid normal index.");
            return false;
        }
    }

    return true;
}

bool Model::calculateBounds()
{
    if (_positions.empty())
    {
        setError("Cannot calculate bounds: no positions.");
        return false;
    }

    _boundsMin = _positions[0];
    _boundsMax = _positions[0];

    for (std::size_t ind = 1; ind < _positions.size(); ind++)
    {
        const Vec3& p = _positions[ind];
        _boundsMin.compareAndSetMin(p);
        _boundsMax.compareAndSetMax(p);
    }

    _center = Vec3::calculateAvVec(_boundsMin, _boundsMax);
    return true;
}

void Model::centerAndNormalize()
{
    Vec3 diff = _boundsMax - _boundsMin;
    const float dx = diff.getX();
    const float dy = diff.getY();
    const float dz = diff.getZ();
    const float maxDimension = std::max(dx, std::max(dy, dz));

    if (maxDimension <= 1.0e-8f)
        return;

    const float scale = 2.0f / maxDimension;
    for (std::size_t ind = 0; ind < _positions.size(); ind++)
        _positions[ind] = (_positions[ind] - _center) * scale;

    _boundsMin =   Vec3(dx == 0.0f ? 0.0f : -dx * 0.5f * scale,
                        dy == 0.0f ? 0.0f : -dy * 0.5f * scale,
                        dz == 0.0f ? 0.0f : -dz * 0.5f * scale);
    _boundsMax = Vec3() -_boundsMin;
    _center = Vec3();
}

bool Model::buildRenderData()
{
    _vertices.clear();

    for (std::size_t ind = 0; ind < _faces.size(); ind++)
    {
        _faces[ind].matIndex = findMaterial(_faces[ind].matName);
        if (!appendFaceTriangles(_faces[ind], static_cast<unsigned int>(ind + 1)))
            return false;
    }

    return true;
}

bool Model::appendFaceTriangles(const Face& face, unsigned int faceNumber)
{
    const unsigned int materialIndex = face.matIndex;
    if (!face.matName.empty() && _materialLookup.find(face.matName) == _materialLookup.end())
    {
        //Using the default material
        //Create default material and add it to the material
        std::cerr << "The material for the face " + numberToString(faceNumber) + " not found. Using the default material: " << std::endl;
    }

    for (std::size_t ind = 1; ind + 1 < face.vertices.size(); ind++)
    {
        if (!appendTriangle(face.vertices[0], face.vertices[ind], face.vertices[ind + 1],
                materialIndex, faceNumber))
            return false;
    }

    return true;
}

bool Model::appendTriangle(const FaceVertex& a, const FaceVertex& b, const FaceVertex& c,
    unsigned int materialIndex, unsigned int faceNumber)
{
    const Vec3& pa = _positions[a.positionIndex];
    const Vec3& pb = _positions[b.positionIndex];
    const Vec3& pc = _positions[c.positionIndex];

    const Vec3 cross = faceNormal(pa, pb, pc);
    if (!cross.isValidVector() || cross.length() <= 1.0e-8f)
    {
        setError("Degenerate triangle in face " + std::to_string(faceNumber) + ".");
        return false;
    }

    const Vec3 normal = cross.normalized();
    const Material& material = _materials[materialIndex];
    const FaceVertex corners[3] = { a, b, c };
    const float nx = std::fabs(normal.getX());
    const float ny = std::fabs(normal.getY());
    const float nz = std::fabs(normal.getZ());
    for (int ind = 0; ind < 3; ind++)
    {
        const FaceVertex &src = corners[ind];
        const Vec3& p = _positions[src.positionIndex];
        Vertex vertex;
        vertex.position = p;
        vertex.normal = normal;
        if (src.texCoordIndex >= 0)
        {
            const Vec2& uv = _texCoords[src.texCoordIndex];
            vertex.texCoord = uv;
        }
        else
        {
            float u, v;
            if (nx >= ny && nx >= nz)
            {
                u = p.getZ();
                v = p.getY();
            }
            else if (ny >= nz)
            {
                u = p.getX();
                v = p.getZ();
            }
            else
            {
                u = p.getX();
                v = p.getY();
            }
            vertex.texCoord = Vec2((u + 1.0f) * 0.5f, (v + 1.0f) * 0.5f);
        }

        vertex.ambient = material.getAmbient();
        vertex.diffuse = material.getDiffuse();
        vertex.specular = material.getSpecular();
        vertex.shininess = material.getShininess();

        _vertices.push_back(vertex);
    }

    return true;
}

bool Model::uploadToGPU()
{
    if (!_loaded || _vertices.empty())
    {
        setError("Cannot upload the model: no CPU-side model data is loaded.");
        return false;
    }

    if (_uploaded)
        return true;

    if (_vertices.size() > static_cast<std::size_t>(INT_MAX) ||
        _vertices.size() > static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max()) / sizeof(Vertex))
    {
        setError("Model exceeds OpenGL buffer or draw count limits.");
        return false;
    }

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    if (_vao == 0 || _vbo == 0)
    {
        _error = "Failed to generate VAO or VBO.";
        releaseGPU();
        return false;
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_vertices.size() * sizeof(Vertex)),
                 _vertices.data(), GL_STATIC_DRAW);
    
                 const std::size_t offsets[] = {
        offsetof(Vertex, position), offsetof(Vertex, normal),
        offsetof(Vertex, texCoord), offsetof(Vertex, ambient),
        offsetof(Vertex, diffuse), offsetof(Vertex, specular),
        offsetof(Vertex, shininess)
    };
    const GLint sizes[] = { 3, 3, 2, 3, 3, 3, 1 };

    for (GLuint ind = 0; ind < 7; ind++)
    {
        glVertexAttribPointer(ind, sizes[ind], GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void*>(offsets[ind]));
        glEnableVertexAttribArray(ind);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (glGetError() != GL_NO_ERROR)
    {
        releaseGPU();
        setError("OpenGL error during upload (or pending earlier GL error).");
        return false;
    }

    _uploaded = true;
    _error.clear();
    return true;
}

void Model::releaseGPU()
{
    if (_vbo)
        glDeleteBuffers(1, &_vbo);
    
    if (_vao)
        glDeleteVertexArrays(1, &_vao);

    _vao = _vbo = 0;
    _uploaded = false;
}

void Model::draw() const
{
    if (!_uploaded)
        return ;

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(_vertices.size()));
    glBindVertexArray(0);
}

bool Model::isLoaded() const
{
    return _loaded;
}

bool Model::isUploaded() const
{
    return _uploaded;
}

const std::vector<Vertex>& Model::getVertices() const
{
    return _vertices;
}

const std::vector<Material>& Model::getMaterials() const
{
    return _materials;
}

const std::string& Model::getError() const
{
    return _error;
}

const std::string& Model::getObjName() const
{
    return _objectName;
}
unsigned int Model::findMaterial(const std::string& name) const
{
    if (name.empty())
        return 0;

    std::map<std::string, unsigned int>::const_iterator it =
        _materialLookup.find(name);

    if (it == _materialLookup.end())
        return 0;
    
    return it->second;
}

unsigned int Model::addMaterial(const Material& material)
{
    std::map<std::string, unsigned int>::iterator existing =
        _materialLookup.find(material.getName());

    if (existing != _materialLookup.end())
    {
        _materials[existing->second] = material;
        return existing->second;
    }

    const unsigned int index = static_cast<unsigned int>(_materials.size());
    _materials.push_back(material);
    _materialLookup[material.getName()] = index;
    return index;
}

Vec3 Model::faceNormal(const Vec3& a, const Vec3& b, const Vec3& c)
{
    return Vec3::cross((b - a), (c - a)).normalized();
}

bool isValidVector(const Vec3& vec)
{
    return (vec.isValidVector());
}

bool isValidVector(const Vec2& vec)
{
    return (vec.isValidVector());
}
