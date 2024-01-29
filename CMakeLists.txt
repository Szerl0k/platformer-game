cmake_minimum_required(VERSION 3.26)
project(GraPlatformowa)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 10.1.1
)
FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.0
)
FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(sfml)
add_executable(GraPlatformowa main.cpp
        Global.hpp
        Satyr.cpp
        Satyr.hpp
        MapManager.cpp
        MapManager.hpp
        GlobalMethods.cpp
        GlobalMethods.hpp
        Enemy.cpp
        Enemy.hpp
        Projectile.cpp
        Projectile.hpp
        Character.cpp
        Character.hpp
)
target_link_libraries(
        GraPlatformowa
        fmt::fmt
        sfml-graphics
        sfml-window
        sfml-system
)
IF (WIN32)
    add_custom_command(TARGET GraPlatformowa POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:GraPlatformowa> $<TARGET_FILE_DIR:GraPlatformowa>
            COMMAND_EXPAND_LISTS
    )
ENDIF ()