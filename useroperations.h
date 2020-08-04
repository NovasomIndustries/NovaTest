#ifndef USEROPERATIONS_H
#define USEROPERATIONS_H

#include <QDialog>
#include <iostream>

namespace Ui {
class userOperations;
}

class userOperations : public QDialog
{
    Q_OBJECT

public:
    explicit userOperations(QWidget *parent = 0);
    ~userOperations();

private slots:
    void on_apply_pushButton_clicked();

    uint operations();

    void on_done_pushButton_clicked();

private:
    Ui::userOperations *ui;
};

#endif // USEROPERATIONS_H
