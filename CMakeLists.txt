﻿cmake_minimum_required (VERSION 3.28)

project (kra_imp VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(pugixml QUIET)
if(NOT pugixml_FOUND)
	add_subdirectory(pugixml)
endif()

find_package(zip QUIET)
if(NOT zip_FOUND)
	add_subdirectory(zip)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in
	${CMAKE_CURRENT_BINARY_DIR}/generated/kra_imp/config.hpp
	@ONLY
)

option(KRA_IMP_BUILD_DOCS "Generate documentation with Doxygen for kra_imp library" OFF)

add_library(kra_imp_shared SHARED)
add_library(kra_imp_static STATIC)

foreach(target kra_imp_shared kra_imp_static)
	target_sources(${target}
		PRIVATE
		src/kra_imp.cpp
		src/lzf/lzf_d.c
		src/lzf/lzf_c.c
		src/lzf/lzfP.h
		src/lzf/lzf.h
		PRIVATE
		inc/kra_imp/kra_imp.hpp
		inc/kra_imp/types.hpp
		${CMAKE_CURRENT_BINARY_DIR}/generated/kra_imp/config.hpp
	)

	target_include_directories(${target}
		PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
		$<INSTALL_INTERFACE:include>
	)
	
	target_link_libraries(${target}
		PRIVATE
		pugixml::pugixml
		zip::zip
	)
endforeach()

target_compile_definitions(kra_imp_shared PRIVATE kra_imp_EXPORTS)
target_compile_definitions(kra_imp_static PUBLIC KRA_IMP_STATIC)

set_target_properties(kra_imp_shared PROPERTIES OUTPUT_NAME kra_imp)
set_target_properties(kra_imp_static PROPERTIES OUTPUT_NAME kra_imp_static)

add_library(kra_imp::shared ALIAS kra_imp_shared)
add_library(kra_imp::static ALIAS kra_imp_static)

set_property(TARGET kra_imp_shared PROPERTY EXPORT_NAME shared)
set_property(TARGET kra_imp_static PROPERTY EXPORT_NAME static)

install(DIRECTORY inc/kra_imp DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/kra_imp DESTINATION include)
install(TARGETS
	kra_imp_shared kra_imp_static
    EXPORT kra_impTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(EXPORT kra_impTargets
	FILE kra_impTargets.cmake
	NAMESPACE kra_imp::
	DESTINATION lib/cmake/kra_imp
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/kra_impConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/kra_impConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/kra_impConfig.cmake
	INSTALL_DESTINATION lib/cmake/kra_imp
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/kra_impConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/kra_impConfigVersion.cmake
	DESTINATION lib/cmake/kra_imp
)

if (BUILD_TESTING)
	enable_testing()
	add_subdirectory(Catch2)
	add_subdirectory(tests)
endif()

if (KRA_IMP_BUILD_DOCS)
	add_subdirectory(docs)
endif()
