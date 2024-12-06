cmake_minimum_required (VERSION 3.22)

project (kra_imp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)

option(BUILD_SHARED_LIBS "Build shared library" OFF)

set(KRAIMP_SRC
	inc/kra_imp/kra_imp.hpp
	inc/kra_imp/config.hpp
	inc/kra_imp/types.hpp
	src/kra_imp.cpp
	src/lzf/lzf.h
	src/lzf/lzf_d.c
	src/lzf/lzf_c.c
	src/lzf/lzfP.h
)

if(BUILD_SHARED_LIBS)
    add_library(kra_imp
		${KRAIMP_SRC}
	)
else()
	add_library(kra_imp
		STATIC
		${KRAIMP_SRC}
	)
	
	target_compile_definitions(kra_imp
		PUBLIC
		KRA_IMP_STATIC
	)
	
	set_target_properties(kra_imp
		PROPERTIES
		OUTPUT_NAME
		KraImpStatic
	)
endif()

target_include_directories(kra_imp
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/inc>
)

target_link_libraries(kra_imp
	PRIVATE
	zip
	pugixml
)

add_subdirectory(pugixml)
add_subdirectory(zip)

if (BUILD_TESTING)
	add_subdirectory(Catch2)
	add_subdirectory(test)
endif()

option(KRA_IMP_BUILD_DOC "Generate documentation with Doxygen for kra_imp library" ON)
if (KRA_IMP_BUILD_DOC)
	add_subdirectory(doc)
endif()
