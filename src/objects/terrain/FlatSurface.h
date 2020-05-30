//
// Created by igor on 23/04/2020.
//

#ifndef PGM_FLATSURFACE_H
#define PGM_FLATSURFACE_H

#include "objects/terrain/BaseTerrain.h"
#include "utils/Texture.h"

/**
 * Flat ground object for the scene.
 */
class FlatSurface : public BaseTerrain {
public:
    /**
     * Generates the terrain.
     * @param shaders
     */
    FlatSurface(Shaders &shaders);

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
    const Texture snowTexture;
};


#endif //PGM_FLATSURFACE_H