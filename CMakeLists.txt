
set(SABRE_NAME sabre)
add_library(${SABRE_NAME} 
	src/sabre/client.cpp
	src/sabre/server.cpp 
	src/sabre/event.cpp 
	src/sabre/packet_factory.cpp
)

target_include_directories(${SABRE_NAME} include)
target_include_directories(${PROJECT_NAME} include)

#Set C++17
target_compile_features(${SABRE_NAME} PUBLIC cxx_std_17)
set_target_properties(${SABRE_NAME} PROPERTIES CXX_EXTENSIONS OFF)

#Set flags
if(MSVC)
  	target_compile_options(${SABRE_NAME} PRIVATE 
    	/W4 /WX)
else()
  	target_compile_options(${SABRE_NAME} PRIVATE 
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
