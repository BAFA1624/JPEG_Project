#include "png/png_chunk_payload_test.hpp"

namespace PNG
{

bool
test_png_chunk_payload_base() {
    return true;
}

} // namespace PNG

int
png_chunk_payload_test( [[maybe_unused]] int     argc,
                        [[maybe_unused]] char ** argv ) {
    std::size_t test_passes{ 0 };

    for ( const auto & func : PNG::test_functions ) {
        test_passes += ( func() ? 1 : 0 );
    }

    return static_cast<int>( PNG::test_functions.size() - test_passes );
}
