#include "help.h"

#include "SOIL.h"

#include <iostream>
#include <ctime>
#include <chrono>

using namespace help;

GLfloat vertices[] = {
    // front
    -1, 1, 1,  1, 1, 1,  1, -1, 1,  -1, -1, 1,
    // back
    1, 1, -1,  -1, 1, -1,  -1, -1, -1,  1, -1, -1,
    // top
    -1, 1, -1,  1, 1, -1,  1, 1, 1,  -1, 1, 1,
    // bottom
    -1, -1, 1,  1, -1, 1,  1, -1, -1,  -1, -1, -1,
    // right
    1, 1, 1,  1, 1, -1,  1, -1, -1,  1, -1, 1,
    // left
    -1, 1, -1,  -1, 1, 1,  -1, -1, 1,  -1, -1, -1,
};

GLfloat normals[] = {
    // front
    0, 0, 1,  0, 0, 1,  0, 0, 1,  0, 0, 1,
    // back
    0, 0, -1,  0, 0, -1,  0, 0, -1,  0, 0, -1,
    // top
    0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0,
    // bottom
    0, -1, 0,  0, -1, 0,  0, -1, 0,  0, -1, 0,
    // right
    1, 0, 0,  1, 0, 0,  1, 0, 0,  1, 0, 0,
    // left
    -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0
};

GLfloat texcoords[] = {
    // front
    0, 1,  1, 1,  1, 0,  0, 0,
    // back
    0, 1,  1, 1,  1, 0,  0, 0,
    // top
    0, 1,  1, 1,  1, 0,  0, 0,
    // bottom
    0, 1,  1, 1,  1, 0,  0, 0,
    // right
    0, 1,  1, 1,  1, 0,  0, 0,
    // left
    0, 1,  1, 1,  1, 0,  0, 0,
};

GLushort indices[] = {
    //front
    0, 3, 2,   2, 1, 0,
    //back
    4, 7, 6,   6, 5, 4,
    // top
    8,11,10,  10, 9, 8,
    // bottom
    12,15,14,  14,13,12,
    //right
    16,19,18,  18,17,16,
    //left
    20,23,22,  22,21,20,
};

class MainScene : public help::Scene {
public:
    static MainScene * create() {
        auto ret = new MainScene();
        return ret;
    }

    void start() {
        shader = Shader::create("./shader/texture_vert.glsl", "./shader/texture_frag.glsl");
        shader->addAttrib("a_vertex");
        shader->addAttrib("a_texcoord");
        shader->addUniform("u_pvm_matrix");
        shader->addUniform("u_texid");
        shader->addUniform("u_texid2");

        shader->addAttrib("a_normal");

        shader->addUniform("u_model_matrix");
        shader->addUniform("u_view_matrix");
        shader->addUniform("u_normal_matrix");

        shader->addUniform("u_light_vector");
        shader->addUniform("u_light_ambient");
        shader->addUniform("u_light_diffuse");
        shader->addUniform("u_light_specular");

        shader->addUniform("u_material_ambient");
        shader->addUniform("u_material_diffuse");
        shader->addUniform("u_material_specular");
        shader->addUniform("u_material_shininess");

        light_vector      = Vec3(10.0f, 10.0f, 10.0f);
        light_ambient     = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        light_diffuse     = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        light_specular    = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

        // material_ambient  = Vec4(0.464f, 0.393f, 0.094f, 1.0f);
        // material_diffuse  = Vec4(0.464f, 0.393f, 0.094f, 1.0f);
        material_ambient  = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        material_diffuse  = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        material_specular = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

        texture = Texture::create();
        texture->loadTexture("tex.jpg");
        texture->loadTexture("frame.jpg");

        prev = curr = std::chrono::system_clock::now();
    }

    void update() {
        curr = std::chrono::system_clock::now();

        std::chrono::duration<float> elaped_seconds = (curr - prev);

        angle += 20 * elaped_seconds.count();

        prev = curr;
    }

    void draw() {
        shader->use();
        shader->enable();

        auto projection = perspective(60.0f, 1.0f, 0.001f, 10000.0f);
        auto viewInv    = lookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
        auto model      = rotate(angle * 0.5f, 1.0f, 0.0f, 0.0f) * 
                          rotate(angle * 1.0f, 0.0f, 1.0f, 0.0f) * 
                          rotate(angle * 0.7f, 0.0f, 0.0f, 1.0f) * 
                          scale(1, 1, 1);
        auto pvm        = projection * viewInv * model;
        
        glUniformMatrix4fv(shader->getUniformLocation("u_pvm_matrix"), 1, false, pvm);
        glVertexAttribPointer(shader->getAttribLocation("a_vertex"), 3, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(shader->getAttribLocation("a_normal"), 3, GL_FLOAT, GL_FALSE, 0, normals);
        glVertexAttribPointer(shader->getAttribLocation("a_texcoord"), 2, GL_FLOAT, GL_FALSE, 0, texcoords);
        glUniform1i(shader->getUniformLocation("u_texid"), 0);
        glUniform1i(shader->getUniformLocation("u_texid2"), 1);


        Mat3 norm;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                norm(i, j) = model(i, j); 
            }
        }
        Mat4 view = viewInv.getInverse();
        
        glUniformMatrix4fv(shader->getUniformLocation("u_model_matrix"), 1, false, model);
        glUniformMatrix4fv(shader->getUniformLocation("u_view_matrix"), 1, false, view);
        glUniformMatrix3fv(shader->getUniformLocation("u_normal_matrix"), 1, false, norm);

        glUniform3fv(shader->getUniformLocation("u_light_vector"), 1, light_vector);

        glUniform4fv(shader->getUniformLocation("u_light_ambient"), 1, light_ambient);
        glUniform4fv(shader->getUniformLocation("u_light_diffuse"), 1, light_diffuse);
        glUniform4fv(shader->getUniformLocation("u_light_specular"), 1, light_specular);

        glUniform4fv(shader->getUniformLocation("u_material_ambient"), 1, material_ambient);
        glUniform4fv(shader->getUniformLocation("u_material_diffuse"), 1, material_diffuse);
        glUniform4fv(shader->getUniformLocation("u_material_specular"), 1, material_specular);
        glUniform1f(shader->getUniformLocation("u_material_shininess"), material_shininess);

        texture->bindTextures();

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLushort), GL_UNSIGNED_SHORT, indices);

        shader->disable();
        shader->unUse();

        shader->printError("draw");
    }

    void end() {
        shader->release();
        texture->release();
    }

public:
    Shader *shader;
    Texture *texture;

    float angle = 0.0;

    Vec3 light_vector;
    Vec4 light_ambient, light_diffuse, light_specular;
    Vec4 material_ambient, material_diffuse, material_specular;

    float material_shininess = 300.0f;

    std::chrono::time_point<std::chrono::system_clock> prev, curr;

};