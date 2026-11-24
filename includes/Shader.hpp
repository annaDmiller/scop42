#ifndef SCOP_SHADER_HPP
# define SCOP_SHADER_HPP

# include <glad/gl.h>
# include <string>
# include <fstream>
# include <sstream>
# include <iostream>

class Shader
{
    private:
        GLuint _id;

    public:
        Shader(const char* vertexPath, const char* fragmentPath, bool& isSucc);
        ~Shader();

        void use();
        const GLuint& getID() const;
};

#endif