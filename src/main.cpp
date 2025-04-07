#include "common/test_interfaces.hpp"
#include "png/png.hpp"

#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

bool
tmp( const Test<double, std::runtime_error, double> auto & test ) {
    test.run();
}

int
main( [[maybe_unused]] int argc, [[maybe_unused]] char * argv[] ) {
    if constexpr ( std::endian::native == std::endian::little ) {
        std::cout << "Little endian.\n";
    }
    else if constexpr ( std::endian::native == std::endian::big ) {
        std::cout << "Big endian.\n";
    }
    else {
        std::cout << "Mixed endian.\n";
    }

    const auto * const filename{ "swirl.png" };

    const auto data_path{ std::filesystem::current_path() / "../data" };

    const std::filesystem::directory_entry data_dir{ data_path };
    if ( !( data_dir.exists() ) ) {
        std::cout << "Directory: " << data_path << " does not exist."
                  << std::endl;
        return 1;
    }
    else {
        std::cout << "Found: " << data_path << std::endl;
    }

    const auto                             png_path{ data_path / filename };
    const std::filesystem::directory_entry png_entry{ png_path };
    if ( !png_entry.exists() ) {
        std::cout << "File: " << png_entry.path() << " does not exist."
                  << std::endl;
        return 1;
    }
    else {
        std::cout << "Found file: " << png_path << std::endl;
    }

    std::ifstream png_file{ png_path, std::ios::binary | std::ios::in };
    if ( !png_file.is_open() ) {
        std::cout << "Unable to open " << png_entry << " file." << std::endl;
    }

    png_file.seekg( std::ios_base::end );
    const auto file_size{ static_cast<std::size_t>( png_file.tellg() ) };
    png_file.seekg( std::ios_base::beg );

    std::stringstream file_contents;
    file_contents << png_file.rdbuf();

    PNG::PNG png{ std::string_view{ file_contents.str() } };

    return 0;
}
