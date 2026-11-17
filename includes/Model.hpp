#ifndef SCOP_MODEL_HPP
# define SCOP_MODEL_HPP

# include <vector>
# include <map>
# include <Vec3.hpp>
# include <Vec2.hpp>
# include <Material.hpp>
# include <fstream>
# include <sstream>
# include <cstdlib>
# include <cerrno>
# include <climits>
# include <cmath>
# include <algorithm>
# include <limits>
# include <string>
# include <iostream>
# include <cstddef>

struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
    Vec3 Color;
};

struct SubMesh
{
    unsigned int firstVertex;
    unsigned int vertexCount;
    unsigned int materialIndex;
};

struct FaceVertex
{
    int positionIndex;
    int texCoordIndex;
    int normalIndex;
};

struct Face
{
    std::vector<FaceVertex> vertices;
    std::string matName;
};

class Model
{
    private:
        std::vector<Vec3> _positions;
        std::vector<Vec2> _texCoords;
        std::vector<Vec3> _normals;
        std::vector<Face> _faces;
        std::vector<std::string> _matLibs;

        std::vector<Material> _materials;
        std::map<std::string, unsigned int> _materialLookup;

        std::vector<Vertex> _vertices;
        std::vector<SubMesh> _subMeshes;

        std::string _pathToFile;
        std::string _objectName;
        std::string _error;
        std::string _currMat;

        unsigned int _vao;
        unsigned int _vbo;
        bool _uploaded;
        bool _loaded;

        void setError(const std::string& msg, unsigned int lineNumber = 0);

        bool parseOBJ(const std::string& filename);
        bool parseVertex(std::stringstream& ss, unsigned int lineNumber);
        bool parseTexCoord(std::stringstream& ss, unsigned int lineNumber);
        bool parseNormal(std::stringstream& ss, unsigned int lineNumber);
        bool parseObjectName(std::stringstream& ss, unsigned int lineNumber);
        bool parseMtlLib(std::stringstream& ss, unsigned int lineNumber);
        bool parseUseMtl(std::stringstream& ss, unsigned int lineNumber);
        bool parseFace(std::stringstream& ss, unsigned int lineNumber);
        bool parseFaceVertex(const std::string& token, FaceVertex& result,
                            unsigned int lineNumber);


        unsigned int addMaterial(const Material& material);


    public:
        Model();
        explicit Model(const std::string& pathToFile);
        ~Model();

        bool load(const std::string& filename);

        //static Model* loadOBJ(const std::string& pathToFile);
        bool uploadToGPU();
        void releaseGPU();

        void draw() const;
        void drawSubMesh(unsigned int SubMeshIndex) const;

        bool isLoaded() const;
        bool isUploaded() const;
        
        const std::vector<Vertex> &getVertices() const;
        const std::vector<Material> &getMaterials() const;
        const std::vector<SubMesh> &getSubMeshes() const;
        const std::string &getError() const;

        void clearCPUData();
};

bool isValidVector(Vec3 vec);
bool isValidVector(Vec2 vec);
bool isFinite(float value);

#endif