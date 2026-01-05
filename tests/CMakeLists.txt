include(FetchContent)

find_package(Catch2 3.12 CONFIG QUIET)
if(NOT Catch2_FOUND)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.12.0
    )
    FetchContent_MakeAvailable(Catch2)
endif()

add_executable(kra_imp_test
    test.cpp
    archive_tests.cpp
    delinearize_tests.cpp
    image_frames_tests.cpp
    image_layer_tests.cpp
    main_doc_tests.cpp
    read_layer_data_tests.cpp
    read_layer_header_tests.cpp
)

target_link_libraries(kra_imp_test
    PRIVATE
    Catch2::Catch2WithMain
    kra_imp::static
)

include(Catch)
catch_discover_tests(kra_imp_test)
