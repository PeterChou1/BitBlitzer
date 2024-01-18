#include "stdafx.h"

#include <stack>

#include "Scene.h"
#include "Camera.h"
#include "AssetServer.h"
#include "CubeMap.h"
#include "ECSManager.h"
#include "Mesh.h"
#include "RigidBody.h"


extern ECSManager ECS;


struct Cell
{
    int x{}, z{};
    bool visited{};
    Cell() = default;
    Cell(int x, int z) : x(x), z(z) {}
};

void CreateMaze(int width, int height, float blockSize, const ObjAsset& wallAsset)
{
    srand(time(NULL)); // Initialize random seed

    // Initialize all cells as walls and unvisited
    std::vector<std::vector<Cell>> grid(width, std::vector<Cell>(height));
    for (int x = 0; x < width; x++) {
        for (int z = 0; z < height; z++) {
            grid[x][z] = Cell(x, z);
        }
    }

    // Start the maze generation from a random cell
    int startX = rand() % width;
    int startZ = rand() % height;
    Cell startCell(startX, startZ);
    startCell.visited = true;
    std::stack<Cell> stack;
    stack.push(startCell);

    while (!stack.empty()) {
        Cell currentCell = stack.top();
        std::vector<Cell*> neighbors;

        // Check for unvisited neighbors
        if (currentCell.x > 1 && !grid[currentCell.x - 2][currentCell.z].visited)
            neighbors.push_back(&grid[currentCell.x - 2][currentCell.z]);
        if (currentCell.x < width - 2 && !grid[currentCell.x + 2][currentCell.z].visited)
            neighbors.push_back(&grid[currentCell.x + 2][currentCell.z]);
        if (currentCell.z > 1 && !grid[currentCell.x][currentCell.z - 2].visited)
            neighbors.push_back(&grid[currentCell.x][currentCell.z - 2]);
        if (currentCell.z < height - 2 && !grid[currentCell.x][currentCell.z + 2].visited)
            neighbors.push_back(&grid[currentCell.x][currentCell.z + 2]);

        if (!neighbors.empty()) {
            // Choose a random unvisited neighbor
            Cell* nextCell = neighbors[rand() % neighbors.size()];
            nextCell->visited = true;

            // Remove the wall between the current cell and the chosen cell
            int wallX = (currentCell.x + nextCell->x) / 2;
            int wallZ = (currentCell.z + nextCell->z) / 2;
            grid[wallX][wallZ].visited = true;

            stack.push(*nextCell);
        }
        else {
            stack.pop();
        }
    }

    // Create the walls in the maze
    for (int x = 0; x < width; x++) {
        for (int z = 0; z < height; z++) {
            if (!grid[x][z].visited) {
                Entity wallEntity = ECS.CreateEntity();
                auto wallTransform = Transform(Vec3(x * blockSize, 0, z * blockSize), Quat(Vec3(0, 1, 0), 0.0));
                ECS.AddComponent<Transform>(wallEntity, wallTransform);
                ECS.AddComponent<Mesh>(wallEntity, Mesh(wallAsset));
            }
        }
    }
}


void Scene::Setup()
{
    // Loaded all asset that the current scene needs
    std::set<ObjAsset> levelAssets({ Box });
    AssetServer& loader = AssetServer::GetInstance();
    loader.LoadLevelAssets(levelAssets);

    auto camera = ECS.GetResource<Camera>();
    // Setup camera
    camera->SetPosition(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f));

    //for (int x = 0; x < 6; x++)
    //{
    //    for (int z = 0; z < 5; z++)
    //    {
    //        Entity meshEntity = ECS.CreateEntity();
    //        auto modelTransform = Transform(Vec3(x * 2, 0, 5 + z * 2), Quat(Vec3(1, 0, 0), 0.0));
    //        ECS.AddComponent<Transform>(meshEntity, modelTransform);
    //        ECS.AddComponent<Mesh>(meshEntity, Mesh(Spot));
    //    }
    //}

    CreateMaze(10, 10, 2, Box);

}


void Scene::Update(float deltaTime)
{

}
