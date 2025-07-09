# Future Planning

## Structure

 - Png structure holding basic data for the PNG with raw data accessor methods.
 - PngDecoder class to parse in a filesystem object as a png.
 - PngEncoder to write out a parsed Png to a file.
 - PngEditor to apply edits to a Png.

## Next Work

 - [ ] Implement *PngChunkPayload* class. [PngChunkPayload](png/png_chunk_payload.md)
 - [ ] Implement PngChunk base class. [PngChunk](png/png_chunk.md)[[png_chunk.md]]
 - [ ] Implement classes for the critical png chunk types inheriting from the base class.
 - [ ] Implement PngChunk tree class.
    - [ ] Might need to implement simple linked list if you want.
    - [ ] Could use `std::deque` instead.
