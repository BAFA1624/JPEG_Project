[Top Level Plan](../plan.md)

# PngChunk Planning

The *PngChunk* class is implemented as a linked list with each chunk containing
a `std::unique_ptr<PngChunk>` to the next chunk in the file. The list itself is
managed by the *Png* class.

Each *PngChunk* indicates which type of chunk it is via it's `m_chunk_type` member variable.
The chunk specific functionality is maintained by the respective *PngChunkPayload* for that type.
The *PngChunk* itself uses a `std::unique_ptr<PngChunkPayload>`

## Base Class

### Member Types:
```cpp
using const_base_pointer_t = std::shared_ptr<const PngChunk>
using base_pointer_t = std::shared_ptr<PngChunk>
```
- Typedef for a `std::shared_ptr` to a `PngChunk`.
```cpp
template <typename T>
using const_pointer_t = std::shared_ptr<
```
- Typedef for a `std::shared_ptr` to a generic type.

### Member Variables
#### Private
```cpp
constexpr png_chunk_t m_chunk_type
```
- Constexpr enumeration value showing the current chunk type.

```cpp
constexpr std::unique_ptr<PngChunkPayload> m_payload
```
- Constexpr `std::unique_ptr` to a *[PngChunkPayload](./png_chunk_payload.md)* containing
any specific implementation for this chunk type.

```cpp
constexpr static const std::uint32_t m_size { 12 };
```
- The size of the data directly managed by the *PngChunk* class, e.g. the chunk size, chunk type, & cyclic redundancy code.
The total size of the chunk data is given by `m_size + m_payload->getSize()`.

#### Protected
```cpp
base_pointer_t m_previous_chunk
base_pointer_t m_next_chunk
```
- The previous/next `PngChunk` in the file.

#### Public
- There are no public member variables.

### Accessors
```cpp
constexpr png_chunk_t PngChunk::get_chunk_type() const
```
- Returns the current chunk type.

```cpp
[[nodiscard]] base_pointer_t PngChunk::previous_chunk()
[[nodiscard]] const_base_pointer_t PngChunk::previous_chunk() const
```
- Returns a pointer to the previous chunk in the file.
If there is no previous chunk (*IHDR* chunks only), holds a nullptr.

```cpp
[[nodiscard]] base_pointer_t PngChunk::next_chunk()
[[nodiscard]] const_base_pointer_t PngChunk::next_chunk() const
```
- Returns a pointer to the next chunk in the file.

```cpp
base_pointer_t PngChunk::set_previous_chunk(const base_pointer_t new_chunk)
base_pointer_t PngChunk::set_next_chunk(const base_pointer_t new_chunk)
```
- Sets the value of m\_previous\_chunk & m\_next\_chunk respectively. Returns the old value.

```cpp
[[nodiscard]] bool PngChunk::insert_chunk_before(const base_pointer_t new_chunk)
[[nodiscard]] bool PngChunk::insert_chunk_after(const base_pointer_t new_chunk)
```
- Insert a new chunk before/after the current chunk. Returns true if successful, false if not.

```cpp
template <typename ChildType>
static consteval bool accepts(
```

---

## Inheriting Classes:
