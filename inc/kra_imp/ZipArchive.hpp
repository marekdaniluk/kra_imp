/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#include <kra_imp/IArchive.hpp>

struct zip_t;

namespace kra_imp
{
class KRA_IMP_API ZipArchive final : public IArchive
{
  public:
    ZipArchive(std::span<std::byte> data);
    ~ZipArchive() override;
    std::size_t getFileSize(std::string_view filePath) const override;
    std::error_code readFile(std::string_view filePath, std::span<std::byte> outBuffer) override;
  private:
    zip_t* _zipArchive{ nullptr };
};
} // namespace kra_imp
