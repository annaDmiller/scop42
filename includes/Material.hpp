#ifndef SCOP_MATERIAL_HPP
# define SCOP_MATERIAL_HPP

# include <Vec3.hpp>
# include <string>

class Material
{
    private:
        std::string name;
        Vec3 ambient;
        Vec3 diffuse;
        Vec3 specular;
        float shininess;
        float opacity;
        std::string diffuseMap;

    public:
        Material();
        ~Material();

};

#endif