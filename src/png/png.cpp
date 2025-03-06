#include "png/png.hpp"

#include <cassert>
#include <iostream>
#include <map>
#include <string>

namespace PNG
{

[[nodiscard]] constexpr bool
verify_png_header( const std::bitset<64> & header_bits ) noexcept {
    constexpr std::bitset<64> expected_header{ 0x89'504E47'0D0A'1A'0A };
    return header_bits == expected_header;
}

constexpr PNG::PNG() :
    valid_png( false ), header_bits( 0 ), png_raw( 0 ), png_chunks( 0 ) {}

constexpr PNG::PNG( const std::string_view raw_data ) :
    valid_png( true ),
    header_bits( raw_data.data(), 8 ),
    png_raw( 0 ),
    png_chunks( 0 ) {
    // Check first 8 bytes for valid header type
    assert( raw_data.size() >= 8 );

    // Verify valid png header
    if ( !( valid_png = verify_header() ) ) {
        // TODO: Add some handling
    }

    // Maintains current offset in the raw data
    std::size_t data_offset{ 8 };

    // Read in raw png data


    // Read PNG blocks
    do {
        // Parse block size
        const std::uint32_t chunk_sz{ *reinterpret_cast<const std::uint32_t *>(
            raw_data.data() + data_offset ) };
        data_offset += sizeof( std::uint32_t );

        // Parse block type
        const png_chunk_t chunk_type{ *reinterpret_cast<const png_chunk_t *>(
            raw_data.data() + data_offset ) };
        data_offset += sizeof( png_chunk_t );

        // Parse block
        // Calculate & verify CRC
        // Build PNGChunk
        png_chunks.emplace_back();
    } while ( true );
}

} // namespace PNG

