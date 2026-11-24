# include <Shader.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath, bool& isSucc)
{
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    //ensure ifstream objects can throw exceptions
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    // retrieve the vertex and fragment code from the filePath
    try
    {
        vShaderFile.open(vertexPath);

        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vShaderFile.close();

        vertexCode = vShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cerr << "ERROR: Shader.cpp: vertex shader file can't be read:" << vertexPath << std::endl;
    }

    try
    {
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fShaderFile.close();

        fragmentCode = fShaderStream.str();
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: Shader.cpp: fragment shader file can't be read:" << fragmentPath << std::endl;
    }
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    //compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader.cpp: Vertex shader compilation failed.\n" << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader.cpp: Fragment shader compilation failed.\n" << std::endl;
    }

    // compile the shader programm
    this->_id = glCreateProgram();
    glAttachShader(this->_id, vertex);
    glAttachShader(this->_id, fragment);
    glLinkProgram(this->_id);

    glGetProgramiv(this->_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(this->_id, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader.cpp: Programm linking failed.\n" << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    isSucc = true;
}

Shader::~Shader()
{
    glDeleteProgram(_id);
}

void Shader::use()
{
    glUseProgram(this->_id);
}

const GLuint& Shader::getID() const
{
    return _id;
}
