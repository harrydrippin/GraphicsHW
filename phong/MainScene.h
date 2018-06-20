#include "help.h"

#include "Teapot.h"

#include <iostream>
#include <ctime>

using namespace help;

class MainScene : public help::Scene {
public:
    static MainScene * create() {
        auto ret = new MainScene();
        return ret;
    }

    void start() {
        teapot = new Teapot();

        shader = Shader::create("./shader/p_vert.glsl", "./shader/p_frag.glsl");
        shader->addAttrib("a_vertex");
        shader->addAttrib("a_normal");

        shader->addUniform("u_pvm_matrix");
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

        material_ambient  = Vec4(0.464f, 0.393f, 0.094f, 1.0f);
        material_diffuse  = Vec4(0.464f, 0.393f, 0.094f, 1.0f);
        material_specular = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    void update() {
        angle += 1.0f;
    }

    void draw() {
        shader->use();
        shader->enable();

        auto projection = perspective(60.0f, 1.0f, 0.001f, 10000.0f);
        auto viewInv    = lookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);
        auto model      = rotate(angle * 0.5f, 1.0f, 0.0f, 0.0f) *
                          rotate(angle, 0.0f, 1.0f, 0.0f) *
                          rotate(angle * 0.7f, 0.0f, 0.0f, 1.0f) *
                          scale(5.0f, 5.0f, 5.0f);
        auto pvm        = projection * viewInv * model;

        Mat3 normal;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                normal(i, j) = model(i, j); 
            }
        }
        Mat4 view = viewInv.getInverse();
        
        glUniformMatrix4fv(shader->getUniformLocation("u_pvm_matrix"), 1, false, pvm);
        glUniformMatrix4fv(shader->getUniformLocation("u_model_matrix"), 1, false, model);
        glUniformMatrix4fv(shader->getUniformLocation("u_view_matrix"), 1, false, view);
        glUniformMatrix3fv(shader->getUniformLocation("u_normal_matrix"), 1, false, normal);

        glUniform3fv(shader->getUniformLocation("u_light_vector"), 1, light_vector);

        glUniform4fv(shader->getUniformLocation("u_light_ambient"), 1, light_ambient);
        glUniform4fv(shader->getUniformLocation("u_light_diffuse"), 1, light_diffuse);
        glUniform4fv(shader->getUniformLocation("u_light_specular"), 1, light_specular);

        glUniform4fv(shader->getUniformLocation("u_material_ambient"), 1, material_ambient);
        glUniform4fv(shader->getUniformLocation("u_material_diffuse"), 1, material_diffuse);
        glUniform4fv(shader->getUniformLocation("u_material_specular"), 1, material_specular);
        glUniform1f(shader->getUniformLocation("u_material_shininess"), material_shininess);

        teapot->draw(shader->getAttribLocation("a_vertex"), shader->getAttribLocation("a_normal"));

        shader->disable();
        shader->unUse();

        shader->printError("draw");
    }

    void end() {
        delete teapot;
        shader->release();
    }

public:
    Teapot *teapot;
    Shader *shader;

    Vec3 light_vector;
    Vec4 light_ambient, light_diffuse, light_specular;
    Vec4 material_ambient, material_diffuse, material_specular;

    float material_shininess = 50.0f;
    float angle = 0;

};