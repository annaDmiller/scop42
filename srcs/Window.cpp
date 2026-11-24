#include "Window.hpp"

Window::Window():_handle(0), _width(0), _height(0)
{}

Window::~Window()
{
    if(_handle)
        glfwDestroyWindow(_handle);

    glfwTerminate();
}

bool Window::create(const int width, const int height, const std::string& title, std::string& error)
{
    if(!glfwInit())
    {
        error = "GLFW initialization failed.";
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _handle=glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if(!_handle)
    {
        error = "Could not create GLFW window.";
        glfwTerminate();
        return false;
    }

    _width = width;
    _height = height;
    
    glfwMakeContextCurrent(_handle);
    glfwSetWindowUserPointer(_handle, this);
    glfwSetFramebufferSizeCallback(_handle, framebufferSizeCallback);
    glfwSetCursorPosCallback(_handle, mouseCallback);
    glfwSetInputMode(_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        error = "GLAD could not load OpenGL functions.";
        return false;
    }

    
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    return true;
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* self=static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(self)
    {
        self->_width = width;
        self->_height = height;
    }
    glViewport(0, 0, width, height);
}

void Window::mouseCallback(GLFWwindow* window, float xPos, float yPos)
{

}

bool Window::shouldClose() const
{
    return !_handle || glfwWindowShouldClose(_handle);
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::swapBuffers() const
{
    if(_handle)
        glfwSwapBuffers(_handle);
}

void Window::clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* Window::getHandle() const
{
    return _handle;
}

int Window::getWidth() const
{ 
    return _width;
}

int Window::getHeight() const
{
    return _height;
}
