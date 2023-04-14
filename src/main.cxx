
#include "scop.hpp"
#include "matrix.hpp"
#include "obj.hpp"
#include "rendering.hpp"

int main()
{
    try
    {
        Obj obj("test.obj");
		rendering(obj);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}