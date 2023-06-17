set(PROJECT_NAME "turbine")
project(${PROJECT_NAME})

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp *.hpp *.c *.h *.rc)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${SOURCE_FILES})

include_directories(
  "."
  "ext"
  "ext/imgui"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${RESOURCES})

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    CURL::libcurl
    freetype
    GL
    pthread
    SDL2::SDL2 
    SDL2::SDL2main
    SDL2::SDL2-static
    SDL2::SDL2_image
)

set(OUTPUT_BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../bin)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD                            17
    RUNTIME_OUTPUT_DIRECTORY                "${OUTPUT_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${OUTPUT_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${OUTPUT_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${OUTPUT_BIN_DIR}"
    DEBUG_POSTFIX                           _d
    RELWITHDEBINGO_POSTFIX                  _rd
    MINSIZEREL_POSTFIX                      _r
    VS_DEBUGGER_WORKING_DIRECTORY           ${OUTPUT_BIN_DIR}
)

target_compile_options(turbine PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:
        -fstandalone-debug
    >
    $<$<CXX_COMPILER_ID:GNU>:
        -ggdb
    >
)