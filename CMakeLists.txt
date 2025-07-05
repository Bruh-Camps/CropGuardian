cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "crop-guardian")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
include_directories(${SDL2_TTF_INCLUDE_DIRS})
link_directories(${SDL2_TTF_LIBRARY_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})
link_directories(${SDL2_MIXER_LIBRARY_DIRS})

add_executable(${PROJECT_NAME}
        Source/Main.cpp
        Source/Math.cpp
        Source/Random.cpp
        Source/Actors/Actor.cpp
        Source/Components/Component.cpp
        Source/Game.cpp
        Source/Game.h
        Source/Components/DrawComponents/DrawComponent.cpp
        Source/Components/DrawComponents/DrawComponent.h
        Source/Components/RigidBodyComponent.cpp
        Source/Components/RigidBodyComponent.h
        Source/Components/ColliderComponents/CircleColliderComponent.cpp
        Source/Components/ColliderComponents/CircleColliderComponent.h
        Source/Json.h
        Source/CSV.h
        Source/Components/DrawComponents/DrawSpriteComponent.cpp
        Source/Components/DrawComponents/DrawSpriteComponent.h
        Source/Components/DrawComponents/DrawPolygonComponent.cpp
        Source/Components/DrawComponents/DrawPolygonComponent.h
        Source/Components/ColliderComponents/AABBColliderComponent.cpp
        Source/Components/ColliderComponents/AABBColliderComponent.h
        Source/Components/DrawComponents/DrawAnimatedComponent.cpp
        Source/Components/DrawComponents/DrawAnimatedComponent.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/AudioSystem.cpp
        Source/AudioSystem.h
        Source/UIElements/UIScreen.h
        Source/UIElements/UIScreen.cpp
        Source/UIElements/UIFont.h
        Source/UIElements/UIFont.cpp
        Source/UIElements/UIButton.cpp
        Source/UIElements/UIButton.h
        Source/UIElements/UIImageButton.cpp
        Source/UIElements/UIImageButton.h
        Source/UIElements/UIText.cpp
        Source/UIElements/UIText.h
        Source/UIElements/UIElement.cpp
        Source/UIElements/UIElement.h
        Source/MainHUD.cpp
        Source/MainHUD.h
        Source/BuildTowerHUD.cpp
        Source/BuildTowerHUD.h
        Source/UIElements/UIImage.cpp
        Source/UIElements/UIImage.h
        Source/SpatialHashing.cpp
        Source/SpatialHashing.h
        Source/Actors/Enemy/Slime.cpp
        Source/Actors/Enemy/Slime.h
        Source/Actors/Enemy/Bee.cpp
        Source/Actors/Enemy/Bee.h
        Source/Actors/Enemy/Enemy.cpp
        Source/Actors/Enemy/Enemy.h
        Source/Actors/BuildSpot.cpp
        Source/Actors/BuildSpot.h
        Source/Actors/EnemyPortal.h
        Source/Actors/EnemyPortal.cpp
        Source/Actors/Base.cpp
        Source/Actors/Base.h
        Source/Actors/Tower/NormalTower.cpp
        Source/Actors/Tower/NormalTower.h
        Source/Actors/Tower/Projectile.cpp
        Source/Actors/Tower/Projectile.h
        Source/Actors/Tower/IceTower.cpp
        Source/Actors/Tower/IceTower.h
        Source/Actors/Tower/Tower.cpp
        Source/Actors/Tower/Tower.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
