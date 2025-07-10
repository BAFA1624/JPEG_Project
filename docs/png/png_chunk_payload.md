[Top Level Plan](../plan.md), [PngChunk Plan](png_chunk.md)

# PngChunkPayload Planning

## Base Class

### Member Variables

#### Private:
```cpp
constexpr const static std::uint32_t m_size { 0 }
```
- The size of the data managed by the base class is defined to be 0 bytes always.
This value is overwritten by inheriting payloads.

#### Protected:
#### Public:

### Member Functions
#### Private:
#### Protected:
#### Public:
```cpp
[[nodiscard]] virtual constexpr auto getSize() const
```
- Returns the size of the data managed by this payload.

## Critical Chunks

### IHDR - Image Header
### PLTE - Palette
### IDAT - Image Data
### IEND - Image Trailer
