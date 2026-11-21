#ifndef SCOP_MATERIAL_HPP
# define SCOP_MATERIAL_HPP

# include <Vec3.hpp>
# include <string>
# include <map>
# include <sstream>

class Material
{
    private:
        std::string name;
        Vec3 ambient;
        Vec3 diffuse;
        Vec3 specular;
        float shininess;
        float opacity;

        static bool checkVector(std::stringstream& ss, Vec3& result);

    public:
        Material();
        ~Material();

        bool parseMTLProperty(const std::string& type, std::stringstream& stream,
                             std::string& errMsg);

        void setName(const std::string& matName);
        void setAmbient(const Vec3& srcVec);
        void setDiffuse(const Vec3& srcVec);
        void setSpecular(const Vec3& srcVec);
        void setShininess(const float& value);
        void setOpacity(const float& value);
};

#endif