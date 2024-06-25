cmake_minimum_required(VERSION 3.8)

project(relay)

add_definitions("-Wall")

include_directories(
	${PROJECT_SOURCE_DIR}/
	)

aux_source_directory(. SRCS)
aux_source_directory(gpio SRCS)
aux_source_directory(system SRCS)
aux_source_directory(network SRCS)
aux_source_directory(time SRCS)

add_executable(${PROJECT_NAME}
	${SRCS}
	)

message(STATUS "SRCS=${SRCS}")

target_link_libraries(${PROJECT_NAME}
	pthread
	rt
	)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION /usr/bin)
