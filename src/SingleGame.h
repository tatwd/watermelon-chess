#ifndef SINGLEGAME_H
#define SINGLEGAME_H

#include "Board.h"

class SingleGame : public Board
{
    Q_OBJECT

public:
    SingleGame();
    ~SingleGame();

    virtual void click(int id, int angle, int r);

    Step* getBestStep();                                 //获取最佳走法
    void getAllPossibleMove(QVector<Step *> &steps);     //获取所有可走棋子

    void fakeMove(Step* step);                           //假想走
    void fakeMove(Step* step, QVector<Step *> steps);

    void unfakeMove(Step* step);                         //想象回
    int calcScore();                                     //价值评估
    int getMinScore(int level, int curMaxScore);                                   //最小值
    int getMaxScore(int level, int curMinScore);

    int _level;

public slots:
    void computerMove();                                 //电脑走棋

};

#endif // SINGLEGAME_H
