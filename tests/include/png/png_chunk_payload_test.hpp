#pragma once

#include "png/png_chunk_payload.hpp"
#include "test_interface.hpp"


namespace PNG
{

template <typename Payload, typename... Args>
    requires std::constructible_from<Payload, Args...>
constexpr bool
test_payload( const Args &&... args ) {
    constexpr auto payload = Payload{ std::forward<Args>( args )... };
    return payload.isValid();
}

bool test_png_chunk_payload_base();

// Critical Chunks
bool test_ihdr_payload();
bool test_plte_payload();
bool test_idat_payload();
bool test_iend_payload();

// Ancillary Chunks

const auto test_functions = std::vector{ test_png_chunk_payload_base,
                                         test_ihdr_payload,
                                         test_plte_payload,
                                         test_idat_payload,
                                         test_iend_payload };

} // namespace PNG
