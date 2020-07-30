#include "ModelDlg.h"
#include "ui_ModelDlg.h"

#include "StandAloneDlg.h"
#include "OnlineDlg.h"

ModelDlg::ModelDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelDlg)
{
    ui->setupUi(this);
}

ModelDlg::~ModelDlg()
{
    delete ui;
}

/*游戏模式*/
int ModelDlg::goGameModel()
{
    //TODO
}

void ModelDlg::on_standAloneButton_clicked()
{
    StandAloneDlg standAlone(this);
    standAlone.exec();
}



void ModelDlg::on_onlineButton_clicked()
{
    OnlineDlg od(this);
    od.exec();
}
