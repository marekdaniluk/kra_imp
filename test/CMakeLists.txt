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
	kra_imp
)

include(CTest)
include(Catch)
catch_discover_tests(kra_imp_test)
