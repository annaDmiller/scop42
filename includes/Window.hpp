#ifndef SCOP_WINDOW_HPP
# define SCOP_WINDOW_HPP

# include <glad/gl.h>
# include <GLFW/glfw3.h>
# include <string>

const int HEIGHT = 800;
const int WIDTH = 1000;

class Window
{
    private:
        GLFWwindow* _handle;
        int _width;
        int _height;

        static void framebufferSizeCallback(GLFWwindow*, int, int);
        static void mouseCallback(GLFWwindow*, float, float);

    public:
        Window();
        ~Window();
        
        bool create(const int width, const int height, const std::string& title, std::string& error);

        bool shouldClose() const;
        void pollEvents() const;
        void swapBuffers() const;

        void clear(float r,float g,float b,float a);

        GLFWwindow* getHandle() const;
        int getWidth() const;
        int getHeight() const;
};

#endif