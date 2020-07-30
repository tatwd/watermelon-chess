#include "SingleGame.h"

#include <QTimer>
#include <QDebug>

SingleGame::SingleGame()
{
    _level = 4;
}

SingleGame::~SingleGame()
{

}

void SingleGame::click(int id, int angle, int r)
{
    if(_beBlackTurn)
    {
        Board::click(id, angle, r); //人执黑棋先走

        if(!_beBlackTurn && !_endGame)
        {
            //延迟刷新
            QTimer::singleShot(100, this, SLOT(computerMove()));
        }
    }
}


/*电脑走棋*/
void SingleGame::computerMove()
{

    Step* step = getBestStep(); //电脑走棋

    moveStone(step->_movedId, step->_killId, step->_angleTo, step->_rTo);

    Board::movePromptInStatusBar(); //落子提示

    delete step;
    update();
}

/*获取最佳走法*/
Step* SingleGame::getBestStep()
{
    QVector<Step *> steps;
    getAllPossibleMove(steps);

    int maxScore = -100000;
    Step* ret = NULL;
    while(steps.count())
    {
        Step* step = steps.last();
        steps.removeLast();

        fakeMove(step);
        //fakeMove(step, steps);

        int score = getMinScore(_level - 1, maxScore);
        unfakeMove(step);

        if(score > maxScore)
        {
            maxScore = score;

            if(ret)
                delete ret;
            ret = step;
        }
        else
        {
            delete step; //防止内存泄漏
        }
    }

    return ret;
}

void SingleGame::getAllPossibleMove(QVector<Step *> &steps)
{
    int iStart = 0;
    int iEnd = 6;

    if(!_beBlackTurn)
    {
        iStart = 6;
        iEnd = 12;
    }

    for(int sid = iStart; sid < iEnd;++sid)
    {
        if(_s[sid]._dead)
            continue;

        int angle = 0;
        int r = 0;

        for(int i = 0; i < 21; ++i)
        {
            getAROfBoard(i, angle, r); //获取极坐标

            int killid = getStoneId(angle, r); //目标位置id
            if(canMove(sid, angle, r)) //目标位置无棋子且走后不会死
            {
                if(killid==-1)
                {
                    saveStep(sid, killid, angle, r, steps);
                }
            }
        }

    }
}

/*假想走*/
void SingleGame::fakeMove(Step* step, QVector<Step *> steps)
{
    killStone(step->_killId);

    moveStone(step->_movedId, step->_angleTo, step->_rTo);

    //add
    for(int i = 0;i < 12;++i)
    {
        if(_s[i]._dead) //判断是否是死子
        {
            //判断此死子是否在_steps中
            if(!isKillIdOfStep(i, _steps) && !isKillIdOfStep(i, steps))
            {
                //_killId = i;
                step->_killId = i;

            }
            step->_killId = -1;
        }
    }

}

void SingleGame::fakeMove(Step* step)
{
    killStone(step->_killId);
    //moveStone(step->_movedId,step->_killId, step->_angleTo, step->_rTo);
    moveStone(step->_movedId, step->_angleTo, step->_rTo);

    for(int i = 0;i < 12;++i)
    {
        if(isDead(i)) //判断是否是死子
        {
            //判断此死子是否在_steps中
            if(!isKillIdOfStep(i, _steps))
            {
                _killId = i;
                step->_killId = _killId;
            }
            _killId = -1;
        }
    }
}

/*想象回*/
void SingleGame::unfakeMove(Step* step)
{
    reliveStone(step->_killId);
    moveStone(step->_movedId, step->_angleFrom, step->_rFrom);
}

/*价值评估*/
int SingleGame::calcScore()
{
    int totalBlack  = 0;
    int totalWhite = 0;

    for(int i = 0;i < 12;++i)
    {
        if(_s[i]._dead)
            continue;

        if(_s[i]._black)
            totalBlack += getLiberty(i);

        totalWhite += getLiberty(i);
    }

    return totalBlack -totalWhite;
}

/*最小值*/
int SingleGame::getMinScore(int level, int curMaxScore)
{
    if(level == 0)
        return calcScore();

    QVector<Step *> steps;
    getAllPossibleMove(steps);   //获取白棋的possiblemove

    int minScore = 300000;
    while(steps.count())
    {
        Step* step = steps.last();
        steps.removeLast();

        fakeMove(step);
        //fakeMove(step, steps);
        int score = getMaxScore(level - 1, minScore);
        unfakeMove(step);
        delete step; //防止内存泄漏

        if(score <= curMaxScore)
        {
            while(steps.count()) //去除剩余步数
            {
                Step* step = steps.last();
                steps.removeLast();
                delete step;
            }
            return score;
        }

        if(score < minScore)
        {
            minScore = score;
        }


    }
    return minScore;
}

int SingleGame::getMaxScore(int level, int curMinScore)
{
    if(level == 0)
        return calcScore();

    QVector<Step *> steps;
    getAllPossibleMove(steps);   //获取白棋的possiblemove

    int maxScore = -300000;
    while(steps.count())
    {
        Step* step = steps.last();
        steps.removeLast();

        fakeMove(step);
        //fakeMove(step, steps);
        int score = getMinScore(level - 1, maxScore);
        unfakeMove(step);

        delete step; //防止内存泄漏

        if(score >= curMinScore)
        {
            while(steps.count()) //去除剩余步数
            {
                Step* step = steps.last();
                steps.removeLast();
                delete step;
            }
            return score;
        }

        if(score > maxScore)
        {
            maxScore = score;
        }


    }
    return maxScore;
}
