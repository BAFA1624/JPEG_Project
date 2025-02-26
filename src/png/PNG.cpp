#include "png/PNG.hpp"

std::map<ChunkType, std::string> TypeToString
    {
        {ChunkType::INVALID, "INVALID"},
        {ChunkType::IHDR, "IHDR"},
        {ChunkType::PLTE, "PLTE"},
        {ChunkType::IDAT, "IDAT"},
        {ChunkType::IEND, "IEND"},
        {ChunkType::bKGD, "bKGD"},
        {ChunkType::cHRM, "cHRM"},
        {ChunkType::dSIG, "dSIG"},
        {ChunkType::eXIF, "eXIF"},
        {ChunkType::gAMA, "gAMA"},
        {ChunkType::hIST, "hIST"},
        {ChunkType::iCCP, "iCCP"},
        {ChunkType::iTXt, "iTXt"},
        {ChunkType::pHYs, "pHYs"},
        {ChunkType::sBIT, "sBIT"},
        {ChunkType::sPLT, "sPLT"},
        {ChunkType::sRGB, "sRGB"},
        {ChunkType::sTER, "sTER"},
        {ChunkType::tEXt, "tEXt"},
        {ChunkType::tIME, "tIME"},
        {ChunkType::tRNS, "tRNS"},
        {ChunkType::zTXt, "zTXt"}
    };
std::map<std::string, ChunkType> StringToType
    {
        {"INVALID", ChunkType::INVALID},
        {"IHDR", ChunkType::IHDR},
        {"PLTE", ChunkType::PLTE},
        {"IDAT", ChunkType::IDAT},
        {"IEND", ChunkType::IEND},
        {"bKGD", ChunkType::bKGD},
        {"cHRM", ChunkType::cHRM},
        {"dSIG", ChunkType::dSIG},
        {"eXIF", ChunkType::eXIF},
        {"gAMA", ChunkType::gAMA},
        {"hIST", ChunkType::hIST},
        {"iCCP", ChunkType::iCCP},
        {"iTXt", ChunkType::iTXt},
        {"pHYs", ChunkType::pHYs},
        {"sBIT", ChunkType::sBIT},
        {"sPLT", ChunkType::sPLT},
        {"sRGB", ChunkType::sRGB},
        {"sTER", ChunkType::sTER},
        {"tEXt", ChunkType::tEXt},
        {"tIME", ChunkType::tIME},
        {"tRNS", ChunkType::tRNS},
        {"zTXt", ChunkType::zTXt}
    };

bool InitChunk(const unsigned char* _start, PNG_CHUNK* _chunk) noexcept
{
    std::cout << "InitChunk started:" << std::endl;
    if (_start == nullptr)
        return false;

    unsigned char length[4];
    std::memcpy(length, _start, 4);
    _chunk->_len = (length[0] << 24) | (length[1] << 16) | (length[2] << 8) | length[3];
    std::cout << "\tChunk length: " << _chunk->_len << std::endl;

    try
    {
        _chunk->_type = StringToType.at(std::string((char*) _start + 4, 4));
        std::cout << "\tChunk type: " << TypeToString.at(_chunk->_type) << std::endl;
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "\t" << std::string((char*) _start + 4, 4) << std::endl;
        std::cout << "\tChunk type: " << "UNKNOWN" << std::endl;
    }

    _chunk->_data = new char[_chunk->_len];
    std::memcpy(_chunk->_data, _start + 8, _chunk->_len);

    if (_chunk->_type == ChunkType::tEXt)
    {
        std::string data{ _chunk->_data, _chunk->_len };
        std::string key = data.substr(0, data.find('\0'));
        std::string value = data.substr(key.length() + 1, data.length());
        std::cout << key << " : " << value << std::endl;
    }

    std::memcpy(length, _start + _chunk->_len + 8, 4);
    _chunk->_CRC = (length[0] << 24) | (length[1] << 16) | (length[2] << 8) | length[3];

    uint32_t calc_crc = CRC32(_start + 4, _chunk->_len + 4);

    char tmp[4];
    std::memcpy(tmp, _start + _chunk->_len + 8, 4);
    uint32_t tmp_val = (tmp[0] << 24) | (tmp[1] << 16) | (tmp[2] << 8) | tmp[3];
    std::cout << "\ttmp: " << tmp_val << std::endl;

    if (_chunk->_CRC != calc_crc)
    {
        std::cout << "\tCRC32: " << _chunk->_CRC << std::endl;
        std::cout << "\tCalc CRC32: " << calc_crc << std::endl;
        return false;
    }

    std::cout << "InitChunk complete." << std::endl;

    return true;
}
bool DeleteChunk(PNG_CHUNK* chunk) noexcept
{
    if (chunk == nullptr)
        return false;

    if (chunk->_data != nullptr)
        delete[] chunk->_data;

    delete chunk;

    return true;
}

bool ParsePNG(const std::filesystem::directory_entry& _file, PNG& _png) noexcept
{
	// Check validity
    if (!_file.is_regular_file() || !_file.exists())
    {
        std::cerr << "File " << _file.path().string() << " is not a regular file or does not exist." << std::endl;
        return false;
    }
    if (_file.path().extension() != ".png")
    {
        std::cerr << "File " << _file.path().string() << " is not a PNG file." << std::endl;
        return false;
    }

    // Get file size
	uint32_t fsize = _file.file_size();

	// Open file in binary read mode
	std::fstream fp;
	fp.open(_file.path().string().c_str(), std::fstream::in | std::ios_base::binary);

    if (!fp.is_open())
    {
        std::cerr << "Failed to open file: " << _file.path().string() << std::endl;
        return false;
    }

    // Read data in
	unsigned char* buf = new unsigned char[fsize];
	fp.get((char*)(&buf[0]), fsize, EOF);

    // Check stream bits to verify success
    if (fp.fail())
    {
        std::cerr << "Failed to read file: " << _file.path().string() << std::endl;
        return false;
    }

    // Check PNG header
    if (std::memcmp(buf, "\x89PNG\x0D\x0A\x1A\x0A", 8) != 0)
    {
        std::cerr << "File " << _file.path().string() << " is not a PNG file." << std::endl;
        return false;
    }

    // Parse chunks
    ChunkType type{ ChunkType::INVALID };
    uint32_t offset{ 8 };
    do
    {
        auto chunk = new PNG_CHUNK();

        std::cout << "\nInitializing chunk..." << std::endl;
        if (!InitChunk(buf + offset, chunk))
        {
            offset += (12 + chunk->_len);
            std::cout << offset << " / " << fsize << std::endl;
            std::cerr << "Failed to parse chunk." << std::endl;
            return false;
        }
        std::cout << "Chunk initialized." << std::endl;

        type = chunk->_type;

        offset += (12 + chunk->_len);
        _png._chunk_data.push_back(std::move(*chunk));

        std::cout << "Chunk added to PNG.\n" << std::endl;
    }
    while ((type != ChunkType::IEND || type != ChunkType::INVALID) && offset < fsize);

    // Delete buffer & close file
	delete[] buf;
    fp.close();

	return true;
}