// OLD CODE:
/*
// clang-format off
std::map<png_chunk_t, std::string> TypeToString {
    {png_chunk_t::INVALID, "INVALID"},
    {png_chunk_t::IHDR, "IHDR"},
    {png_chunk_t::PLTE, "PLTE"},
    {png_chunk_t::IDAT, "IDAT"},
    {png_chunk_t::IEND, "IEND"},
    {png_chunk_t::bKGD, "bKGD"},
    {png_chunk_t::cHRM, "cHRM"},
    {png_chunk_t::dSIG, "dSIG"},
    {png_chunk_t::eXIF, "eXIF"},
    {png_chunk_t::gAMA, "gAMA"},
    {png_chunk_t::hIST, "hIST"},
    {png_chunk_t::iCCP, "iCCP"},
    {png_chunk_t::iTXt, "iTXt"},
    {png_chunk_t::pHYs, "pHYs"},
    {png_chunk_t::sBIT, "sBIT"},
    {png_chunk_t::sPLT, "sPLT"},
    {png_chunk_t::sRGB, "sRGB"},
    {png_chunk_t::sTER, "sTER"},
    {png_chunk_t::tEXt, "tEXt"},
    {png_chunk_t::tIME, "tIME"},
    {png_chunk_t::tRNS, "tRNS"},
    {png_chunk_t::zTXt, "zTXt"}
};
std::map<std::string, png_chunk_t> StringToType {
    {"INVALID", png_chunk_t::INVALID},
    {"IHDR", png_chunk_t::IHDR},
    {"PLTE", png_chunk_t::PLTE},
    {"IDAT", png_chunk_t::IDAT},
    {"IEND", png_chunk_t::IEND},
    {"bKGD", png_chunk_t::bKGD},
    {"cHRM", png_chunk_t::cHRM},
    {"dSIG", png_chunk_t::dSIG},
    {"eXIF", png_chunk_t::eXIF},
    {"gAMA", png_chunk_t::gAMA},
    {"hIST", png_chunk_t::hIST},
    {"iCCP", png_chunk_t::iCCP},
    {"iTXt", png_chunk_t::iTXt},
    {"pHYs", png_chunk_t::pHYs},
    {"sBIT", png_chunk_t::sBIT},
    {"sPLT", png_chunk_t::sPLT},
    {"sRGB", png_chunk_t::sRGB},
    {"sTER", png_chunk_t::sTER},
    {"tEXt", png_chunk_t::tEXt},
    {"tIME", png_chunk_t::tIME},
    {"tRNS", png_chunk_t::tRNS},
    {"zTXt", png_chunk_t::zTXt}
};
// clang-format on

bool
InitChunk( const unsigned char * _start, PNG_CHUNK * _chunk ) noexcept {
    std::cout << "InitChunk started:" << std::endl;
    if ( _start == nullptr )
        return false;

    unsigned char length[4];
    std::memcpy( length, _start, 4 );
    _chunk->_len = ( length[0] << 24 ) | ( length[1] << 16 )
                   | ( length[2] << 8 ) | length[3];
    std::cout << "\tChunk length: " << _chunk->_len << std::endl;

    try {
        _chunk->_type =
            StringToType.at( std::string( ( char * ) _start + 4, 4 ) );
        std::cout << "\tChunk type: " << TypeToString.at( _chunk->_type )
                  << std::endl;
    }
    catch ( const std::out_of_range & e ) {
        std::cout << "\t" << std::string( ( char * ) _start + 4, 4 )
                  << std::endl;
        std::cout << "\tChunk type: " << "UNKNOWN" << std::endl;
    }

    _chunk->_data = new char[_chunk->_len];
    std::memcpy( _chunk->_data, _start + 8, _chunk->_len );

    if ( _chunk->_type == ChunkType::tEXt ) {
        std::string data{ _chunk->_data, _chunk->_len };
        std::string key = data.substr( 0, data.find( '\0' ) );
        std::string value = data.substr( key.length() + 1, data.length() );
        std::cout << key << " : " << value << std::endl;
    }

    std::memcpy( length, _start + _chunk->_len + 8, 4 );
    _chunk->_CRC = ( length[0] << 24 ) | ( length[1] << 16 )
                   | ( length[2] << 8 ) | length[3];

    uint32_t calc_crc = CRC32( _start + 4, _chunk->_len + 4 );

    char tmp[4];
    std::memcpy( tmp, _start + _chunk->_len + 8, 4 );
    uint32_t tmp_val =
        ( tmp[0] << 24 ) | ( tmp[1] << 16 ) | ( tmp[2] << 8 ) | tmp[3];
    std::cout << "\ttmp: " << tmp_val << std::endl;

    if ( _chunk->_CRC != calc_crc ) {
        std::cout << "\tCRC32: " << _chunk->_CRC << std::endl;
        std::cout << "\tCalc CRC32: " << calc_crc << std::endl;
        return false;
    }

    std::cout << "InitChunk complete." << std::endl;

    return true;
}
bool
DeleteChunk( PNG_CHUNK * chunk ) noexcept {
    if ( chunk == nullptr )
        return false;

    if ( chunk->_data != nullptr )
        delete[] chunk->_data;

    delete chunk;

    return true;
}

bool
ParsePNG( const std::filesystem::directory_entry & _file,
          PNG &                                    _png ) noexcept {
    // Check validity
    if ( !_file.is_regular_file() || !_file.exists() ) {
        std::cerr << "File " << _file.path().string()
                  << " is not a regular file or does not exist." << std::endl;
        return false;
    }
    if ( _file.path().extension() != ".png" ) {
        std::cerr << "File " << _file.path().string() << " is not a PNG file."
                  << std::endl;
        return false;
    }

    // Get file size
    uint32_t fsize = _file.file_size();

    // Open file in binary read mode
    std::fstream fp;
    fp.open( _file.path().string().c_str(),
             std::fstream::in | std::ios_base::binary );

    if ( !fp.is_open() ) {
        std::cerr << "Failed to open file: " << _file.path().string()
                  << std::endl;
        return false;
    }

    // Read data in
    unsigned char * buf = new unsigned char[fsize];
    fp.get( ( char * ) ( &buf[0] ), fsize, EOF );

    // Check stream bits to verify success
    if ( fp.fail() ) {
        std::cerr << "Failed to read file: " << _file.path().string()
                  << std::endl;
        return false;
    }

    // Check PNG header
    if ( std::memcmp( buf, "\x89PNG\x0D\x0A\x1A\x0A", 8 ) != 0 ) {
        std::cerr << "File " << _file.path().string() << " is not a PNG file."
                  << std::endl;
        return false;
    }

    // Parse chunks
    ChunkType type{ ChunkType::INVALID };
    uint32_t  offset{ 8 };
    do {
        auto chunk = new PNG_CHUNK();

        std::cout << "\nInitializing chunk..." << std::endl;
        if ( !InitChunk( buf + offset, chunk ) ) {
            offset += ( 12 + chunk->_len );
            std::cout << offset << " / " << fsize << std::endl;
            std::cerr << "Failed to parse chunk." << std::endl;
            return false;
        }
        std::cout << "Chunk initialized." << std::endl;

        type = chunk->_type;

        offset += ( 12 + chunk->_len );
        _png._chunk_data.push_back( std::move( *chunk ) );

        std::cout << "Chunk added to PNG.\n" << std::endl;
    } while ( ( type != ChunkType::IEND || type != ChunkType::INVALID )
              && offset < fsize );

    // Delete buffer & close file
    delete[] buf;
    fp.close();

    return true;
}*/
