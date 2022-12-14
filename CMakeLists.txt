# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("ebnf")

# Include sub-projects.
add_subdirectory ("src")

file(GLOB_RECURSE EEl_HEADERS CONFIGURE_DEPENDS include/*.h include/*.hpp)

add_library (ebnf STATIC ${EBNF_SOURCES} ${EEl_HEADERS})
set_property(TARGET ebnf PROPERTY CXX_STANDARD 20)

set_target_properties(ebnf
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/$<CONFIG>"
)

target_compile_definitions(ebnf PRIVATE _AMD64_)

#if(MSVC)
#    string(REGEX REPLACE "/W[1-3]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#else()
#    add_compile_options(-Wall -Wextra -pedantic -Werror)
#endif()

target_include_directories(ebnf PUBLIC "include")
target_include_directories(ebnf PRIVATE "src")

###############################

add_subdirectory ("tests")

add_executable (ebnf_test ${EBNF_TEST_SOURCES})
set_property(TARGET ebnf_test PROPERTY CXX_STANDARD 20)

add_dependencies(ebnf_test ebnf)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ebnf_test)

target_compile_definitions(ebnf_test PRIVATE _AMD64_)
    
target_link_directories(ebnf_test PRIVATE "lib")
target_link_libraries(ebnf_test ebnf)
target_include_directories(ebnf_test PUBLIC "include")
target_include_directories(ebnf_test PRIVATE "src")
target_include_directories(ebnf_test PRIVATE "tests")
