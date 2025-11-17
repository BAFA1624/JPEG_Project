# Options List

- [x] Create ChunkSizeType:
    - Requirements:
        - Enum class
        - Describes if chunk size is constant or variable.
- [x] Create PngChunkSize:
    - Options:
        - [x] Opt 1:
            - Template parametrizable with a ChunkSizeType.
            - If constant $\rightarrow$ size defined at construction time time.
        - [ ] Opt 2:
            - Template parametrizable with a PngChunkType.
            - Templated PngChunkType:
                - Constant $\rightarrow$ sets size at compile time.
                - Variable $\rightarrow$ size can be set at runtime.

# Notes
## PngChunkSize
### Option 1
### Option 2
-
