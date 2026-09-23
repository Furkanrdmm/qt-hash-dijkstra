#include <QApplication>
#include "GridWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GridWindow window;
    window.show();

    return app.exec();
}
