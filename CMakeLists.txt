# crystalmines2-lynx-recomp - reference game built on the lynxrecomp toolkit.
cmake_minimum_required(VERSION 3.16)
project(crystalmines2_lynx_recomp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# The toolkit, vendored as a git submodule. Builds the lynxrecomp runtime lib
# and the m65c02recomp recompiler CLI.
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lynxrecomp/CMakeLists.txt)
    message(FATAL_ERROR
        "lynxrecomp submodule missing. Run: git submodule update --init --recursive")
endif()
add_subdirectory(lynxrecomp)

add_subdirectory(games/crystalmines2)
