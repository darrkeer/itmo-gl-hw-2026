#include "window.h"

#include <QLabel>
#include <qnamespace.h>
#include <qslider.h>
#include <qvectornd.h>
#include <QSlider>

constexpr std::array<GLfloat, 8u> vertices = {
    -1.0f,   1.0f,
    -1.0f, -1.0f,
    1.0f,-1.0f,
    1.0f, 1.0f
};

constexpr std::array<GLuint, 6u> indices = {
    0, 1, 2,
    0, 2, 3
};

Window::Window()
{
    // Add label for FPS
    auto* fpsLabel = new QLabel("FPS: 0", this);
    fpsLabel->setStyleSheet("QLabel { color: white; }");
    fpsLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    fpsLabel->adjustSize();
    fpsLabel->move(0, 0);
    fpsLabel->raise();

    boldness_slider = create_slider("Change boldness:", 0, 30);
    param_a_slider = create_slider("A parameter:", 0, 80);
    param_b_slider = create_slider("B parameter:", 0, 130);
    param_c_slider = create_slider("C parameter:", 0, 180);

    connect(this, &Window::fpsUpdated, [fpsLabel](double fps) {
        fpsLabel->setText(QString("FPS: %1").arg(fps, 0, 'f', 0));
        fpsLabel->adjustSize();
    });

    recalc_show_y();
}

Window::~Window()
{
    program_.reset();
}

void Window::onInit()
{
    // Configure shaders
    program_ = std::make_unique<QOpenGLShaderProgram>(this);
    program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/template.vs");
    program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/template.fs");
    if (!program_->link()) {
        qDebug() << "Ошибка линковки:" << program_->log();
    }
    program_->bind();

    // Configure uniforms
    mvpUniform_ = program_->uniformLocation("mvp");
    show_size_uniform = program_->uniformLocation("show_size");
    shift_uniform = program_->uniformLocation("shift");
    params_uniform = program_->uniformLocation("params");  
    boldness_uniform = program_->uniformLocation("boldness");

    // Create VAO object
    vao_.create();
    vao_.bind();

    // Create VBO
    vbo_.create();
    vbo_.bind();
    vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

    // Create IBO
    ibo_.create();
    ibo_.bind();
    ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

    // Bind attributes
    program_->enableAttributeArray(0);
    program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));

    // Release all
    program_->release();

    vao_.release();
    vbo_.release();
    ibo_.release();

    // Start timer for animation (if needed)
    timer_.start();

    // Еnable depth test and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Clear all FBO buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate MVP matrix
    model_.setToIdentity();
    model_.translate(0, 0, -1.5);
    view_.setToIdentity();
    const auto mvp = projection_ * view_ * model_;

    // Bind shader program
    program_->bind();

    // Update uniform values
    program_->setUniformValue(mvpUniform_, mvp);
    program_->setUniformValue(show_size_uniform, QVector2D(show_size_x, show_size_y));
    program_->setUniformValue(shift_uniform, QVector2D(shift_x, shift_y));
    auto params = QVector3D(
        get_slider_value(param_a_slider),
        get_slider_value(param_b_slider),
        get_slider_value(param_c_slider)
    );
    program_->setUniformValue(params_uniform, params);
    program_->setUniformValue(boldness_uniform, get_slider_value(boldness_slider));

    // Bind VAO
    vao_.bind();

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Release all
    vao_.release();
    program_->release();

    if (animated_)
        update();
}

void Window::onResize(const size_t width, const size_t height)
{
    // Configure viewport
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

    // Configure matrix
    const auto aspect = static_cast<float>(width) / static_cast<float>(height);
    const auto zNear = 0.1f;
    const auto zFar = 100.0f;
    const auto fov = 60.0f;
    projection_.setToIdentity();
    projection_.perspective(fov, aspect, zNear, zFar);

    recalc_show_y();
}

void Window::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mouse_pressed = true;
        last_mouse_pos = event->pos();
    }
}

void Window::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mouse_pressed = false;
    }
}

void Window::mouseMoveEvent(QMouseEvent *event) {
    if (mouse_pressed) {
        auto delta = event->pos() - last_mouse_pos;
        shift_x -= delta.x() * show_size_x / size().width();
        shift_y += delta.y() * show_size_y / size().height();

    }
    last_mouse_pos = event->pos();
}

void Window::wheelEvent(QWheelEvent *event) {
    auto delta = static_cast<float>(event->angleDelta().y()) / SCROLL_SPEED;
    show_size_x = std::max(MIN_SHOW_X, show_size_x - delta);
    recalc_show_y();
}

void Window::recalc_show_y() {
    show_size_y = show_size_x * size().height() / size().width();
}

QSlider* Window::create_slider(const std::string& label, int pos_x, int pos_y) {
    auto* l = new QLabel(label.c_str(), this);
    l->setStyleSheet("QLabel { color: white; }");
    l->setAttribute(Qt::WA_TransparentForMouseEvents);
    l->adjustSize();
    l->move(pos_x, pos_y);
    l->raise();

    auto *slider = new QSlider(Qt::Horizontal, this);
    slider->resize(100, 30);
    slider->move(pos_x, pos_y + 15);
    slider->setRange(0, 100);
    slider->setValue(50);
    slider->show();

    return slider;
}

float Window::get_slider_value(QSlider* slider, float min, float max) {
    return (slider->value() - min) / (max - min);
}