#ifndef ONLINEDLG_H
#define ONLINEDLG_H

#include <QDialog>

namespace Ui {
class OnlineDlg;
}

class OnlineDlg : public QDialog
{
    Q_OBJECT

public:
    explicit OnlineDlg(QWidget *parent = 0);
    ~OnlineDlg();

private:
    Ui::OnlineDlg *ui;
};

#endif // ONLINEDLG_H
