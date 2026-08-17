cmake_minimum_required(VERSION 3.20)
project(MySDLProject CXX)

set(CMAKE_CXX_STANDARD 17)

set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDL2")
include_directories(${SDL2_DIR}/include)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    link_directories(${SDL2_DIR}/lib/x64)
else()
    link_directories(${SDL2_DIR}/lib/x86)
endif()

set(SOURCES
        main.cpp
        Component.cpp
        Wire.cpp
        CircuitManager.cpp
        AdvancedUI.cpp
)

set(HEADERS
        Types.h
        Component.h
        Wire.h
        CircuitManager.h
        AdvancedUI.h
        ContextMenu.h
        ComponentFactory.h
        ComponentFactory.cpp
        MainMenu.h
        MainMenu.cpp
)

add_executable(MySDLProject ${SOURCES} ${HEADERS})

# افزودن SDL2_ttf به بخش target_link_libraries
if(MINGW)
    target_link_libraries(MySDLProject PRIVATE mingw32 SDL2main SDL2 SDL2_ttf)
else()
    target_link_libraries(MySDLProject PRIVATE SDL2main SDL2 SDL2_ttf)
endif()