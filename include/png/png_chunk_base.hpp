#pragma once // PNG_CHUNK_BASE_HPP

#include "png/png_types.hpp"

#include <memory>

namespace PNG
{

class PngChunkBase
{
    public:
    // The base class should not be constructible or assignable
    // Constructors
    PngChunkBase() = delete;
    PngChunkBase( const PngChunkBase & other ) = delete;
    PngChunkBase( PngChunkBase && other ) = delete;
    // Assignment
    PngChunkBase & operator=( const PngChunkBase & rhs ) = delete;
    PngChunkBase & operator=( PngChunkBase && rhs ) = delete;
    virtual ~PngChunkBase() = default;

    // Unique ptr type to the PngChunkBase base class
    using const_base_pointer_t = std::shared_ptr<const PngChunkBase>;
    using base_pointer_t = std::shared_ptr<PngChunkBase>;

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

    friend std::ostream & operator<<( std::ostream &       os,
                                      const PngChunkBase & png_chunk );

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
    std::is_base_of_v<PngChunkBase, Child> && Parent::template accepts<Child>();

} // namespace PNG
