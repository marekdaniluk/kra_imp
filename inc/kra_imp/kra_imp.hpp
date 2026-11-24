/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once

#include <kra_imp/export.h>
#include <kra_imp/IArchive.hpp>
#include <kra_imp/Importer.hpp>
#include <kra_imp/IXmlNode.hpp>
#include <kra_imp/IXmlParser.hpp>
#include <kra_imp/Types.hpp>
#include <kra_imp/XmlParser.hpp>
#include <kra_imp/ZipArchive.hpp>

namespace kra_imp
{
struct KRA_IMP_API Version
{
	unsigned int major{ 0U };
	unsigned int minor{ 0U };
	unsigned int patch{ 0U };
};

KRA_IMP_API Version getVersion();
} // namespace kra_imp