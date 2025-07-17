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
The IHDR chunk is the image header, it is always the first chunk in the PNG datastream.
It's data section contains:

##### Table 1: IHDR contents.
| Field Name         | Field Size (Bytes) |
| ------------------ | ------------------ |
| Width              | 4                  |
| Height             | 4                  |
| Bit Depth          | 1                  |
| Colour Type        | 1                  |
| Compression Method | 1                  |
| Filter Method      | 1                  |
| Interlace Method   | 1                  |

For a total size of 13 bytes.

- Width & Height: 4 byte unsigned integers giving the image width & height in pixels. Zero is an invalid value.
- Bit Depth: 1 byte integer giving the number of bits per sample or per palette index (not per pixel).
Valid values are 1, 2, 4, 8, & 16, although not all values are allowed for all colour types.
- Colour Type: 1 byte integer that defines the PNG image type. Valid values are 0, 2, 3, 4, & 6.
A table of the allowed bit depths for different colour types can be found below.
- Filter Method: 1 byte integer that indicates the preprocessing method applied to the image data before
compression. Only filter method 0 (adaptive filtering with 5 basic filter types) is defined in the
international standard.
- Interlace Method: 1 byte integer indicating the transmission order of the image data. 2 values are defined
in the international standard - 0 (no interlace), or 1 (Adam7 interlace).


### PLTE - Palette
### IDAT - Image Data
### IEND - Image Trailer
