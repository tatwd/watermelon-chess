#include "HelpDlg.h"
#include "ui_HelpDlg.h"

HelpDlg::HelpDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDlg)
{
    ui->setupUi(this);
}

HelpDlg::~HelpDlg()
{
    delete ui;
}

void HelpDlg::on_pushButton_clicked()
{

}
