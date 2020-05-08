//
// Created by Aloysio Galvão Lopes on 2020-05-08.
//

#ifndef PGM_MOUNTAINTERRAIN_H
#define PGM_MOUNTAINTERRAIN_H

#include <src/shaders/Shaders.h>
#include "BaseTerrain.h"

/**
 * Generates procedurally a terrain with some
 * predefined relief
 */

class MountainTerrain : public BaseTerrain {
public:
    /**
     * Generates the relief mesh
     * @param shaders
     */
    MountainTerrain(Shaders &shaders, float xSize, float ySize);

    /**
     * Draws the surface in opengl;
     * @param camera
     */
    void drawMesh(vcl::camera_scene &camera) override;

    /**
     * Sets a new light to the terrain.
     * @param light
     * @param idx
     */
    void setLight(std::shared_ptr<vcl::light_source> &light, int idx) override;

private:

    /**
     * Generates the mesh to be drawn
     */
    void evaluate_mesh();

    /**
     * Maps a surface point in u, v in (0, 1) to a 3D
     * point
     * @param u
     * @param v
     * @return 3D point
     */
    vcl::vec3 evaluate_terrain(float u, float v);

    vcl::mesh terrainMesh;
    float xSize;
    float ySize;
};


#endif //PGM_MOUNTAINTERRAIN_H
