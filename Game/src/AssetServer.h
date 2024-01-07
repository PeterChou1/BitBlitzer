#pragma once
#include <unordered_map>
#include <set>
#include "Assets.h"
#include "Texture.h"
#include "Utils.h"
#include "MeshInstance.h"

/*
 * /brief An AssetServer is responsible for handling loading of all obj/textures
 *          files in the game
 */
class AssetServer
{
protected:
    AssetServer() = default;

public:
    AssetServer(AssetServer& other) = delete;
    void operator=(const AssetServer&) = delete;

    static AssetServer& GetInstance()
    {
        static AssetServer instance;
        return instance;
    }

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

    Texture& GetTexture(unsigned int texID)
    {
        assert(texID <= textureList.size() && "texID does not exist");
        return textureList[texID];
    }

    MeshInstance& GetObj(ObjAsset objID)
    {
        assert(assets.find(objID) != assets.end() && "Obj Not Loaded");
        return assets[objID];
    }

private:
    std::vector<Texture> textureList;
    std::unordered_map<ObjAsset, MeshInstance> assets;

};