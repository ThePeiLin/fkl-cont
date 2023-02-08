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
add_library(${CONT_TARGET_NAME}
	MODULE
	${CONT_SOURCE})
set_target_properties(${CONT_TARGET_NAME} PROPERTIES PREFIX "")

target_link_libraries(${CONT_TARGET_NAME} fkl)
