//---------------------------------------------------------------------------------
// AssetServer.h
//---------------------------------------------------------------------------------
// 
// An AssetServer responsible for handling loading of all
// obj/textures/shader files in the game
//
#pragma once

#include <unordered_map>
#include <set>

#include "Assets.h"
#include "BlingPhong.h"
#include "Material.h"
#include "Utils.h"
#include "MeshInstance.h"
#include "ToonShaderSIMD.h"
#include "UnlitSIMD.h"

class AssetServer
{
protected:
    AssetServer() = default;

public:
    AssetServer(AssetServer& other) = delete;
    void operator=(const AssetServer&) = delete;

    /**
     * \brief The Asset Server is A Global Singleton that is instantiated only once
     *        GetInstance retrieves this Object
     * \return 
     */
    static AssetServer& GetInstance()
    {
        static AssetServer instance;
        return instance;
    }

    /**
     * \brief Clears the loaded geometry and loads geometry and associated
     *        textures from the AssetList
     * \param AssetList Specify which assets loaded
     */
    void LoadLevelAssets(const std::set<ObjAsset>& AssetList)
    {
        // clear previous loaded assets
        assets.clear();
        textureList.clear();
        for (auto objID : AssetList)
        {
            MeshInstance instance;
            const bool success = Utils::LoadInstance(LookUpFilePath(objID), instance, textureList);
            assert(success && "failed to load assets");
            assets[objID] = instance;
        }
    }

    /**
     * \brief Given a texture ID returns a material
     *        When a we load a vertex every Vertex is given a texID correspond
     *        to material it belongs to this method is used to retrieve it
     *        when we Fragment Shade
     * \param texID 
     * \return 
     */
    Material& GetMaterial(int texID)
    {
        assert(texID <= textureList.size() && "texID does not exist");

        if (texID == -1)
            return Material::DefaultMaterial;

        return textureList[texID];
    }

    /**
     * \brief Returns a Shader given a shaderID 
     * \param shaderID
     * \return
     */
    SIMDShader* GetShader(ShaderAsset shaderID)
    {
        return LookUpShader(shaderID);
    }

    /**
     * \brief Returns a Mesh Obj given its ID
     *        NOTE: this objID must be loaded with LoadLevelAssets
     *              or else it will throw an error
     * \param shaderID
     * \return
     */
    MeshInstance& GetObj(ObjAsset objID)
    {
        assert(assets.find(objID) != assets.end() && "Obj Not Loaded");
        return assets[objID];
    }

private:

    std::string LookUpFilePath(ObjAsset asset)
    {
        switch (asset)
        {
        case Spot:
            return "./Assets/spotWithTextures.obj";
        case Furina:
            return "./Assets/furina.obj";
        case Maze:
            return "./Assets/MazeTriangulated.obj";
        case Pinball:
            return "./Assets/NoNormalsPinball.obj";
        case Pacman:
            return "./Assets/PacmanModel.obj";
        }
        assert(false && "not possible");
    }

    SIMDShader* LookUpShader(ShaderAsset asset)
    {
        static BlinnPhongSIMD BlinnPhongShaderInstance = BlinnPhongSIMD();
        static UnlitSIMD UnlitShaderInstance = UnlitSIMD();
        static ToonShaderSIMD ToonShaderInstance = ToonShaderSIMD();

        switch (asset)
        {
        case DefaultShader:
            // The Default Shader is an Unlit Shader
            return &BlinnPhongShaderInstance;
        case BlinnPhong:
            return &BlinnPhongShaderInstance;
        case ToonShader:
            return &ToonShaderInstance;
        }

        assert(false && "not possible");
    }


    std::vector<Material> textureList;
    std::unordered_map<ObjAsset, MeshInstance> assets;

};