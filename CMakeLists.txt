cmake_minimum_required(VERSION 3.21)
project(editor LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(editor 
src/main.cpp 
src/button.cpp 
src/xml_parser.cpp 
src/entry.cpp 
src/component.cpp 
src/utils.tpp
src/scrollbar.cpp
src/application.cpp
src/section.cpp
src/events.cpp
)
target_link_libraries(editor PRIVATE sfml-graphics)
target_compile_features(editor PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:editor> $<TARGET_FILE_DIR:editor> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS editor)
