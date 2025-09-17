#include "png/png_chunk_payload_test.hpp"

#include <print>

namespace PNG
{

bool
test_png_chunk_payload_base() {
    return false;
}

bool
test_ihdr_payload() {
    return false;
}

bool
test_plte_payload() {
    return false;
}

bool
test_idat_payload() {
    return false;
}

bool
test_iend_payload() {
    return false;
}

} // namespace PNG

int
png_chunk_payload_test( [[maybe_unused]] int     argc,
                        [[maybe_unused]] char ** argv ) {
    return TEST_INTERFACE::run_tests( PNG::test_functions );
}
