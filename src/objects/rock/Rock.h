//
// Created by igor on 25/04/2020.
//

#ifndef PGM_ROCK_H
#define PGM_ROCK_H

#include "vcl.hpp"
#include "objects/Object.h"
#include "utils/Texture.h"

/**
 * Rock object.
 */
class Rock : public Object {
public:
    /**
     * Generates the rock.
     * @param shaders
     * @param base Position of the center of the rock's base.
     * @param snowCoverage Float from 0 to 1, controls how much snow there will be in the rock.
     * @param ellisoidAxisSize Sizes of the rock's ellipsoid shape.
     */
    explicit Rock(Shaders &shaders, vcl::vec3 base, float snowCoverage, vcl::vec3 ellisoidAxisSize = {1.0, 0.5, 1.0}, bool verbose = false);

    /**
     * Draws the rock in opengl;
     * @param camera
     */
    void drawMesh(vcl::camera_scene &camera) override;

private:
    vcl::mesh mesh;
    vcl::mesh snowMesh;
    vcl::mesh_drawable rock;
    vcl::mesh_drawable snow;
    static std::shared_ptr<Texture> rockTexture;
    static std::shared_ptr<Texture> snowTexture;

    /**
     * Calculates the rock's mesh using an ellipsoid with pelerin noise.
     * @param base
     */
    void calculateMesh(vcl::vec3 &base, vcl::vec3 &ellisoidAxisSize);

    /**
     * Replaces the top triangles from the rock mesh with snow.
     * @param snowCoverage Float from 0 to 1, controls how much snow there will be in the rock.
     */
    void putSnow(float snowCoverage);
};


#endif //PGM_ROCK_H