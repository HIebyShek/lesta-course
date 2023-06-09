project(nano)

set(TARGET ${PROJECT_NAME})
set(imgui ${CMAKE_SOURCE_DIR}/external/imgui)

set(SDL3IMAGE_AVIF OFF)
set(SDL3IMAGE_JPG OFF)
set(SDL3IMAGE_JPG_SAVE OFF)
set(SDL3IMAGE_BMP OFF)
set(SDL3IMAGE_GIF OFF)
set(SDL3IMAGE_LBM OFF)
set(SDL3IMAGE_PCX OFF)
set(SDL3IMAGE_PNG_SAVE OFF)
set(SDL3IMAGE_PNM OFF)
set(SDL3IMAGE_QOI OFF)
set(SDL3IMAGE_SVG OFF)
set(SDL3IMAGE_SVG OFF)
set(SDL3IMAGE_TIF OFF)
set(SDL3IMAGE_WEBP OFF)
set(SDL3IMAGE_XCF OFF)
set(SDL3IMAGE_XPM OFF)
set(SDL3IMAGE_XV OFF)
set(SDL3IMAGE_PNG ON)

if (ANDROID)
execute_process (
    COMMAND bash -c "pushd ../../external &&
                    git submodule update --init SDL_image imgui  SDL
                    cd ../external/SDL_image/external/ &&
                    git submodule update --init libpng &&
                    popd"
    OUTPUT_VARIABLE INIT_SUBMODULES_SDL3IMAGE_IMGUI_RESULT
)
else()
execute_process (
    COMMAND bash -c "pushd ../external &&
                    git submodule update --init SDL_image imgui SDL &&
                    cd ../external/SDL_image/external/ &&
                    git submodule update --init libpng &&
                    popd"
    OUTPUT_VARIABLE INIT_SUBMODULES_SDL3IMAGE_IMGUI_RESULT
)
endif()

execute_process (
    COMMAND bash -c pushd ${EXTERNAL_DIR} &&
                    git submodule update --init SDL_image imgui  SDL
                    cd ../external/SDL_image/external/ &&
                    git submodule update --init libpng &&
                    popd
    OUTPUT_VARIABLE INIT_SUBMODULES_SDL3IMAGE_IMGUI_RESULT
)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL ${CMAKE_CURRENT_BINARY_DIR}/lib/SDL)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL_image ${CMAKE_CURRENT_BINARY_DIR}/lib/SDL_image)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glad ${CMAKE_CURRENT_BINARY_DIR}/lib/glad)

add_library(
    ${TARGET} SHARED

    src/engine.cpp
    src/event.cpp
    src/shader.cpp
    src/transform2D.cpp
    src/vertbuf.cpp
    src/texture2D.cpp
    src/shader.cpp
    src/shape.cpp
    src/canvas.cpp
    src/error.cpp
    src/utils.cpp
    src/soundbuf.cpp
    src/resource_loader.cpp
    src/sprite.cpp
    src/sound.cpp
    src/scene.cpp
    src/postman.cpp
    src/imgui_backend.cpp

    include/nano/imgui_backend.hpp
    include/nano/drawable.hpp
    include/nano/sound.hpp
    include/nano/sprite.hpp
    include/nano/soundbuf.hpp
    include/nano/utils.hpp
    include/nano/error.hpp
    include/nano/vertbuf.hpp
    include/nano/shape.hpp
    include/nano/shader.hpp
    include/nano/canvas.hpp
    include/nano/resource_loader.hpp
    include/nano/texture2D.hpp
    include/nano/shape.hpp
    include/nano/vertbuf.hpp
    include/nano/rect.hpp
    include/nano/vec.hpp
    include/nano/vertex.hpp
    include/nano/color.hpp
    include/nano/engine.hpp
    include/nano/event.hpp
    include/nano/transform2D.hpp
    include/nano/scene.hpp
    include/nano/postman.hpp

    # creating separate static lib from Dear ImGui occurs multiply definition on windows
    # in addition this method is recommended by Dear ImGui Developers
    src/imconfig.h

    ${imgui}/imgui.cpp
    ${imgui}/imgui_demo.cpp
    ${imgui}/imgui_draw.cpp
    ${imgui}/imgui_tables.cpp
    ${imgui}/imgui_widgets.cpp
    ${imgui}/imgui_internal.h
    ${imgui}/imstb_rectpack.h
    ${imgui}/imstb_textedit.h
    ${imgui}/imstb_truetype.h

    ${imgui}/backends/imgui_impl_opengl3.h
    ${imgui}/backends/imgui_impl_opengl3.cpp

    ${imgui}/backends/imgui_impl_sdl3.h
    ${imgui}/backends/imgui_impl_sdl3.cpp
)
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${TARGET} PUBLIC cxx_std_20)

option(NANO_DEBUG "Debug macro for libnano" ON)
if (NANO_DEBUG)
target_compile_options(${TARGET} PUBLIC -DNANO_DEBUG)
target_compile_options(${TARGET} PUBLIC -DDEBUG)
target_compile_options(${TARGET} PUBLIC -g)
endif()

if (ANDROID)
target_link_libraries(${TARGET} PUBLIC GLESv3 EGL glad SDL3::SDL3-shared SDL3_image::SDL3_image android log)
else()
find_package(OpenGL REQUIRED)
target_link_libraries(${TARGET} PUBLIC OpenGL::GL glad SDL3::SDL3-shared SDL3_image::SDL3_image)
endif()

target_link_libraries(${TARGET} PUBLIC sanitizer)

target_include_directories(${TARGET} PUBLIC ${imgui})
target_include_directories(${TARGET} PUBLIC ${imgui}/backends)
target_compile_definitions(${TARGET} PRIVATE -DIMGUI_USER_CONFIG="../../nano/src/imconfig.h")
