#pragma once

#define NOMOVE( class )         \
    class( class && ) = delete; \
    class & operator=( class && ) = delete;

#define NOCOPY( class )              \
    class( const class & ) = delete; \
    class & operator=( const class & ) = delete;

#define COLD [[unlikely]]
