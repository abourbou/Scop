
#include "Scop.hpp"
#include "Matrix.hpp"
#include "Obj.hpp"
#include "Rendering.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./Scop [OBJ]" << std::endl;
		return 0;
	}
    try
    {
        Obj obj(argv[1]);
		rendering(obj);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}