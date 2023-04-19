
#include "Scop.hpp"
#include "Matrix.hpp"
#include "Obj.hpp"
#include "Rendering.hpp"

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