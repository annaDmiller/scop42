#ifndef SCOP_MODEL_HPP
# define SCOP_MODEL_HPP

# include <vector>
# include <Vec3.hpp>
# include <Vec2.hpp>
# include <sstream>
# include <string>
# include <fstream>
# include <iostream>
# include <map>

struct ObjIndex
{
    int vertex;
    int texCoord;
    int normal;
};

struct ObjFace
{
    std::vector<ObjIndex> vertices;
    std::string matName;
};

struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
    Vec3 Color;
};

struct Material
{
    std::string name;

    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;

    float shininess;
    float opacity;

    std::string diffuseMap;
};

struct ObjData
{
    std::vector<Vec3> positions;
    std::vector<Vec2> texCoords;
    std::vector<Vec3> normals;

    std::vector<ObjFace> faces;
};

class Model
{
    private:
        std::vector<Vec3> _positions;
        std::vector<Vec2> _texCoords;
        std::vector<Vec3> _normals;
        std::vector<std::string> _matLibs;

        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
        std::vector<Material> _materials;

        std::string _pathToFile;
        std::string _objectName;

        static bool parseVertex(std::stringstream& ss, Model* model, std::string& err_str);
        static bool parseTexCoord(std::stringstream& ss, Model* model, std::string& err_str);
        static bool parseNormal(std::stringstream& ss, Model* model, std::string& err_str);
        static bool parseFace(std::stringstream& ss, Model* model, std::string& err_str);
        static bool parseObjectName(std::stringstream& ss, Model* model, std::string& err_str);

    public:
        Model();
        ~Model();

        static Model* loadOBJ(const std::string& pathToFile);
        void setupModel();
};

bool isFinite(float value);

#endif