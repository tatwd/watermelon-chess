#ifndef RESIGNDLG_H
#define RESIGNDLG_H

#include <QDialog>

namespace Ui {
class ResignDlg;
}

class ResignDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ResignDlg(QWidget *parent = 0);
    ~ResignDlg();

    void changeText(bool isblack);

private:
    Ui::ResignDlg *ui;
};

#endif // RESIGNDLG_H
