
#include <scop.hpp>
#include <matrix.hpp>
#include <obj.hpp>

void	test_stdout()
{
	Matrix<double, 1, 1> mat1(1);
	Matrix<double, 1, 2> mat2(2);
	Matrix<double, 2, 1> mat3(3);
	Matrix<double, 2, 2> mat4(4);

	std::cout << mat1 << std::endl;
	std::cout << mat2 << std::endl;
	std::cout << mat3 << std::endl;
	std::cout << mat4 << std::endl;
}

void	test_matrix()
{
	test_stdout();
}

int main()
{
	// test_matrix();
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