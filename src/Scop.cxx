
#include "Scop.hpp"
#include "Matrix.hpp"
#include "Obj.hpp"
#include "Rendering.hpp"

int main(int argc, char **argv)
{
	if (argc < 2 || argc > 3)
	{
		std::cerr << "Usage: ./Scop OBJ [Text]" << std::endl;
		return 0;
	}
    try
    {
        Obj obj(argv[1]);
		if (argc == 3)
			rendering(obj, argv[2]);
		else
			rendering(obj);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}