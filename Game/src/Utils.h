#pragma once
#include <string>
#include <unordered_set>

#include "MeshInstance.h"
#include "Entity.h"
#include "Texture.h"

namespace Utils
{
    /**
     * \brief  Code adapted from https://stackoverflow.com/questions/33571609/having-trouble-with-vector-erase-and-remove-if
     *         Modify to remove ranges instead of individual index
     * \tparam Type
     * \param ranges_to_erase integer ranges of ranges you want to erase
     * \param vec vector to erase ranges from
     */
    template <typename Type>
    void EraseRanges(const std::vector<std::pair<int, int>>& ranges_to_erase, std::vector<Type>& vec) {
        std::vector<bool> erase_index(vec.size(), false);

        for (const std::pair<int, int> pair : ranges_to_erase) {
            std::fill(erase_index.begin() + pair.first, erase_index.begin() + pair.second, true);
        }
        std::vector<bool>::const_iterator it_to_erase = erase_index.cbegin();

        typename std::vector<Type>::iterator it_erase_from = std::remove_if(
            vec.begin(), vec.end(),
            [&it_to_erase](const Type&) -> bool {
                return *it_to_erase++ == true;
            }
        );

        vec.erase(it_erase_from, vec.end());
    }


    /**
     * \brief Loads an .obj file only supports triangles, normals, texture coordinates 
     * \param filename 
     * \param mesh the object the function will populate the vertices with
     * \param textureList populates the list with textures reference in .obj file each vertex will contain a
     *                    index pointing to a texture on this list
     * \return 
     */
    bool LoadInstance(std::string filename, MeshInstance& mesh, std::vector<Texture>& textureList);

    /**
     * \brief Loads an .mtl material file support Ambient, Diffuse, Specular in the Bling-Phong Model
     *        meant for use by LoadInstance but can be called independently
     * \param filename 
     * \param textureList loaded list from texture
     * \param textureIDs map of texture names to index of texture in textureList
     * \return 
     */
    bool LoadMTLFile(const std::string& filename, std::vector<Texture>& textureList, std::unordered_map<std::string, size_t>& textureIDs);
}
