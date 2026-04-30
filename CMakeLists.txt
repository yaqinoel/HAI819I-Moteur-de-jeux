# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Moteur)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(embree 4 REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
        message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
        message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compile external dependencies
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
        add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)

include_directories(
        external/glfw-3.1.2/include/
        external/glm-0.9.7.1/
        external/glew-1.13.0/include/
        external/assimp/include/
        ${CMAKE_BINARY_DIR}/external/assimp/include/
        .
)

include_directories(${EMBREE_INCLUDE_DIRS})

set(ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        GLEW_1130
        embree
        assimp
)

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)

# Main
add_executable(Main
        common/Main.cpp
        common/shader.cpp
        common/shader.hpp
        common/Edge.hpp
        common/node.h common/node.cpp
        
        common/Materials/material.h common/Materials/material.cpp
        common/Materials/texture.h common/Materials/texture.cpp
        common/Materials/pbrmaterial.h common/Materials/pbrmaterial.cpp


        common/Controls/cameracontrols.cpp common/Controls/cameracontrols.h
        common/Controls/freecamera.cpp common/Controls/freecamera.h

        common/3dEntities/node3d.h common/3dEntities/node3d.cpp
        common/3dEntities/camera.h common/3dEntities/camera.cpp
        common/3dEntities/Mesh.hpp
        common/3dEntities/Model.hpp common/3dEntities/Model.cpp

        common/3dEntities/Meshes/Terrain/proceduralterrain.h common/3dEntities/Meshes/Terrain/proceduralterrain.cpp
        common/3dEntities/Meshes/Terrain/heightmapterrain.h common/3dEntities/Meshes/Terrain/heightmapterrain.cpp
        common/3dEntities/Meshes/Terrain/terrain.h common/3dEntities/Meshes/Terrain/terrain.cpp
        common/3dEntities/Meshes/Terrain/proceduralvoxelterrain.h common/3dEntities/Meshes/Terrain/proceduralvoxelterrain.cpp

        Shaders/vertex_shader.glsl
        Shaders/fragment_shader.glsl
        Shaders/fragment_shader_shape.glsl
        Shaders/fragment_shader_lit.glsl
        Shaders/fragment_shader_Terrain_HeightMap.glsl
        Shaders/fragment_shader_Terrain_Noise.glsl
        
        external/stb_image/stb_image.h external/stb_image/stb_image.cpp
        common/3dEntities/Meshes/planet.h common/3dEntities/Meshes/planet.cpp

        common/3dEntities/Mesh.cpp
        common/3dEntities/rigidbody3d.h common/3dEntities/rigidbody3d.cpp
        common/3dEntities/collisionshape3d.h common/3dEntities/collisionshape3d.cpp
        common/3dEntities/terrainmanager.h common/3dEntities/terrainmanager.cpp
        common/3dEntities/Meshes/lod.h common/3dEntities/Meshes/lod.cpp
        common/Utilities/rayintersection.h
        common/Utilities/PerlinNoise.h
        common/Utilities/Geometry.h

        common/Controls/charactercontroller.cpp common/Controls/charactercontroller.h
        common/Materials/terrainmaterial.h common/Materials/terrainmaterial.cpp
        common/scene.h common/scene.cpp

        common/Shapes/shape.h common/Shapes/shape.cpp
        common/Shapes/terrainshape.h common/Shapes/terrainshape.cpp
        common/Shapes/meshshape.h common/Shapes/meshshape.cpp
        common/Shapes/sphere.h common/Shapes/sphere.cpp
        common/Shapes/cube.h common/Shapes/cube.cpp

        Scenes/SolarSystem.h
        Scenes/InfiniteTerrain.h
        Scenes/PBRGridScene.h
        Scenes/Ball.h
        Scenes/Cube.h
        Scenes/MainCharacter.h
        Scenes/HoveringCharacter.h
        common/Utilities/colliderIntersection.h
        Scenes/PhysicsCube.h
        common/Utilities/InputManager.h
        common/Controls/hoveringcontroller.h common/Controls/hoveringcontroller.cpp
        common/Constraint/constraint.h
        common/Constraint/contactconstraint.h common/Constraint/contactconstraint.cpp
        common/Shapes/voxelshape.h common/Shapes/voxelshape.cpp
        Scenes/FirstPersonCharacter.h



)
target_link_libraries(Main
        ${ALL_LIBS}
        embree
)
# Xcode and Visual working directories
set_target_properties(Main PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Main/")
# create_target_launcher uses deprecated LOCATION property, skip it
# create_target_launcher(Main WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Main/")




SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(common/3dEntities REGULAR_EXPRESSION ".*/common/3dEntities/.*" )
SOURCE_GROUP(common/3dEntities/Meshes/ REGULAR_EXPRESSION ".*/common/3dEntities/Meshes/.*" )
SOURCE_GROUP(common/3dEntities/Meshes/Terrain REGULAR_EXPRESSION ".*/common/3dEntities/Meshes/Terrain.*" )
SOURCE_GROUP(common/Controls REGULAR_EXPRESSION ".*/common/Controls/.*" )
SOURCE_GROUP(common/Materials REGULAR_EXPRESSION ".*/common/Materials/.*" )
SOURCE_GROUP(common/Shapes REGULAR_EXPRESSION ".*/common/Shapes/.*" )
SOURCE_GROUP(common/Utilities REGULAR_EXPRESSION ".*/common/Utilities/.*" )
SOURCE_GROUP(common/Constraint REGULAR_EXPRESSION ".*/common/Constraint/.*" )
SOURCE_GROUP(scenes REGULAR_EXPRESSION ".*/Scenes/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


#if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
#add_custom_command(
#   TARGET Main POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Main${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/TP1/"
#)
#elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

#endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

