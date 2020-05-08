//
// Created by Aloysio Galvão Lopes on 2020-05-08.
//

#include "MountainTerrain.h"

MountainTerrain::MountainTerrain(Shaders &shaders, float xSize, float ySize) {
    // Initializing internal variables
    light2 = std::make_shared<vcl::light_source>();
    light3 = std::make_shared<vcl::light_source>();
    this->xSize = xSize;
    this->ySize = ySize;

    // Creating mesh
    evaluate_mesh();

    // Setting bounding sphere
    boundingSphere = BoundingSphere(terrainMesh);

    //Setting shader
    terrain.shader = shaders["terrain_mesh"];
}

void MountainTerrain::drawMesh(vcl::camera_scene &camera) {
    terrain.uniform.light = light;
    terrain.uniform.light2 = light2;
    terrain.uniform.light3 = light3;
    vcl::draw(terrain, camera);
}

void MountainTerrain::setLight(std::shared_ptr<vcl::light_source> &light, int idx) {
    switch (idx) {
        case 1:
            this->light = light;
            break;
        case 2:
            this->light2 = light;
            break;
        default:
            this->light3 = light;
            break;
    }
    terrain.uniform.current_light = idx;
}

vcl::vec3 MountainTerrain::evaluate_terrain(const float u, const float v){
    const float x = xSize*(u-0.5f);
    const float y = ySize*(v-0.5f);
    const float z = 0;

    return {x, y, z};
}

void MountainTerrain::evaluate_mesh() {
    // TODO change to variable size
    size_t uDimensionSize = 100;
    size_t vDimensionSize = 100;

    terrainMesh.position.resize(uDimensionSize*vDimensionSize);

    // Fill terrain geometry
    for(size_t ku=0; ku<uDimensionSize; ++ku)
    {
        for(size_t kv=0; kv<vDimensionSize; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/((float)uDimensionSize-1.0f);
            const float v = kv/((float)vDimensionSize-1.0f);

            // Compute coordinates
            terrainMesh.position[kv+uDimensionSize*ku] = evaluate_terrain(u, v);
        }
    }

    // Generate triangle organization
    // Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(unsigned int ku=0; ku<(unsigned int) uDimensionSize-1; ++ku)
    {
        for(unsigned int kv=0; kv<(unsigned int) vDimensionSize-1; ++kv)
        {
            const unsigned int idx = kv + ((unsigned int)uDimensionSize)*ku; // current vertex offset

            const vcl::uint3 triangle_1 = {idx, idx+1+(unsigned int)vDimensionSize, idx+1};
            const vcl::uint3 triangle_2 = {idx, idx+(unsigned int)vDimensionSize, idx+1+(unsigned int)vDimensionSize};

            terrainMesh.connectivity.push_back(triangle_1);
            terrainMesh.connectivity.push_back(triangle_2);
        }
    }

    terrain = vcl::mesh_drawable(terrainMesh);
}
