cmake_minimum_required(VERSION 3.18)

#warnnig setting
if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall)# -fsanitize=thread)
endif()
set(CMAKE_C_STANDARD 11)

if(WIN32)
	set(LINK_TARGET m pthread)
else()
	set(LINK_TARGET m pthread dl)# tsan)
endif()

set(CONT_TARGET_NAME cont)
file(GLOB CONT_SOURCE "c/*.c")

project(${CONT_TARGET_NAME})

add_subdirectory(fakeLisp)

include_directories("${PROJECT_SOURCE_DIR}/fakeLisp/include")

#cont.so
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(${CONT_TARGET_NAME}
	MODULE
	${CONT_SOURCE})
set_target_properties(${CONT_TARGET_NAME} PROPERTIES PREFIX "")

target_link_libraries(${CONT_TARGET_NAME} fkl)

#copy cont.fkl
add_custom_target(COPY_CONT_FKL ALL
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/cont.fkl ${CMAKE_BINARY_DIR})

