#include "common/common_test.hpp"

#include <functional>
#include <iostream>

constexpr bool
test_convert_endian() {
    constexpr auto test_little_little{
        convert_endian<std::endian::little, std::endian::little>( 0x12345678u )
        == 0x12345678u
    };
    if ( !test_little_little ) {
        std::cerr << "convert_endian<std::endian::little, std::endian::little> "
                     "failed."
                  << std::endl;
    }

    constexpr auto test_little_big{
        convert_endian<std::endian::little, std::endian::big>( 0x12345678u )
        == 0x78563412u
    };
    if ( !test_little_big ) {
        std::cerr
            << "convert_endian<std::endian::little, std::endian::big> failed."
            << std::endl;
    }

    constexpr auto test_big_little{
        convert_endian<std::endian::big, std::endian::little>( 0x78563412u )
        == 0x12345678u
    };
    if ( !test_big_little ) {
        std::cerr
            << "convert_endian<std::endian::big, std::endian::little> failed."
            << std::endl;
    }

    constexpr auto test_big_big{
        convert_endian<std::endian::big, std::endian::big>( 0x78563412u )
        == 0x78563412u
    };
    if ( !test_big_big ) {
        std::cerr
            << "convert_endian<std::endian::big, std::endian::big> failed."
            << std::endl;
    }

    return test_little_little && test_little_big && test_big_little
           && test_big_big;
}

constexpr bool
test_lsb_msb() {
    constexpr auto test_little_lsb_32{ lsb<std::endian::little, 32> == 31 };
    if ( !test_little_lsb_32 ) {
        std::cerr << "lsb<std::endian::little, 32> failed." << std::endl;
    }
    constexpr auto test_little_msb_32{ msb<std::endian::little, 32> == 0 };
    if ( !test_little_msb_32 ) {
        std::cerr << "msb<std::endian::little, 32> failed." << std::endl;
    }

    constexpr auto test_big_lsb_32{ lsb<std::endian::big, 32> == 0 };
    if ( !test_big_lsb_32 ) {
        std::cerr << "lsb<std::endian::big, 32> failed." << std::endl;
    }
    constexpr auto test_big_msb_32{ msb<std::endian::big, 32> == 31 };
    if ( !test_big_msb_32 ) {
        std::cerr << "msb<std::endian::big, 32> failed." << std::endl;
    }

    return test_little_lsb_32 && test_little_msb_32 && test_big_lsb_32
           && test_big_msb_32;
}

constexpr bool
test_lsb_msb_offset() {
    constexpr auto test_little_lsb_offset_0{
        lsb_offset<std::endian::little, 0, 32>() == 31
    };
    if ( !test_little_lsb_offset_0 ) {
        std::cerr << "lsb_offset<std::endian::little, 0, 32> failed."
                  << std::endl;
    }
    constexpr auto test_little_lsb_offset_max{
        lsb_offset<std::endian::little, 31, 32>() == 0
    };
    constexpr auto test_little_msb_offset_0{};
    constexpr auto test_little_msb_offset_max{};
    constexpr auto test_big_lsb_offset_0{};
    constexpr auto test_big_lsb_offset_max{};
    constexpr auto test_big_msb_offset_0{};
    constexpr auto test_big_msb_offset_max{};

    return test_little_lsb_offset_0 && test_little_lsb_offset_max
           && test_little_msb_offset_0 && test_little_msb_offset_max
           && test_big_lsb_offset_0 && test_big_lsb_offset_max
           && test_big_msb_offset_0 && test_big_msb_offset_max;
}

constexpr bool
test_span_to_integer() {}

const auto test_functions = std::vector<std::function<bool()>>{
    test_convert_endian, test_lsb_msb, test_lsb_msb_offset, test_span_to_integer
};

int
common_test( [[maybe_unused]] int argc, [[maybe_unused]] char ** argv ) {
    std::size_t test_passes{ 0 };

    for ( const auto & test : test_functions )
        test_passes += ( test() ? 1 : 0 );

    return test_functions.size() - test_passes;
}
