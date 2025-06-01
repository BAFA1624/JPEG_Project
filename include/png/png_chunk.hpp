#pragma once // PNG_CHUNK_HPP

#include "png/png_types.hpp"

#include <memory>

namespace PNG
{

class PngChunk
{
    public:
    // The base class should not be constructible or assignable
    // Constructors
    PngChunk() = delete;
    PngChunk( const PngChunk & other ) = delete;
    PngChunk( PngChunk && other ) = delete;
    // Assignment
    PngChunk & operator=( const PngChunk & rhs ) = delete;
    PngChunk & operator=( PngChunk && rhs ) = delete;
    virtual ~PngChunk() = default;

    // Unique ptr type to the PngChunk base class
    using const_base_pointer_t = std::shared_ptr<const PngChunk>;
    using base_pointer_t = std::shared_ptr<PngChunk>;

    // Returns the png_chunk_t for the current chunk
    [[nodiscard]] constexpr auto get_chunk_type() const noexcept {
        return m_chunk_type;
    }

    // Returns either a modifiable or const pointer to the previous chunk in the
    // chain.
    [[nodiscard]] base_pointer_t previous_chunk() { return m_previous_chunk; }
    [[nodiscard]] const_base_pointer_t previous_chunk() const {
        return m_previous_chunk;
    }

    [[nodiscard]] base_pointer_t       next_chunk() { return m_next_chunk; }
    [[nodiscard]] const_base_pointer_t next_chunk() const {
        return m_next_chunk;
    }

    [[maybe_unused]] constexpr base_pointer_t
    set_previous_chunk( const base_pointer_t new_chunk );
    [[maybe_unused]] constexpr base_pointer_t
    set_next_chunk( const base_pointer_t new_chunk );

    constexpr bool insert_chunk_before( base_pointer_t new_chunk );
    constexpr bool insert_chunk_after( base_pointer_t new_chunk );

    friend std::ostream & operator<<( std::ostream &   os,
                                      const PngChunk & png_chunk );

    protected:
    base_pointer_t m_previous_chunk;
    base_pointer_t m_next_chunk;

    template <typename ChildType>
    static consteval bool accepts() {
        return false;
    };

    private:
    static constexpr auto m_chunk_type{ png_chunk_t::INVALID };
};

template <typename Parent, typename Child>
concept ValidChild =
    std::is_base_of_v<PngChunk, Child> && Parent::template accepts<Child>();

class PngChunkIHDR : public PngChunk
{
    private:
    static constexpr auto m_chunk_type{ png_chunk_t::IHDR };
};

class PngChunkPLTE : public PngChunk
{
    private:
    static constexpr auto m_chunk_type{ png_chunk_t::PLTE };
};

class PngChunkIDAT : public PngChunk
{
    private:
    static constexpr auto m_chunk_type{ png_chunk_t::IDAT };
};

class PngChunkIEND : public PngChunk
{
    private:
    static constexpr auto m_chunk_type{ png_chunk_t::IEND };
};

} // namespace PNG
