//---------------------------------------------------------------------------------
// Shader.h
//---------------------------------------------------------------------------------
// 
// Provides a simple wrapper for users to apply different Meshes
//

#pragma once

#include "Assets.h"


struct Shader
{
    ShaderAsset ShaderId;

    Shader() : ShaderId(DefaultShader) {}

    Shader(ShaderAsset ShaderId) : ShaderId(ShaderId) {}
};
