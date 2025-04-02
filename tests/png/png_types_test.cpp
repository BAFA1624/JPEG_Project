#include "common/test_interfaces.hpp"
#include "png/png_types.hpp"

namespace PNG
{

void
tmp( const TestError auto error ) {
    std::cout << error.what() << std::endl;
}

void
test_func( const Test<double, std::runtime_error, double, double> auto tmp ) {}

// Test: png_chunk_t

// Test: PNGChunk
/* PNGChunk::PNGChunk():*/
/* PNGChunk::PNGChunk( const std::uint32_t _len, const png_chunk_t _type,
                       std::unique_ptr<char *> _source_ptr,
                       const std::bitset<32> & _crc ):*/
/* PNGChunk::PNGChunk( const std::uint32_t _len, const png_chunk_t _type,
                       const char * const      _source_ptr,
                       const std::bitset<32> & _crc ):*/

} // namespace PNG

int
png_types_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    return 0;
}
