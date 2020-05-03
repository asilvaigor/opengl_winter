#include "mesh_drawable.hpp"

#include "vcl/opengl/opengl.hpp"

namespace vcl
{

GLuint mesh_drawable::shaderOverride = 0;

mesh_drawable::mesh_drawable()
    :data(),uniform(),shader(0),texture_id(0)
{}

mesh_drawable::mesh_drawable(const mesh& mesh_arg, GLuint shader_arg, GLuint texture_id_arg)
    :data(mesh_arg),uniform(),shader(shader_arg),texture_id(texture_id_arg)
{
}

void mesh_drawable::clear()
{
    data.clear();
}

void mesh_drawable::update_position(const vcl::buffer<vec3>& new_position)
{
    data.update_position(new_position);
}

void mesh_drawable::update_normal(const vcl::buffer<vec3>& new_normal)
{
    data.update_normal(new_normal);
}

void mesh_drawable::overrideShader(GLuint override) {
    shaderOverride = override;
}


void draw(const mesh_drawable& drawable, const camera_scene& camera)
{
    draw(drawable, camera, drawable.shader, drawable.texture_id);
}

void draw(const mesh_drawable& drawable, const camera_scene& camera, GLuint shader)
{
    draw(drawable, camera, shader, drawable.texture_id);
}


void draw(const mesh_drawable& drawable, const camera_scene& camera, GLuint shader, GLuint texture_id)
{
    if (mesh_drawable::shaderOverride != (GLuint) 0)
        shader = mesh_drawable::shaderOverride;

    // If shader is, skip display
    if(shader==0)
        return ;

    // Check that the shader is a valid one
    if( glIsProgram(shader)==GL_FALSE ) {
        std::cout<<"No valid shader set to display mesh: skip display"<<std::endl;
        return;
    }

    // Switch shader program only if necessary
    GLint current_shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_shader); opengl_debug();
    if(shader!=GLuint(current_shader))
        glUseProgram(shader); opengl_debug();

    // Associating textures
    auto texture_sampler = glGetUniformLocation(shader, "texture_sampler");
    auto shadow_map_still_1 = glGetUniformLocation(shader, "shadow_map_still_1");
    auto shadow_map_movable_1 = glGetUniformLocation(shader, "shadow_map_movable_1");
    auto shadow_map_still_2 = glGetUniformLocation(shader, "shadow_map_still_2");
    auto shadow_map_movable_2 = glGetUniformLocation(shader, "shadow_map_movable_2");
    auto shadow_map_still_3 = glGetUniformLocation(shader, "shadow_map_still_3");
    auto shadow_map_movable_3 = glGetUniformLocation(shader, "shadow_map_movable_3");
    glUseProgram(shader);
    glUniform1i(texture_sampler, 0);
    glUniform1i(shadow_map_still_1, 1);
    glUniform1i(shadow_map_movable_1, 2);
    glUniform1i(shadow_map_still_2, 3);
    glUniform1i(shadow_map_movable_2, 4);
    glUniform1i(shadow_map_still_3, 5);
    glUniform1i(shadow_map_movable_3, 6);

    // Bind texture only if id != 0
    if(texture_id!=0) {
        assert(glIsTexture(texture_id));
        glBindTexture(GL_TEXTURE_2D, texture_id);  opengl_debug();
    }

    // Send all uniform values to the shader
    uniform(shader, "rotation", drawable.uniform.transform.rotation);            opengl_debug();
    uniform(shader, "translation", drawable.uniform.transform.translation);      opengl_debug();
    uniform(shader, "color", drawable.uniform.color);                            opengl_debug();
    uniform(shader, "color_alpha", drawable.uniform.color_alpha);                opengl_debug();
    uniform(shader, "scaling", drawable.uniform.transform.scaling);              opengl_debug();
    uniform(shader, "scaling_axis", drawable.uniform.transform.scaling_axis);    opengl_debug();

    uniform(shader,"perspective",camera.perspective.matrix());         opengl_debug();
    uniform(shader,"view",camera.view_matrix());                       opengl_debug();
    uniform(shader,"camera_position",camera.camera_position());        opengl_debug();

    uniform(shader, "ambiant", drawable.uniform.shading.ambiant);      opengl_debug();
    uniform(shader, "diffuse", drawable.uniform.shading.diffuse);      opengl_debug();
    uniform(shader, "specular", drawable.uniform.shading.specular);    opengl_debug();
    uniform(shader, "specular_exponent", drawable.uniform.shading.specular_exponent); opengl_debug();

    uniform(shader, "current_light", drawable.uniform.current_light);  opengl_debug();
    uniform(shader, "light_matrix", drawable.uniform.light->get_matrix()); opengl_debug();
    uniform(shader, "light_matrix_2", drawable.uniform.light2->get_matrix()); opengl_debug();
    uniform(shader, "light_matrix_3", drawable.uniform.light3->get_matrix()); opengl_debug();
    uniform(shader, "light_pos", drawable.uniform.light->get_pos());    opengl_debug();
    uniform(shader, "light_color", drawable.uniform.light->get_color()); opengl_debug();
    uniform(shader, "shadow_map_id", drawable.uniform.light->get_shadow_map_id()); opengl_debug();

    vcl::draw(drawable.data); opengl_debug();


}

}
