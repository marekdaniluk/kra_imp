add_executable(kralapp
	kralapp.cpp
	stb_image.h
	stb_image_write.h
)

target_link_libraries(kralapp
	PRIVATE
	kra_imp
)
