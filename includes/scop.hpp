#ifndef SCOP_HPP
# define SCOP_HPP

// using openGL version of 3.3

# include <iostream>
# include <fstream>
# include <algorithm>
# include <cctype>
# include <filesystem>
# include <glad/gl.h>
# include <GLFW/glfw3.h>
# include <cmath>
# include <vector>
# include <string>

# include <math/Mat4.hpp>
# include <math/Vec3.hpp>
# include <Texture.hpp>
# include <Shader.hpp>
# include <Model.hpp>

namespace fs = std::filesystem;

const int HEIGHT = 800;
const int WIDTH = 1000;

class Model;
class Texture;
class Shader;
class Mat4;
class Vec3;

typedef struct  s_all
{
    Model*      objModel;
    GLFWwindow* window;
    Texture*    texture;
    Shader*     prog;
}               t_all;

int initWindow(GLFWwindow*& window);
bool validateOBJFile(const char* filename);

#endif