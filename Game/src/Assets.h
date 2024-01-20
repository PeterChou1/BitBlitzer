//---------------------------------------------------------------------------------
// Asset.h
//---------------------------------------------------------------------------------
// 
// Defines Enum Used by the AssetServer to locate where an asset is located
//
#pragma once

// Graphical Assets .obj files
enum ObjAsset
{
    SlingShot,
    Spot,
    Robot
};

// Shaders -> See SIMDShader.h for more details
enum ShaderAsset
{
    DefaultShader,
    BlinnPhong,
    ToonShader,
    UnlitShader
};
