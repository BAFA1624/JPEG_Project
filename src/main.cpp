#include "../includes/PNG.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
	std::cout << "Hello" << std::endl;
	assert(argc == 2);

	PNG png;
	auto test = ParsePNG(std::filesystem::directory_entry{argv[1]}, png);
	if (test)
		std::cout << "Success!" << std::endl;
	else
		std::cout << "Failure!" << std::endl;
}
