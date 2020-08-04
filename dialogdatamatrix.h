#ifndef DIALOGDATAMATRIX_H
#define DIALOGDATAMATRIX_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class DialogDataMatrix;
}

class DialogDataMatrix : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDataMatrix(QWidget *parent = 0);
    ~DialogDataMatrix();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::DialogDataMatrix *ui;
};

#endif // DIALOGDATAMATRIX_H
