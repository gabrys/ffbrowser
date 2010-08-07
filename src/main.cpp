#include <QApplication>
#include <QDir>
#include "browserwindow.h"

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(res);

#ifdef Q_WS_X11
    QApplication::setGraphicsSystem(QString::fromLatin1("raster"));
#endif

    QApplication application(argc, argv);
    application.setStyle("plastique");
    BrowserWindow window;
    window.show();
    window.resize(472, 578);
    return application.exec();
}

