#include "ResignDlg.h"
#include "ui_ResignDlg.h"

ResignDlg::ResignDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResignDlg)
{
    ui->setupUi(this);


}

ResignDlg::~ResignDlg()
{
    delete ui;
}

void ResignDlg::changeText(bool isblack)
{
    if(ui->label->text() == "未知胜负")
    {
        if(isblack)
        {
            ui->label->setText("黑输");
        }
        else
        {
            ui->label->setText("白输");
        }
    }
}
