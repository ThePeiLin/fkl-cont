cmake_minimum_required(VERSION 3.18)

set(CONT_TARGET_NAME cont)
file(GLOB CONT_SOURCE "c/*.c")

#cont.so
add_library(${CONT_TARGET_NAME}
	MODULE
	${CONT_SOURCE})
set_target_properties(${CONT_TARGET_NAME} PROPERTIES PREFIX "")

target_link_libraries(${CONT_TARGET_NAME} ${LIBFKL_TARGET_NAME})
