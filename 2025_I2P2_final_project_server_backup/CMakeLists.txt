cmake_minimum_required(VERSION 3.27)
project(2025_I2P2_TowerDefense)

set(CMAKE_CXX_STANDARD 17)

# 設定 Allegro 路徑
set(ALLEGRO_ROOT "C:/allegro")

include_directories(.)
include_directories(Resource)


file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(2025_I2P2_TowerDefense
    
    #Connecting
    Connect/RenderSender.hpp
    Connect/RenderSender.cpp
    
    #Enemy
    Enemy/Enemy.cpp
    Enemy/Enemy.hpp
    main.cpp
)

# zlib
include(FetchContent)
find_package(ZLIB QUIET)
if(NOT ZLIB_FOUND)
    message(STATUS "ZLIB not found, downloading zlib via FetchContent...")
    FetchContent_Declare(
        zlib
        URL https://zlib.net/zlib-1.3.1.tar.gz
    )
    FetchContent_MakeAvailable(zlib)

    # 手動定義 target 別名
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()
target_link_libraries(2025_I2P2_TowerDefense PRIVATE ZLIB::ZLIB)



# json
target_include_directories(2025_I2P2_TowerDefense PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/json-develop/json-develop/single_include
)

if (WIN32)
    target_include_directories(2025_I2P2_TowerDefense PUBLIC ${ALLEGRO_ROOT}/include)
    target_link_directories(2025_I2P2_TowerDefense PUBLIC ${ALLEGRO_ROOT}/lib)
    target_link_libraries(2025_I2P2_TowerDefense PRIVATE allegro_monolith ws2_32)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(2025_I2P2_TowerDefense PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_link_directories(2025_I2P2_TowerDefense PUBLIC ${ALLEGRO5_LIBRARY_DIRS})
    target_link_libraries(2025_I2P2_TowerDefense ${ALLEGRO5_LIBRARIES})
endif()