#ifndef BOARDPOINT_H
#define BOARDPOINT_H

#include <QString>



class BoardPoint
{
public:
    BoardPoint();
    BoardPoint(int angle, int diameter);

    int PolarAngle;    //极角
    int PolarDiameter; //极径

    QString GetString() { return QString(PolarAngle) + "," + QString(PolarDiameter);  }

};

#endif // BOARDPOINT_H
