#include "OnlineDlg.h"
#include "ui_OnlineDlg.h"

OnlineDlg::OnlineDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OnlineDlg)
{
    ui->setupUi(this);
}

OnlineDlg::~OnlineDlg()
{
    delete ui;
}
