#ifndef MODELDLG_H
#define MODELDLG_H

#include <QDialog>

namespace Ui {
class ModelDlg;
}

class ModelDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ModelDlg(QWidget *parent = 0);
    ~ModelDlg();

    int goGameModel(); //游戏模式

private slots:
    void on_standAloneButton_clicked();

private:
    Ui::ModelDlg *ui;
};

#endif // MODELDLG_H
