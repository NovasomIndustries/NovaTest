#ifndef USRPSWDIALOG_H
#define USRPSWDIALOG_H

#include <QDialog>
#include "mainwindow.h"

#define encrCredDir "/Devel/NOVAtestSMPU/credentials.ini"
#define encrCredBakDir "/Devel/NOVAtestSMPU/credentials.ini.bak"
#define decrCredDir "/Devel/NOVAtestSMPU/deCredentials.ini"
#define cryptokey 0x0c2ad4a4acb9f023

namespace Ui {
class UsrPswDialog;
}

class UsrPswDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsrPswDialog(QWidget *parent = 0);
    ~UsrPswDialog();
    QString read_credentials(QString sourceDir, bool decrypt);
    void write_credentials(QString content, QString targetDir, bool crypt);


private slots:
    void on_logIn_pushButton_clicked();
    void on_cancel_pushButton_clicked();
    uint authentication(QString uname, QString pswd);
    void on_clear_pushButton_clicked();

private:
    Ui::UsrPswDialog *ui;
};

#endif // USRPSWDIALOG_H
