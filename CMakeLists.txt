
add_library(sabre 
	src/client.cpp
	src/server.cpp 
	src/event.cpp 
	src/packet_factory.cpp
)

target_include_directories(sabre PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE include)

#Set C++17
target_compile_features(sabre PUBLIC cxx_std_17)
set_target_properties(sabre PROPERTIES CXX_EXTENSIONS OFF)

#Set flags
if(MSVC)
  	target_compile_options(sabre PRIVATE 
    	/W4 /WX)
else()
  	target_compile_options(sabre PRIVATE 
		-Wall -Wextra -pedantic -Werror)
endif()

#Link libraries 

#Link threads
#find_package(Threads)
#target_link_libraries(sfnet Threads::Threads)

#Link SFML 
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#find_package(SFML REQUIRED network system)
#target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
