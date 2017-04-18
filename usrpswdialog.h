#ifndef USRPSWDIALOG_H
#define USRPSWDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class usrpswdialog;
}

class usrpswdialog : public QDialog
{
    Q_OBJECT

public:
    explicit usrpswdialog(QWidget *parent = 0);
    ~usrpswdialog();

private slots:
    void on_logIn_pushButton_clicked();

    void on_cancel_pushButton_clicked();

    //void on_usrpswdialog_destroyed();

private:
    Ui::usrpswdialog *ui;
};

#endif // USRPSWDIALOG_H
