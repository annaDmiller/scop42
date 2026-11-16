#include <Model.hpp>

Model::Model()
{   }

Model::~Model()
{   }

typedef bool (*ParserFunction)(std::stringstream&, Model*, std::string&);

Model* Model::loadOBJ(const std::string& pathToFile)
{
    Model* ret = new Model();
    if (!ret)
    {
        std::cerr << "ERROR: loadOBJ: Impossible to allocate memory for the model." << std::endl;
        return NULL;        
    }

    std::ifstream file(pathToFile.c_str());

    if (!file.is_open())
    {
        std::cerr << "ERROR: loadOBJ: Impossible to open the file for reading." << std::endl;
        delete ret;
        return NULL;
    }

    std::string line;
    size_t lineNumber = 0;
    std::map<std::string, ParserFunction> types = {
        {"v", &Model::parseVertex}, // v = Vertex coordinates as floats
        {"vt", &Model::parseTexCoord}, // vt = Texture coordinates as floats
        {"vn", &Model::parseNormal}, // vn = Normals as floats
        {"f", &Model::parseFace}, // f = Faces : collection of indices which creates a face of the model
        {"o", &Model::parseObjectName} // o = Object name as string
    };
    
    while (std::getline(file, line))
    {
        ++lineNumber;

        std::stringstream ss(line);
        std::string type, err_str;

        if (!(ss >> type) || type[0] == '#')
            continue ;

        
        std::map<std::string, ParserFunction>::iterator it = types.find(type);
        if (it != types.end())
        {
            if (!(it->second(ss, ret, err_str)))
            {
                std::cerr << "ERROR: loadOBJ: OBJ parsing error at line " << lineNumber << ":" << std::endl;
                std::cerr << err_str << std::endl;
                delete ret;
                return NULL;
            }
        }
        // mtllib = material library : path to another file which must contain 
        //          information about the material(s) used for model
        else if (type == "mtllib")
        {

        }

        // usemtl = Material assignment to the following faces
        else if (type == "usemtl")
        {

        }

        // s
        // g

    }
}

bool Model::parseVertex(std::stringstream& ss, Model* model, std::string& err_str)
{
    float x, y, z;
    if (!(ss >> x >> y >> z))
    {
        err_str = "Vertex has less than 3 coordinates.";
        return false;
    }
            
    if (!isFinite(x) || !isFinite(y) || !isFinite (z))
    {
        err_str = "Vertex coordinates are not floats.";
        return false;
    }

    Vec3 vertex = Vec3(x, y, z);
    model->_positions.push_back(vertex);
    return true;
}

bool Model::parseTexCoord(std::stringstream& ss, Model* model, std::string& err_str)
{
    float x, y;
    if (!(ss >> x >> y))
    {
        err_str = "Texture vertex has less than 2 coordinates.";
        return false;
    }

    if (!isFinite(x) || !isFinite(y))
    {
        err_str = "Texture coordinates are not floats";
        return false;
    }

    Vec2 texCoord = Vec2(x, y);
    model->_texCoords.push_back(texCoord);
    return true;
}

bool Model::parseNormal(std::stringstream& ss, Model* model, std::string& err_str)
{
    float x, y, z;
    if (!(ss >> x >> y >> z))
    {
        err_str = "Vertex normal has less than 3 coordinates.";
        return false;
    }
            
    if (!isFinite(x) || !isFinite(y) || !isFinite (z))
    {
        err_str = "Vertex normal coordinates are not floats.";
        return false;
    }

    Vec3 normal = Vec3(x, y, z);
    model->_positions.push_back(normal);
    return true;
}

bool Model::parseFace(std::stringstream& ss, Model* model, std::string& err_str)
{
    
}

bool Model::parseObjectName(std::stringstream& ss, Model* model, std::string& err_str)
{
    std::string name;
    if (!(ss >> name))
        model->_objectName = "SCOP";
    else
        model->_objectName = name;
    
    return true;
}

bool isFinite(float value)
{
    return value == value &&
            value <= std::numeric_limits<float>::max() &&
            value >= -std::numeric_limits<float>::max();
}
