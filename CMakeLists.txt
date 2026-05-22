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
        Shaders/cubemap_capture.vert
        Shaders/equirectangular_to_cubemap.frag
        Shaders/irradiance_convolution.frag
        Shaders/prefilter_environment.frag
        Shaders/brdf.vert
        Shaders/brdf.frag
        Shaders/skybox.vert
        Shaders/skybox.frag
        Shaders/shadow_depth.vert
        Shaders/shadow_depth.frag
        Shaders/point_shadow_depth.vert
        Shaders/point_shadow_depth.frag
        
        external/stb_image/stb_image.h external/stb_image/stb_image.cpp
        common/3dEntities/Meshes/planet.h common/3dEntities/Meshes/planet.cpp

        common/3dEntities/Lights/Light.hpp common/3dEntities/Lights/Light.cpp
        common/3dEntities/Lights/DirectionalLight.hpp common/3dEntities/Lights/DirectionalLight.cpp
        common/3dEntities/Lights/PointLight.hpp common/3dEntities/Lights/PointLight.cpp
        common/3dEntities/Lights/SpotLight.hpp common/3dEntities/Lights/SpotLight.cpp

        common/3dEntities/Mesh.cpp
        common/Physics/rigidbody3d.h common/Physics/rigidbody3d.cpp
        common/Physics/collider3d.h common/Physics/collider3d.cpp
        common/3dEntities/terrainmanager.h common/3dEntities/terrainmanager.cpp
        common/3dEntities/Meshes/lod.h common/3dEntities/Meshes/lod.cpp
        common/Physics/physicstypes.h
        common/Physics/physicssettings.h
        common/Physics/Collision/physicsgeometry.h common/Physics/Collision/physicsgeometry.cpp
        common/Physics/Collision/broadphase.h common/Physics/Collision/broadphase.cpp
        common/Physics/collisiondispatcher.h common/Physics/collisiondispatcher.cpp
        common/Physics/Collision/narrowphase.h common/Physics/Collision/narrowphase.cpp
        common/Physics/Collision/contactsolver.h common/Physics/Collision/contactsolver.cpp
        common/Physics/contactcache.h common/Physics/contactcache.cpp
        common/Physics/physicsworld.h common/Physics/physicsworld.cpp

        common/Render/Shader.hpp common/Render/Shader.cpp
        common/Render/RenderSystem.hpp common/Render/RenderSystem.cpp
        common/Render/ForwardRenderSystem.hpp common/Render/ForwardRenderSystem.cpp
        common/Render/IBLEnvironment.hpp common/Render/IBLEnvironment.cpp

        common/Utilities/rayintersection.h
        common/Utilities/PerlinNoise.h
        common/Utilities/Geometry.h

        common/Controls/charactercontroller.cpp common/Controls/charactercontroller.h
        common/Materials/terrainmaterial.h common/Materials/terrainmaterial.cpp
        common/scene.h common/scene.cpp

        common/Physics/Shapes/shape.h common/Physics/Shapes/shape.cpp
        common/Physics/Shapes/terrainshape.h common/Physics/Shapes/terrainshape.cpp
        common/Physics/Shapes/meshshape.h common/Physics/Shapes/meshshape.cpp
        common/Physics/Shapes/sphere.h common/Physics/Shapes/sphere.cpp
        common/Physics/Shapes/capsuleshape.h common/Physics/Shapes/capsuleshape.cpp
        common/Physics/Shapes/cube.h common/Physics/Shapes/cube.cpp

        Scenes/SolarSystem.h
        Scenes/InfiniteTerrain.h
        Scenes/PBRInfiniteTerrain.h
        Scenes/PBRGridScene.h
        Scenes/Ball.h
        Scenes/Cube.h
        Scenes/MainCharacter.h
        Scenes/HoveringCharacter.h
        Scenes/PhysicsCube.h
        common/Utilities/InputManager.h
        common/Controls/hoveringcontroller.h common/Controls/hoveringcontroller.cpp
        common/Physics/Shapes/voxelshape.h common/Physics/Shapes/voxelshape.cpp
        Scenes/FirstPersonCharacter.h
        Shaders/vertex_shader_line.glsl
        Shaders/fragment_shader_line.glsl
        common/Utilities/hash.h
        common/GUI/gui.h common/GUI/gui.cpp
        textrenderer.h






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
SOURCE_GROUP(common/3dEntities/Lights REGULAR_EXPRESSION ".*/common/3dEntities/Lights/.*" )
SOURCE_GROUP(common/Controls REGULAR_EXPRESSION ".*/common/Controls/.*" )
SOURCE_GROUP(common/Materials REGULAR_EXPRESSION ".*/common/Materials/.*" )
SOURCE_GROUP(common/Physics/Collision REGULAR_EXPRESSION ".*/common/Physics/Collision/.*" )
SOURCE_GROUP(common/Physics/Shapes REGULAR_EXPRESSION ".*/common/Physics/Shapes/.*" )
SOURCE_GROUP(common/Utilities REGULAR_EXPRESSION ".*/common/Utilities/.*" )
SOURCE_GROUP(common/Physics/ REGULAR_EXPRESSION ".*/common/Physics/.*" )
SOURCE_GROUP(common/Render/ REGULAR_EXPRESSION ".*/common/Render/.*" )
SOURCE_GROUP(common/GUI/ REGULAR_EXPRESSION ".*/common/GUI/.*" )
SOURCE_GROUP(scenes REGULAR_EXPRESSION ".*/Scenes/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


#if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
#add_custom_command(
#   TARGET Main POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Main${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/TP1/"
#)
#elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

#endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

