#ifndef BOARD_H
#define BOARD_H

#include <QMainWindow>

#include "Stone.h"
#include "Step.h"

namespace Ui {
class Board;
}

class Board : public QMainWindow
{
    Q_OBJECT

public:
    explicit Board(QWidget *parent = 0);
    ~Board();

    int _R;                   //棋盘外圆半径
    int _selectId;            //被选棋子
    int _movedId;             //被走棋子
    int _killId;              //被杀棋子
    bool _beBlackTurn;        //轮流走棋
    bool _endGame;               //游戏结束

    QVector<Step*> _steps;    //记录步数

    Stone _s[12];             //定义12颗棋子


    /*绘制棋盘及棋子*/
    void init();                                               //初始化函数
    QPoint polarCoordToXY(QPoint , double angle, double r);    //极坐标转化成棋子的像素坐标
    QPoint polarCoordToXY(QPoint , int id);
    void paintEvent(QPaintEvent *);                            //绘画事件
    void drawStone(QPainter &, QPoint &, int id);              //绘制棋子
    QPoint getPointOfBoard(int pos,  int &angle, int &r);      //获取棋盘上点像素坐标
    void getAROfBoard(int pos, int &angle, int &r);            //获取棋盘上点的极坐标


    /*基本走棋*/
    QPoint transpCoordToXY(QPoint &);                           //将变换的坐标还原成像素坐标
    void mouseReleaseEvent(QMouseEvent *);                      //鼠标释放事件
    void click(QPoint pt);                                      //点击鼠标
    virtual void click(int id, int angle, int r);               //虚函数click
    int getStoneId(int angle, int r);                           //判断棋子能否被选
    bool canSelect(int id);                                     //获取被点棋子的id
    void trySelectStone(int id);                                //选中棋子
    void tryMoveStone(int id, int angle, int r);                //尝试走棋
    void moveStone(int movedId, int killid, int angle, int r);  //走棋
    void moveStone(int movedId, int angle, int r);
    void playMoveSound();                                       //播放走棋声音
    void movePromptInStatusBar();                               //下棋提示，在状态栏显示
    bool getPolarCoord(QPoint, int &angle, int &r);             //像素坐标转成棋子的极点坐标并判断是否有子,待优化
    void saveStep(int movedId, int killId,  int angle, int r,   //保存步数
                  QVector<Step *>& steps);

    /*规则部分*/
    int getLaps(int r);                         //获取点击位置所在圈数
    bool canMove(int id, int eAngle, int eR);   //判断目标位置是否能走棋
    bool isDead(int id);                        //判断死子
    int getSurroundStone(int id);               //获取棋子被围数
    int getLiberty(int id);                     //获取单个棋子的气

    int getLiberty(int preid, int nexid, QVector<int>& stoneid); //new add
    bool inNextid(int id, QVector<int>* stoneid);                //new add

    bool hasStone(int angle, int r);            //判断位置是否有子
    int getNumOfStone(bool isblack);            //获取棋子数
    void setBeBlackTurn();                      //设定轮到谁走
    void getResult();                           //判断输赢

    /*悔棋*/
    void reliveStone(int id);                   //复活被杀棋子
    void killStone(int id);                     //杀死棋子
    void back();                                //悔棋
    void back(Step* step);
    void backStep();                            //悔一步
    bool isKillIdOfStep(int id, QVector<Step*> steps);                //判断死子是否已经被棋子杀死



private slots:
    /*菜单栏*/
    void on_action_New_triggered();              //新棋局按钮
    void on_action_DisplayToolBar_triggered();   //显示工具栏按钮
    void on_action_CloseMenuBar_triggered();     //关闭菜单栏按钮
    void on_action_Help_triggered();             //帮助按钮
    void on_action_About_triggered();            //关于按钮
    void on_action_Flush_triggered();            //刷新按钮
    void on_action_Resign_triggered();           //认输按钮
    void on_action_Back_triggered();             //悔棋按钮

private:
    Ui::Board *ui;
};

#endif // BOARD_H
