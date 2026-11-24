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
# include <Window.hpp>

namespace fs = std::filesystem;

const int HEIGHT = 800;
const int WIDTH = 1000;

const char* VERTEX_SHADER = "./shaders/vertexShader.c";
const char* FRAGMENT_SHADER =  "./shaders/fragmentShader.c";

class Model;
class Texture;
class Shader;
class Mat4;
class Vec3;

typedef struct  s_all
{
    Model*      objModel;
    Window*     window;
    Texture*    texture;
    Shader*     prog;
}               t_all;

bool validateOBJFile(const char* filename);

#endif