#include <scop.hpp>

static char to_lower(unsigned char c)
{
    return static_cast<char>(std::tolower(c));
}

// we want to validate that the file given to the programm is valid for the run
bool validateOBJFile(const char* filename)
{
    if (filename == nullptr || filename[0] == '\0')
    {
        std::cerr << "ERROR: No file is provided." << std::endl;
        return false;
    }

    fs::path path(filename);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), to_lower);
    if (extension != ".obj")
    {
        std::cerr << "ERROR: The file must have a .obj extension." << std::endl;
        return false;
    }

    std::error_code err;
    if (!fs::exists(path, err))
    {
        if (err)
            std::cerr << "ERROR: an error occurred while checking path: " << err.message() << std::endl;
        else
            std::cerr << "ERROR: File does not exist." << std::endl;

        return false;
    }

    if (fs::is_directory(path, err))
    {
        std::cerr << "ERROR: The provided path is a directory." << std::endl;
        return false;
    }

    if (err)
    {
        std::cerr << "ERROR: an error occurred while checking file type: " << err.message() << std::endl;
        return false;
    }

    if (!fs::is_regular_file(path, err))
    {
        if (err)
            std::cerr << "ERROR: an error occurred while checking file type: " << err.message() << std::endl;
        else
            std::cerr << "ERROR: The path is not a regular file." << std::endl;

        return false;
    }

    const auto fileSize = fs::file_size(path, err);
    if (err)
    {
        std::cerr << "ERROR: an error occurred while checking file size: " << err.message() << std::endl;
        return false;
    }

    if (fileSize == 0)
    {
        std::cerr << "ERROR: The file is empty." << std::endl;
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "ERROR: Cannot open the file." << std::endl;
        return false;
    }

    char c;
    bool containsContent = false;
    while (file.get(c))
    {
        if (!std::isspace(static_cast<unsigned char>(c)))
        {
            containsContent = true;
            break;
        }
    }
    if (!containsContent)
    {
        std::cerr << "ERROR: The file contains only whitespace." << std::endl;
        return false;
    }

    std::cout << "File validation successful: " << path << std::endl;
    return true;
}

