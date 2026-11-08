#include "window.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <iostream>
#include <qobject.h>

constexpr auto g_sampels = 16;
constexpr auto g_gl_major_version = 3;
constexpr auto g_gl_minor_version = 3;

int main(int argc, char *argv[])
{
    // Create app and set attributes.
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication a(argc, argv);

    // Set default surface format.
    QSurfaceFormat format;
    format.setSamples(g_sampels);
    format.setVersion(g_gl_major_version, g_gl_minor_version);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QString program = "func";
    if (a.arguments().size() > 1) {
        program = a.arguments()[1];
    }

    if (program != "func" && program != "fractal") {
        std::cerr << "invalid program name, it should be either func or fractal" << std::endl;
        return 1;
    }

    Window window(program);
    window.resize(640, 480);
    window.show();

    return a.exec();
}
