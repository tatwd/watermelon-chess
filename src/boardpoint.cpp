#include "boardpoint.h"

BoardPoint::BoardPoint() : BoardPoint(0, 0)
{
}

BoardPoint::BoardPoint(int angle, int diameter)
{
    this->PolarAngle = angle;
    this->PolarDiameter = diameter;
}
