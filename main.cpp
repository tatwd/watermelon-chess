#include "Board.h"
#include "SingleGame.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Board w;
    //SingleGame w;
    w.show();

    return a.exec();
}
