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

## Recommended Next Steps

- Keep `PngChunkSize<PngChunkType>` as the single chunk-size abstraction.
- Avoid reintroducing runtime `chunk_type` and `size` duplication inside payload
  bases unless there is a concrete parsing need that the typed model cannot
  express cleanly.
- When refactoring payload internals such as `PLTE`, preserve the typed-size
  payload API first and change storage layout underneath it second.
- Add payload tests at the same time as representation changes:
  - construction from raw bytes
  - moved-from invalid state
  - file-backed parsing from checked-in fixtures
  - accessor round-trips for any derived view such as reconstructed palettes
- Prefer one representation per payload at a time. Do not leave header and
  source files split between old and new layouts across rebases.
- If ancillary payload work continues, keep each chunk family in a separate
  commit with tests so rebases onto `main` stay mechanical.
