cmake_minimum_required(VERSION 3.27)
project(2025_I2P2_TowerDefense)

set(CMAKE_CXX_STANDARD 17)

# 設定 Allegro 路徑
set(ALLEGRO_ROOT "C:/allegro")

include_directories(.)
include_directories(Resource)

# 複製資源與 DLL
file(COPY Resource/audios DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(2025_I2P2_TowerDefense
    Bullet/Bullet.cpp Bullet/Bullet.hpp
    Bullet/FireBullet.cpp Bullet/FireBullet.hpp
    Bullet/LaserBullet.cpp Bullet/LaserBullet.hpp
    Bullet/FireNetBullet.cpp Bullet/FireNetBullet.hpp
    Bullet/OrbitBullet.hpp Bullet/OrbitBullet.cpp
    Bullet/BounceBullet.hpp Bullet/BounceBullet.cpp

    Enemy/Enemy.cpp Enemy/Enemy.hpp
    Enemy/SoldierEnemy.cpp Enemy/SoldierEnemy.hpp
    Enemy/TankEnemy.cpp Enemy/TankEnemy.hpp
    Enemy/PlaneEnemy.cpp Enemy/PlaneEnemy.hpp
    Enemy/BossEnemy.cpp Enemy/BossEnemy.hpp
    Enemy/ArmoredEnemy.cpp Enemy/ArmoredEnemy.hpp
    
    Enemy/ArmoredEnemy.cpp Enemy/ArmoredEnemy.hpp
    Engine/Allegro5Exception.cpp Engine/Allegro5Exception.hpp
    Engine/AudioHelper.cpp Engine/AudioHelper.hpp
    Engine/Collider.cpp Engine/Collider.hpp
    Engine/GameEngine.cpp Engine/GameEngine.hpp
    Engine/Group.cpp Engine/Group.hpp
    Engine/IControl.cpp Engine/IControl.hpp
    Engine/IObject.cpp Engine/IObject.hpp
    Engine/IScene.cpp Engine/IScene.hpp
    Engine/LOG.cpp Engine/LOG.hpp
    Engine/Point.cpp Engine/Point.hpp
    Engine/Resources.cpp Engine/Resources.hpp
    Engine/Sprite.cpp Engine/Sprite.hpp

    Scene/LoseScene.cpp Scene/LoseScene.hpp
    Scene/PlayScene.cpp Scene/PlayScene.hpp
    Scene/StageSelectScene.cpp Scene/StageSelectScene.hpp
    Scene/StartScene.cpp Scene/StartScene.h
    Scene/WinScene.cpp Scene/WinScene.hpp
    Scene/SettingsScene.cpp
    Scene/ScoreboardScene.cpp Scene/ScoreboardScene.hpp
    Scene/SubmitScene.cpp Scene/SubmitScene.hpp
    Scene/RemotePlayScene.cpp Scene/RemotePlayScene.hpp
    Scene/DrawMapScene.cpp Scene/DrawMapScene.hpp
    Scene/LoadingScene.cpp Scene/LoadingScene.hpp
    
    Turret/LaserTurret.cpp Turret/LaserTurret.hpp
    Turret/MachineGunTurret.cpp Turret/MachineGunTurret.hpp
    Turret/Turret.cpp Turret/Turret.hpp
    Turret/TurretButton.cpp Turret/TurretButton.hpp
    Turret/AbsorptionTurret.cpp Turret/AbsorptionTurret.hpp
    Turret/Shovel.cpp Turret/Shovel.hpp

    UI/Animation/DirtyEffect.cpp UI/Animation/DirtyEffect.hpp
    UI/Animation/ExplosionEffect.cpp UI/Animation/ExplosionEffect.hpp
    UI/Animation/Plane.cpp UI/Animation/Plane.hpp

    UI/Component/Image.cpp UI/Component/Image.hpp
    UI/Component/ImageButton.cpp UI/Component/ImageButton.hpp
    UI/Component/Label.cpp UI/Component/Label.hpp
    UI/Component/Slider.cpp UI/Component/Slider.hpp

    Connect/Client.cpp Connect/Client.hpp
    
    Player/Player.cpp Player/Player.hpp
    
    Map/Texture.cpp Map/Texture.hpp
    Map/TextureButton.cpp Map/TextureButton.hpp
    
    Camera/Camera.hpp Camera/Camera.cpp

    Weapon/Weapon.hpp Weapon/Weapon.cpp
    Weapon/GunWeapon.hpp Weapon/GunWeapon.cpp
    Weapon/CircleWeapon.hpp Weapon/CircleWeapon.cpp
    Weapon/ShotgunWeapon.hpp Weapon/ShotgunWeapon.cpp
    Weapon/BounceWeapon.hpp Weapon/BounceWeapon.cpp

    Camera/Camera.hpp Camera/Camera.cpp
    
    main.cpp
)

include(FetchContent)

# -----------------------
# ZLIB: find or download
# -----------------------
find_package(ZLIB QUIET)
if(NOT ZLIB_FOUND)
    message(STATUS "ZLIB not found, downloading zlib via FetchContent...")
    FetchContent_Declare(
        zlib
        URL https://zlib.net/zlib-1.3.1.tar.gz
    )
    FetchContent_MakeAvailable(zlib)
    if (TARGET zlibstatic)
        add_library(ZLIB::ZLIB ALIAS zlibstatic)
    elseif(TARGET zlib)
        add_library(ZLIB::ZLIB ALIAS zlib)
    endif()
endif()

# Link ZLIB
target_link_libraries(2025_I2P2_TowerDefense PRIVATE ZLIB::ZLIB)

# -----------------------
# JSON
# -----------------------
target_include_directories(2025_I2P2_TowerDefense PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/json-develop/json-develop/single_include
)

# -----------------------
# ALLEGRO
# -----------------------
if (WIN32)
    target_include_directories(2025_I2P2_TowerDefense PRIVATE ${ALLEGRO_ROOT}/include)
    target_link_directories(2025_I2P2_TowerDefense PRIVATE ${ALLEGRO_ROOT}/lib)
    target_link_libraries(2025_I2P2_TowerDefense PRIVATE allegro_monolith ws2_32)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(2025_I2P2_TowerDefense PRIVATE ${ALLEGRO5_INCLUDE_DIRS})
    target_link_directories(2025_I2P2_TowerDefense PRIVATE ${ALLEGRO5_LIBRARY_DIRS})
    target_link_libraries(2025_I2P2_TowerDefense PRIVATE ${ALLEGRO5_LIBRARIES})
endif()