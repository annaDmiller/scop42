#ifndef SCOP_MATERIAL_HPP
# define SCOP_MATERIAL_HPP

# include <Vec3.hpp>
# include <string>
# include <map>
# include <sstream>

class Material
{
    private:
        std::string _name;
        Vec3 _ambient;
        Vec3 _diffuse;
        Vec3 _specular;
        float _shininess;
        float _opacity;

        static bool checkVector(std::stringstream& ss, Vec3& result);

    public:
        Material();
        ~Material();

        bool parseMTLProperty(const std::string& type, std::stringstream& stream,
                             std::string& errMsg);

        void setName(const std::string& name);

        const std::string& getName() const;
        const Vec3& getAmbient() const;
        const Vec3& getDiffuse() const;
        const Vec3& getSpecular() const;
        const float& getShininess() const;
};

#endif