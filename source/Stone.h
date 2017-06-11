#ifndef STONE_H
#define STONE_H


class Stone
{
public:
    Stone();
    ~Stone();

    int _id;
    int _polarAngle;    //极角
    int _polarDiameter; //极径
    bool _dead;         //死活
    bool _black;        //颜色

    //int _librety;       //气

    void init( int id);  //棋子初始化
};

#endif // STONE_H
