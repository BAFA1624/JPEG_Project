# Future Planning

## Structure

 - Png structure holding basic data for the PNG with raw data accessor methods.
 - PngDecoder class to parse in a filesystem object as a png.
 - PngEncoder to write out a parsed Png to a file.
 - PngEditor to apply edits to a Png.

## Next Work

 - [ ] Plan *PngChunkPayload* base class. [PngChunkPayload](png/png_chunk_payload.md)
 - [ ] Implement *PngChunkPayload* base class. [PngChunkPayload](png/png_chunk_payload.md)
 - [ ] Unit tests for *PngChunkPayload* base class.
 - [ ] Implement critical *PngChunkPayload*s. [PngChunkPayload Critical Chunks](png/png_chunk_payload.md#critical-chunks)
 - [ ] Unit tests for critical *PngChunkPayload*s.
 - [ ] Implement *PngChunk* class. [PngChunk](png/png_chunk.md)
 - [ ] Unit tests for *PngChunk* class.
