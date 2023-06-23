cmake_minimum_required(VERSION 3.22)
project(ParallelDB)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_BINARY_DIR})

include(Conan NO_POLICY_SCOPE)
set(DEPENDENCY_LIBS boost/1.80.0 catch2/3.1.0 fmtlog/2.2.1 libpqxx/7.7.4 libpq/14.5)
conan_config_and_install("${DEPENDENCY_LIBS}")


file(GLOB_RECURSE "${PROJECT_NAME}_SRC" CONFIGURE_DEPENDS
        "src/*.cpp")

add_executable(${PROJECT_NAME} "${${PROJECT_NAME}_SRC}")

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# installing libraries with conan and linking libraries to the project

find_package(Boost)
find_package(fmtlog)
find_package(libpqxx)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Boost::system
	Boost::program_options
	Boost::json
	Boost::filesystem
	fmtlog::fmtlog
	libpqxx::libpqxx
)

function(check_env_vars ENV_LIST)
	foreach(ENV_VAR ${ENV_LIST})
		if (NOT DEFINED ENV{${ENV_VAR}})
			list(APPEND UNSET_VARS ${ENV_VAR})
		endif()
	endforeach()

	if (UnsetVars)
		list(JOIN UNSET_VARS ", " UNSET_VARS)
		message(FATAL_ERROR "Not all environment variables are set, please set them and try again: ${UNSET_VARS}")
	endif()
endfunction()

set(ENV_VARIABLES PARALLEL_DB_NAME PARALLEL_DB_IP PARALLEL_DB_PORT PARALLEL_DB_USER_NAME PARALLEL_DB_USER_PASSWORD)
check_env_vars("${ENV_VARIABLES}")
configure_file(${CMAKE_SOURCE_DIR}/templates/settings.json.in ${CMAKE_BINARY_DIR}/settings.json)
