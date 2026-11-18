#include <Material.hpp>

bool Material::parseMTLProperty(const std::string& type, const std::stringstream& stream,
                                std::string& errMsg)
{
    std::map<std::string, int> types = {
        {"Ka", 0}, // Ka - ambien coefficients' vector
        {"Kd", 1}, // Kd - diffuse coefficients' vector
        {"Ks", 2}, // Ks - specular coefficients' vector
        {"Ns", 3}, // Ns - shininess value
        {"d", 4}, // d - opacity value
        {"Tr", 5}, // Tr - transparency value
        {"map_Kd", 6} // map_Kd - 
    };

    std::map<std::string, int>::iterator it = types.find(type);
    if (it != types.end())
    {
        switch (it->second)
        {
        case 0:
            
            break;
        
        default:
            break;
        }
    }
}
