
#include "obj.hpp"

Obj::Obj(std::string fileName)
{
   std::fstream file;

   file.open(fileName, std::ios::in);
   if (file.is_open())
   {
      std::string line;
      while(getline(file, line))
      {
        std::cout << line << std::endl;
      }
      file.close();
   }
   else
    throw FileException();
}