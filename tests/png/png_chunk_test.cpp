#include "png/png_chunk_test.hpp"

#include <print>

namespace PNG
{

bool
test_ihdr_chunk() {
    return false;
}

bool
test_plte_chunk() {
    return false;
}

bool
test_idat_chunk() {
    return false;
}

bool
test_iend_chunk() {
    return false;
}

} // namespace PNG

int
png_chunk_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    std::size_t test_passes{ 0 };

    for ( const auto & func : PNG::test_functions ) {
        std::println( "Running test..." );
        test_passes += ( func() ? 1 : 0 );
    }

    std::println(
        "Failed tests: {}",
        static_cast<int>( PNG::test_functions.size() - test_passes ) );
    return static_cast<int>( PNG::test_functions.size() - test_passes );
}
