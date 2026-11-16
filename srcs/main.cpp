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
    all.objModel = Model::loadOBJ(argv[1]);
    if (!all.objModel)
        return 1;

    all.objModel->setupModel();
    all.window = NULL;
    if (initWindow(all.window) == -1)
        return 1;
    
    // run the window and inner processes

    // clear the all data

    return 0;
}