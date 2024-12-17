add_executable(kra_imp_test
	test.cpp
	main_doc_tests.cpp
)

target_link_libraries(kra_imp_test
	PRIVATE
	Catch2::Catch2WithMain
	kra_imp
)

include(CTest)
include(Catch)
catch_discover_tests(kra_imp_test)
