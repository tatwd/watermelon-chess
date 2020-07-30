#include "StandAloneDlg.h"
#include "ui_StandAloneDlg.h"

#include "SingleGame.h"
#include "MultiGame.h"

StandAloneDlg::StandAloneDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StandAloneDlg)
{
    ui->setupUi(this);
}

StandAloneDlg::~StandAloneDlg()
{
    delete ui;
}

void StandAloneDlg::chooseSingMulti()
{
    //TODO
}

void StandAloneDlg::on_goSingleGame_clicked()
{
    QApplication::closeAllWindows(); //关闭原主窗口

    SingleGame *sg = new SingleGame();
    sg->show();
}

void StandAloneDlg::on_goMultiGame_clicked()
{
    QApplication::closeAllWindows(); //关闭原主窗口

    MultiGame *mg = new MultiGame();
    mg->show();
}
