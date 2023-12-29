#include "stdafx.h"
#include "Utils.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <unordered_map>


bool Utils::LoadFromObjectFile(std::string filename, Mesh& mesh, bool UV, bool Normal)
{

    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::vector<Vec3> temp_vertices;
    std::vector<Vec2> temp_uvs;
    std::vector<Vec3> temp_normals;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt" && UV) {
            Vec2 uv;
            ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn" && Normal) {
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            std::string vertex1, vertex2, vertex3;
            ss >> vertex1 >> vertex2 >> vertex3;

            // Process each vertex of the triangle
            Triangle triangle;
            for (int i = 0; i < 3; i++) {
                std::string vertex = (i == 0) ? vertex1 : (i == 1) ? vertex2 : vertex3;

                std::istringstream vertexStream(vertex);
                std::string vertexIndex, uvIndex, normalIndex;

                std::getline(vertexStream, vertexIndex, '/');
                if (UV) {
                    std::getline(vertexStream, uvIndex, '/');
                }
                if (Normal) {
                    if (!UV) { // Skip over the uv index if UVs are not being loaded
                        // vertexStream.ignore(std::numeric_limits<std::streamsize>::max(), '/');
                    }
                    std::getline(vertexStream, normalIndex, '/');
                }

                int vIndex = std::stoi(vertexIndex) - 1;
                triangle.verts[i].pos = temp_vertices[vIndex];

                if (UV && !uvIndex.empty()) {
                    int uvIdx = std::stoi(uvIndex) - 1;
                    triangle.verts[i].tex.x = temp_uvs[uvIdx].x;
                    triangle.verts[i].tex.y = std::abs(temp_uvs[uvIdx].y - 1);
                }

                if (Normal && !normalIndex.empty()) {
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

bool Utils::LoadInstance(std::string filename, MeshInstance& mesh, Entity id)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    std::vector<Vec3> temp_vertices;
    std::vector<Vec2> temp_uvs;
    std::vector<Vec3> temp_normals;
    std::unordered_map<Vertex, uint32_t> vertexMap;
    bool UV = false, Normal = false;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            UV = true;
            Vec2 uv;
            ss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn") {
            Normal = true;
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            std::string vertex1, vertex2, vertex3;
            ss >> vertex1 >> vertex2 >> vertex3;
            std::string varray[3] = { vertex1, vertex2, vertex3 };
            Vertex vertarray[3] = { Vertex(), Vertex(), Vertex() };

            for (int i = 0; i < 3; i++) {
                Vertex& v = vertarray[i];
                v.id = id;
                std::string vertex = varray[i];
                std::istringstream vertexStream(vertex);
                std::string vertexIndex, uvIndex, normalIndex;

                std::getline(vertexStream, vertexIndex, '/');
                if (UV) {
                    std::getline(vertexStream, uvIndex, '/');
                }
                if (Normal) {
                    if (!UV) { // Skip over the uv index if UVs are not being loaded
                        // vertexStream.ignore(std::numeric_limits<std::streamsize>::(max()), '/');
                    }
                    std::getline(vertexStream, normalIndex, '/');
                }

                int vIndex = std::stoi(vertexIndex) - 1;
                v.pos = temp_vertices[vIndex];

                if (UV && !uvIndex.empty()) {
                    int uvIdx = std::stoi(uvIndex) - 1;
                    v.tex.x = temp_uvs[uvIdx].x;
                    v.tex.y = std::abs(temp_uvs[uvIdx].y - 1);
                }

                if (Normal && !normalIndex.empty()) {
                    int nIdx = std::stoi(normalIndex) - 1;
                    v.normal = temp_normals[nIdx];
                }
            }

            if (!Normal) {
                Vec3 lineA = vertarray[0].pos - vertarray[1].pos;
                Vec3 lineB = vertarray[0].pos - vertarray[2].pos;
                Vec3 normal = lineB.Cross(lineA);
                normal.Normalize();
                vertarray[0].normal = normal;
                vertarray[1].normal = normal;
                vertarray[2].normal = normal;
            }

            for (int i = 0; i < 3; i++) {
                Vertex& v = vertarray[i];
                if (vertexMap.find(v) == vertexMap.end()) {
                    vertexMap[v] = mesh.vertices.size();
                    v.index = mesh.vertices.size();
                    mesh.vertices.push_back(v);
                    mesh.indices.push_back(vertexMap[v]);
                }
                else {
                    mesh.indices.push_back(vertexMap[v]);
                }
            }

        }

    }
    file.close();
    return false;
}
