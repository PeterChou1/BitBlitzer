#include "stdafx.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>


bool Utils::LoadFromObjectFile(std::string filename, Mesh& mesh, bool UV,
                               bool Normal)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    std::vector<Vec3> temp_vertices;
    std::vector<Vec2> temp_uvs;
    std::vector<Vec3> temp_normals;

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v")
        {
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt" && UV)
        {
            Vec2 uv;
            ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn" && Normal)
        {
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f")
        {
            std::string vertex1, vertex2, vertex3;
            ss >> vertex1 >> vertex2 >> vertex3;

            // Process each vertex of the triangle
            Triangle triangle;
            for (int i = 0; i < 3; i++)
            {
                std::string vertex = (i == 0) ? vertex1 : (i == 1) ? vertex2 : vertex3;

                std::istringstream vertexStream(vertex);
                std::string vertexIndex, uvIndex, normalIndex;

                std::getline(vertexStream, vertexIndex, '/');
                if (UV)
                {
                    std::getline(vertexStream, uvIndex, '/');
                }
                if (Normal)
                {
                    if (!UV)
                    {
                        // Skip over the uv index if UVs are not being loaded
                        vertexStream.ignore(std::numeric_limits<std::streamsize>::max(), '/');
                    }
                    std::getline(vertexStream, normalIndex, '/');
                }

                int vIndex = std::stoi(vertexIndex) - 1;
                triangle.verts[i].pos = temp_vertices[vIndex];

                if (UV && !uvIndex.empty())
                {
                    int uvIdx = std::stoi(uvIndex) - 1;
                    triangle.verts[i].tex.x = temp_uvs[uvIdx].x;
                    triangle.verts[i].tex.y = std::abs(temp_uvs[uvIdx].y - 1);
                }

                if (Normal && !normalIndex.empty())
                {
                    int nIdx = std::stoi(normalIndex) - 1;
                    triangle.verts[i].normal = temp_normals[nIdx];
                }
            }

            mesh.tris.push_back(triangle);
        }
    }

    file.close();
    return true;
}


bool
Utils::LoadInstance(std::string filename,
                    MeshInstance& mesh,
                    TextureList& texList)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }
    std::vector<Vec3> temp_vertices;
    std::vector<Vec2> temp_uvs;
    std::vector<Vec3> temp_normals;
    std::unordered_map<Vertex, uint32_t> vertexMap;
    bool UV = false, Normal = false;
    size_t cur_texID = -1;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;
        if (prefix == "mtllib")
        {
            std::string mtlfilename;
            ss >> mtlfilename;
            size_t lastSlash = filename.find_last_of("/\\");
            std::string directory = filename.substr(0, lastSlash + 1);
            std::string mtlfilepath = directory + mtlfilename;
            // fail to load mtl return
            if (!LoadMTLFile(mtlfilepath, texList)) return false;
        }
        else if (prefix == "usemtl")
        {
            std::string matname;
            ss >> matname;
            // fail to find material in mtl file
            if (texList.textureIDs.find(matname) == texList.textureIDs.end())
                return false;

            cur_texID = texList.textureIDs[matname];
        }
        else if (prefix == "v")
        {
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt")
        {
            UV = true;
            Vec2 uv;
            ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn")
        {
            Normal = true;
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f")
        {
            std::string vertex1, vertex2, vertex3;
            ss >> vertex1 >> vertex2 >> vertex3;
            std::string varray[3] = {vertex1, vertex2, vertex3};
            Vertex vertarray[3] = {Vertex(), Vertex(), Vertex()};

            for (int i = 0; i < 3; i++)
            {
                Vertex& v = vertarray[i];
                // v.id = id;
                std::string vertex = varray[i];
                std::istringstream vertexStream(vertex);
                std::string vertexIndex, uvIndex, normalIndex;

                std::getline(vertexStream, vertexIndex, '/');
                if (UV)
                {
                    std::getline(vertexStream, uvIndex, '/');
                }
                if (Normal)
                {
                    if (!UV)
                    {
                        // Skip over the uv index if UVs are not being loaded
                        vertexStream.ignore(std::numeric_limits<std::streamsize>::max(), '/');
                    }
                    std::getline(vertexStream, normalIndex, '/');
                }

                int vIndex = std::stoi(vertexIndex) - 1;
                v.pos = temp_vertices[vIndex];

                if (UV && !uvIndex.empty())
                {
                    int uvIdx = std::stoi(uvIndex) - 1;
                    v.tex.x = temp_uvs[uvIdx].x;
                    v.tex.y = std::abs(temp_uvs[uvIdx].y - 1);
                }

                if (Normal && !normalIndex.empty())
                {
                    int nIdx = std::stoi(normalIndex) - 1;
                    v.normal = temp_normals[nIdx];
                }
            }

            if (!Normal)
            {
                Vec3 lineA = vertarray[0].pos - vertarray[1].pos;
                Vec3 lineB = vertarray[0].pos - vertarray[2].pos;
                Vec3 normal = lineA.Cross(lineB);
                normal.Normalize();
                vertarray[0].normal = normal;
                vertarray[1].normal = normal;
                vertarray[2].normal = normal;
            }

            for (int i = 0; i < 3; i++)
            {
                Vertex& v = vertarray[i];
                v.tex_id = cur_texID;

                if (vertexMap.find(v) == vertexMap.end())
                {
                    vertexMap[v] = mesh.vertices.size();
                    v.index = mesh.vertices.size();
                    mesh.vertices.push_back(v);
                    mesh.indices.push_back(vertexMap[v]);
                }
                else
                {
                    mesh.indices.push_back(vertexMap[v]);
                }
            }
        }
    }
    file.close();
    return true;
}

bool
Utils::LoadMTLFile(const std::string& filename, TextureList& texList)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    Vec3 ambient, diffuse, specular;
    float highlight;
    std::string textureFilename;
    std::string textureName;
    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "newmtl") {
            // Handle new material - save the previous material if it exists
            if (!textureFilename.empty() && !textureName.empty()) {
                texList.textureIDs[textureName] = 
                    texList.textureList.size();
                texList.textureList.emplace_back(textureFilename.c_str(),
                                                 ambient,
                                                 diffuse,
                                                 specular,
                                                 highlight);
                textureFilename.clear();
            }
            iss >> textureName;
        } else if (keyword == "Ka") { // Ambient color
            iss >> ambient.x >> ambient.y >> ambient.z;
        } else if (keyword == "Kd") { // Diffuse color
            iss >> diffuse.x >> diffuse.y >> diffuse.z;
        } else if (keyword == "Ks") { // Specular color
            iss >> specular.x >> specular.y >> specular.z;
        } else if (keyword == "Ns") { // Specular highlight, exponent
            iss >> highlight;
        } else if (keyword == "map_Kd") { // Diffuse texture map
            iss >> textureFilename;
        }
        // ... Handle other properties as needed ...
    }

    // Handle the last material
    if (!textureFilename.empty()) {
        texList.textureIDs[textureName] = texList.textureList.size();
        texList.textureList.emplace_back(
          textureFilename.c_str(), ambient, diffuse, specular, highlight);
    }

    file.close();
    return true;

}
