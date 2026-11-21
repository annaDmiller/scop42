#include <Material.hpp>

bool Material::parseMTLProperty(const std::string& type, std::stringstream& stream,
                                std::string& errMsg)
{
    std::map<std::string, int> types = {
        {"Ka", 0}, // Ka - ambien coefficients' vector
        {"Kd", 1}, // Kd - diffuse coefficients' vector
        {"Ks", 2}, // Ks - specular coefficients' vector
        {"Ns", 3}, // Ns - shininess value
        {"d", 4}, // d - opacity value
        {"Tr", 5} // Tr - transparency value
    };

    std::map<std::string, int>::iterator it = types.find(type);
    if (it != types.end())
    {
        switch (it->second)
        {
        case 0:
            if (!checkVector(stream, this->ambient))
            {
                errMsg = "Invalid data for ambient vector.";
                return false;
            }
            break;

        case 1:
            if (!checkVector(stream, this->diffuse))
            {
                errMsg = "Invalid data for diffuse vector.";
                return false;
            }
            break;
            
        case 2:
            if (!checkVector(stream, this->specular))
            {
                errMsg = "Invalid data for specular vector.";
                return false;
            }
            break;

        case 3:
            if (!(stream >> this->shininess) ||
                !isFinite(this->shininess) || this->shininess < 0.0f)
            {
                errMsg = "Invalid shininess value.";
                return false;
            }
            break;

        case 4:
            if (!(stream >> this->opacity) || 
                !isFinite(this->opacity) || this->opacity < 0.0f ||
                this->opacity > 1.0f)
            {
                errMsg = "Invalid opacity value.";
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
            this->opacity = 1.0f - trans;
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