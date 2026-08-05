/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <kra_imp/ZipArchive.hpp>
#include <zip.h>

namespace kra_imp
{
ZipArchive::ZipArchive(std::span<std::byte> data)
{
    static constexpr const char READ_MODE{ 'r' };
    _zipArchive = zip_stream_open(reinterpret_cast<const char*>(data.data()), data.size(), 0, READ_MODE);
}

ZipArchive::~ZipArchive()
{
    if (_zipArchive != nullptr)
    {
        zip_stream_close(_zipArchive);
    }
}

std::size_t ZipArchive::getFileSize(std::string_view filePath) const
{
    if (_zipArchive == nullptr || zip_entry_open(_zipArchive, filePath.data()) != 0)
    {
        return 0;
    }
    const std::size_t entrySize = static_cast<std::size_t>(zip_entry_size(_zipArchive));
    zip_entry_close(_zipArchive);
    return entrySize;
}

std::error_code ZipArchive::readFile(std::string_view filePath, std::span<std::byte> outBuffer)
{
    if (_zipArchive == nullptr)
    {
        return std::make_error_code(std::errc::bad_file_descriptor);
    }
    if (zip_entry_open(_zipArchive, filePath.data()) != 0)
    {
        return std::make_error_code(std::errc::no_such_file_or_directory);
    }
    const std::size_t readBytes = static_cast<std::size_t>(zip_entry_noallocread(_zipArchive, reinterpret_cast<char*>(outBuffer.data()), outBuffer.size()));
    zip_entry_close(_zipArchive);
    if (readBytes != outBuffer.size())
    {
        return std::make_error_code(std::errc::io_error);
    }
    return std::error_code{};
}
} // namespace kra_imp
