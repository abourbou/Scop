
#include <scop.hpp>
#include <matrix.hpp>
#include <obj.hpp>

int main()
{
    try
    {
        Obj("test.obj");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}