#include <scop.hpp>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "ERROR: the launch usage - ./scop <path_to_obj_file>. Please, try again." << std::endl;
        return 1;
    }

    if (!validateOBJFile(argv[1]))
        return 1;
    
    t_all all;
    all.prog = NULL;
    all.window = NULL;
    all.texture = NULL;
    all.objModel = NULL;

    all.objModel = new Model();
    if (!all.objModel)
    {
        std::cerr << "ERROR: impossible to create Model instance. Please, try again later." << std::endl;
        return 1;
    }

    if (!all.objModel->load(std::string(argv[1])))
    {
        std::cerr << all.objModel->getError() << std::endl;
        delete all.objModel;
        return 1;
    }

    all.window = new Window();
    if (!all.window)
    {
        std::cerr << "ERROR: impossible to initialize the window." << std::endl;
        delete all.objModel;
        return 1;
    }

    std::string err;
    if (!all.window->create(WIDTH, HEIGHT, all.objModel->getObjName(), err))
    {
        std::cerr << "ERROR: WINDOW: " << err << std::endl;
        delete all.objModel;
        return 1;
    }

    bool suc = false;
    all.prog = new Shader(VERTEX_SHADER, FRAGMENT_SHADER, suc);
    if (!all.prog || !suc)
    {
        std::cerr << "ERROR: impossible to create Shader program." << std::endl;
        delete all.objModel;
        delete all.window;
        return 1;
    }
    all.prog->use();

    if (!all.objModel->uploadToGPU())
    {
        std::cerr << all.objModel->getError() << std::endl;
        delete all.objModel;
        delete all.prog;
        delete all.window;
        return 1;
    }

    // upload texture

    // init rendering settings
    
    // run the window and inner processes

    // clear the all data

    return 0;
}