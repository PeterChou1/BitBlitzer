set(PROJECT_NAME Game)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Readme.md"
    "ReadMe.txt"
    "scratch.txt"
)
source_group("" FILES ${no_group_source_files})

set(NextAPI
    "App/app.cpp"
    "App/app.h"
    "App/AppSettings.h"
    "App/main.cpp"
    "App/main.h"
    "App/SimpleController.cpp"
    "App/SimpleController.h"
    "App/SimpleSound.cpp"
    "App/SimpleSound.h"
    "App/SimpleSprite.cpp"
    "App/SimpleSprite.h"
    "src/stdafx.h"
    "src/targetver.h"
)
source_group("NextAPI" FILES ${NextAPI})

set(src
    "src/Game.cpp"
    "src/stdafx.cpp"
)
source_group("src" FILES ${src})

set(src__Components
    "src/Camera.cpp"
    "src/Camera.h"
    "src/Mesh.h"
    "src/Transform.cpp"
    "src/Transform.h"
)
source_group("src\\Components" FILES ${src__Components})

set(src__ECS
    "src/ComponentBuffer.h"
    "src/Entity.h"
    "src/TypeID.h"
    "src/Visitor.h"
)
source_group("src\\ECS" FILES ${src__ECS})

set(src__Graphics
    "src/Assets.h"
    "src/AssetServer.h"
    "src/MeshInstance.h"
    "src/Point.h"
    "src/Texture.cpp"
    "src/Texture.h"
    "src/Triangle.cpp"
    "src/Triangle.h"
    "src/Vertex.h"
)
source_group("src\\Graphics" FILES ${src__Graphics})

set(src__Manager
    "src/ComponentManager.h"
    "src/ECSManager.h"
    "src/EntityManager.h"
    "src/GameManager.cpp"
    "src/GameManager.h"
    "src/VisitorManager.h"
)
source_group("src\\Manager" FILES ${src__Manager})

set(src__Math
    "src/Mat2.cpp"
    "src/Mat2.h"
    "src/Mat3.cpp"
    "src/Mat3.h"
    "src/Mat4.cpp"
    "src/Mat4.h"
    "src/Quat.cpp"
    "src/Quat.h"
    "src/Vec2.cpp"
    "src/Vec2.h"
    "src/Vec3.cpp"
    "src/Vec3.h"
    "src/Vec4.cpp"
    "src/Vec4.h"
)
source_group("src\\Math" FILES ${src__Math})

set(src__Renderer
    "src/Clipper.cpp"
    "src/Clipper.h"
    "src/ColorBuffer.h"
    "src/DepthBuffer.h"
    "src/Lights.h"
    "src/Rasterizer.cpp"
    "src/Rasterizer.h"
    "src/Renderer.cpp"
    "src/Renderer.h"
    "src/Tile.cpp"
    "src/Tile.h"
)
source_group("src\\Renderer" FILES ${src__Renderer})

set(src__SIMD
    "src/SIMD.h"
    "src/SIMDDepthBuffer.h"
    "src/SIMDPixel.cpp"
    "src/SIMDPixel.h"
    "src/SIMDPixelBuffer.h"
    "src/SIMDTriangle.cpp"
    "src/SIMDTriangle.h"
)
source_group("src\\SIMD" FILES ${src__SIMD})

set(src__Scene
    "src/Scene.cpp"
    "src/Scene.h"
)
source_group("src\\Scene" FILES ${src__Scene})

set(src__Shader
    "src/BlingPhong.cpp"
    "src/BlingPhong.h"
    "src/SIMDShader.h"
    "src/Unlit.h"
)
source_group("src\\Shader" FILES ${src__Shader})

set(src__Systems
    "src/DebugCamera.cpp"
    "src/DebugCamera.h"
    "src/DebugMesh.cpp"
    "src/DebugMesh.h"
)
source_group("src\\Systems" FILES ${src__Systems})

set(src__Utils
    "src/Concurrent.h"
    "src/Utils.cpp"
    "src/Utils.h"
)
source_group("src\\Utils" FILES ${src__Utils})

set(stb_image
    "stb_image/stb_image.cpp"
    "stb_image/stb_image.h"
)
source_group("stb_image" FILES ${stb_image})

set(ALL_FILES
    ${no_group_source_files}
    ${NextAPI}
    ${src}
    ${src__Components}
    ${src__ECS}
    ${src__Graphics}
    ${src__Manager}
    ${src__Math}
    ${src__Renderer}
    ${src__SIMD}
    ${src__Scene}
    ${src__Shader}
    ${src__Systems}
    ${src__Utils}
    ${stb_image}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Game)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CRT_SECURE_NO_WARNINGS;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CRT_SECURE_NO_WARNINGS;"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /std:c++17;
                /O2;
                /Oi;
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /ZI
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /SAFESEH:NO;
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> xcopy $<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\Game\\glut\\bin\\x64\\*.dll $ENV{OutputPath} /Y
        COMMAND $<CONFIG:Release> xcopy $<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\Game\\glut\\bin\\x64\\*.dll $ENV{OutputPath} /Y
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> xcopy $<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\Game\\glut\\bin\\*.dll $ENV{OutputPath} /Y
        COMMAND $<CONFIG:Release> xcopy $<SHELL_PATH:${CMAKE_SOURCE_DIR}/>\\Game\\glut\\bin\\*.dll $ENV{OutputPath} /Y
    )
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Release>:"
            "kernel32;"
            "user32;"
            "gdi32;"
            "winspool;"
            "comdlg32;"
            "advapi32;"
            "shell32;"
            "ole32;"
            "oleaut32;"
            "uuid;"
            "odbc32;"
            "odbccp32"
        ">"
        "dxguid;"
        "dsound"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "dxguid;"
        "dsound;"
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "advapi32;"
        "shell32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

