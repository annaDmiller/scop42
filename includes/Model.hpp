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
# include <glad/gl.h>
# include <type_traits>

struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;

    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;

    float shininess;
};

static_assert(std::is_standard_layout_v<Vec3>,
    "Vec3 class must have a standard layout to be compatible with Vertex attributes.");

static_assert(std::is_standard_layout_v<Vec2>,
    "Vec2 class must have a standard layout to be compatible with Vertex attributes.");

static_assert(sizeof(float) == 4,
    "GL_FLOAT requires 32-bit float.");

static_assert(sizeof(Vec3) == 3 * sizeof(float),
    "Unexpected size of Vec3 class.");

static_assert(sizeof(Vec2) == 2 * sizeof(float),
    "Unexpected size of Vec2 class.");
    
static_assert(std::is_standard_layout_v<Vertex>,
    "Vertex struct must have a standard layout to be compatible with Vertex attributes.");

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
    unsigned int matIndex;
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

        std::string _pathToFile;
        std::string _objDirectory;
        std::string _objectName;
        std::string _error;
        std::string _currMat;

        unsigned int _vao;
        unsigned int _vbo;
        bool _uploaded;
        bool _loaded;

        Vec3 _boundsMin;
        Vec3 _boundsMax;
        Vec3 _center;

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        bool buildRenderData();
        void clearCPUData();
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
        bool parseMTL(const std::string& libPath);

        static std::string directoryOf(const std::string& filename);
        static std::string joinPath(const std::string& directory, const std::string& filename);
        static bool stringToInteger(const std::string& text, int& value);
        static bool resolveOBJIndex(int objIndex, std::size_t count, int& result);

        bool validate();
        bool validateFace(const Face& face, unsigned int faceNumber);

        bool calculateBounds();
        void centerAndNormalize();

        bool appendFaceTriangles(const Face& face, unsigned int faceNumber);
        bool appendTriangle(const FaceVertex& a, const FaceVertex& b, const FaceVertex& c,
                            unsigned int indexMaterial, unsigned int faceNumber);

        static Vec3 faceNormal(const Vec3& a, const Vec3& b, const Vec3& c);

        unsigned int findMaterial(const std::string &name) const;
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

        bool isLoaded() const;
        bool isUploaded() const;
        
        const std::vector<Vertex> &getVertices() const;
        const std::vector<Material> &getMaterials() const;
        const std::string &getError() const;
        const std::string &getObjName() const;

};

bool isValidVector(const Vec3& vec);
bool isValidVector(const Vec2& vec);

#endif