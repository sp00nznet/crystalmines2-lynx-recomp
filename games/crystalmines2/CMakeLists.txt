# Chip's Challenge - recompiled executable.
#
# Links the per-game host (src/main.c) + the recompiler-generated C against the
# lynxrecomp runtime. The generated file is produced from your own ROM:
#   m65c02recomp emit "roms/Chip's Challenge (USA, Europe).lnx" games/crystalmines2/generated
add_executable(crystalmines2
    src/main.c
    generated/recomp_funcs.c
)

target_include_directories(crystalmines2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

target_link_libraries(crystalmines2 PRIVATE lynxrecomp)

# The generated C is machine-emitted; keep its warnings quiet.
if(MSVC)
    target_compile_options(crystalmines2 PRIVATE /W1 /wd4244)
else()
    target_compile_options(crystalmines2 PRIVATE -w)
endif()
