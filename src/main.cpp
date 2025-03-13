#include "png/png.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

int
main( [[maybe_unused]] int argc, [[maybe_unused]] char * argv[] ) {
    std::cout << "Hello" << std::endl;

    const auto filename{ "swirl.png" };

    const std::filesystem::path data_path{
        "/Users/ben/Documents/gitrepos.nosync/JPEG_Project/data"
    };

    const std::filesystem::directory_entry data_dir{ data_path };
    if ( !( data_dir.exists() ) ) {
        std::cout << "Directory: " << data_path << " does not exist."
                  << std::endl;
        return 1;
    }

    const auto                             png_path{ data_path / filename };
    const std::filesystem::directory_entry png_entry{ png_path };
    if ( !png_entry.exists() ) {
        std::cout << "File: " << png_entry.path() << " does not exist."
                  << std::endl;
        return 1;
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

    constexpr std::bitset<64> expected_header{ 0x89'504E47'0D0A'1A'0A };
    const auto result = PNG::verify_png_header( expected_header );

    PNG::PNG png{ std::string_view{ file_contents.str() } };
}
