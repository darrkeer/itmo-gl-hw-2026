#pragma once

#include "openglwidget.h"

#include <GL/gl.h>
#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QWindow>
#include <QMouseEvent>
#include <qobject.h>
#include <qpoint.h>
#include <QSlider>
#include <qslider.h>

class Window : public OpenGLWidget
{
    Q_OBJECT

public:
    constexpr static float SCROLL_SPEED = 120;
    constexpr static float MIN_SHOW_X = 0.1;

    Window(QString program = "func");
    ~Window();

    void onInit() override;
    void onRender() override;
    void onResize(size_t width, size_t height) override;
    
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void recalc_show_y();
    QSlider* create_slider(const std::string& label, int pos_x, int pos_y, float min = 0, float max = 100);
    float get_slider_value(QSlider* slider, float min = 0, float max = 100);
    
    QElapsedTimer timer_;
    float shift_x = 0;
    float shift_y = 0;
    float show_size_x = 10;
    float show_size_y;
    bool mouse_pressed = false;
    QPoint last_mouse_pos;

    QString shader_name;

    QSlider* boldness_slider;
    QSlider* param_a_slider;
    QSlider* param_b_slider;
    QSlider* param_c_slider;
    QSlider* iterations_slider;

    GLint mvpUniform_ = -1;
    GLint show_size_uniform = -1;
    GLint shift_uniform = -1;
    GLint params_uniform = -1;
    GLint boldness_uniform = -1;
    GLint resolution_uniform = -1;
    GLint iterations_uniform = -1;

    QMatrix4x4 model_;
    QMatrix4x4 view_;
    QMatrix4x4 projection_;

    QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
    QOpenGLVertexArrayObject vao_;

    std::unique_ptr<QOpenGLShaderProgram> program_;

    bool animated_ = true;
    bool сolorAnimated_ = animated_;
};
