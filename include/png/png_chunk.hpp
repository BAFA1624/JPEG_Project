#pragma once // PNG_CHUNK_HPP

#include "png/png_types.hpp"

#include <deque>
#include <memory>

namespace PNG
{

class PngChunk
{
    public:
    PngChunk() = delete;
    virtual ~PngChunk() = default;

    // Unique ptr type to the PngChunk base class
    using const_base_pointer_t = const std::shared_ptr<const PngChunk>;
    using base_pointer_t = const std::shared_ptr<PngChunk>;

    [[nodiscard]] constexpr auto get_chunk_type() const noexcept {
        return m_chunk_type;
    }

    [[nodiscard]] base_pointer_t previous_chunk() { return m_previous_chunk; }
    [[nodiscard]] const_base_pointer_t previous_chunk() const {
        return m_previous_chunk;
    }

    [[nodiscard]] base_pointer_t       next_chunk() { return m_next_chunk; }
    [[nodiscard]] const_base_pointer_t next_chunk() const {
        return m_next_chunk;
    }

    base_pointer_t set_previous_chunk();
    base_pointer_t set_next_chunk();

    bool insert_chunk_before( const base_pointer_t new_chunk );
    bool insert_chunk_after( const base_pointer_t new_chunk );

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

class PngChunkIHDR : public PngChunk
{};

} // namespace PNG
