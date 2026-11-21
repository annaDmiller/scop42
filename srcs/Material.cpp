#include <Material.hpp>

bool Material::parseMTLProperty(const std::string& type, std::stringstream& stream,
                                std::string& errMsg)
{
    std::map<std::string, int> types = {
        {"Ka", 0}, // Ka - ambien coefficients' vector
        {"Kd", 1}, // Kd - _diffuse coefficients' vector
        {"Ks", 2}, // Ks - _specular coefficients' vector
        {"Ns", 3}, // Ns - _shininess value
        {"d", 4}, // d - _opacity value
        {"Tr", 5} // Tr - transparency value
    };

    std::map<std::string, int>::iterator it = types.find(type);
    if (it != types.end())
    {
        switch (it->second)
        {
        case 0:
            if (!checkVector(stream, this->_ambient))
            {
                errMsg = "Invalid data for _ambient vector.";
                return false;
            }
            break;

        case 1:
            if (!checkVector(stream, this->_diffuse))
            {
                errMsg = "Invalid data for _diffuse vector.";
                return false;
            }
            break;
            
        case 2:
            if (!checkVector(stream, this->_specular))
            {
                errMsg = "Invalid data for _specular vector.";
                return false;
            }
            break;

        case 3:
            if (!(stream >> this->_shininess) ||
                !isFinite(this->_shininess) || this->_shininess < 0.0f)
            {
                errMsg = "Invalid _shininess value.";
                return false;
            }
            break;

        case 4:
            if (!(stream >> this->_opacity) || 
                !isFinite(this->_opacity) || this->_opacity < 0.0f ||
                this->_opacity > 1.0f)
            {
                errMsg = "Invalid _opacity value.";
                return false;
            }
            break;

        case 5:
            float trans;
            if (!(stream >> trans) || !isFinite(trans) || 
                trans < 0.0f || trans > 1.0f)
            {
                errMsg = "Invalid transparency value.";
                return false;
            }
            this->_opacity = 1.0f - trans;
            break;
            
        default:
            break;
        }
    }
}

bool Material::checkVector(std::stringstream& ss, Vec3& result)
{
    float x, y, z;
    if (!(ss >> x >> y >> z))
        return false;
    
    Vec3 check(x, y, z);
    if (!check.isValidVector())
        return false;
    
    result = check;
    return true;
}

const std::string& Material::getName() const
{
    return _name;
}