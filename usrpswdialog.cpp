#include "usrpswdialog.h"
#include "ui_usrpswdialog.h"
#include "simplecrypt.h"
#include <QDebug>
#include <QSettings>
#include <QString>
#include <QtGlobal>
#include <iostream>

QString user, password;

using namespace std;

UsrPswDialog::UsrPswDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsrPswDialog)
{
    ui->setupUi(this);
    this->setWindowFlags((
    (windowFlags() | Qt::CustomizeWindowHint)
    & ~Qt::WindowCloseButtonHint
    ));
    this->setWindowTitle("LogIn");
}

UsrPswDialog::~UsrPswDialog()
{
    delete ui;
}

void UsrPswDialog::on_logIn_pushButton_clicked()
{
    qDebug() << "Login button pressed (UserPswDialog)";
    MainWindow *m = qobject_cast<MainWindow*>(this->parent());
    user=ui->usr_lineEdit->text();
    password=ui->psw_lineEdit->text();
    if (user.isEmpty())
    {
        qDebug() << "Username is empty";
        return;
    }

    uint authenticated = authentication(user, password);
    if (authenticated == 1 || authenticated == 2) {
        m->setUserPsw(user, password);
        if (authenticated == 1) {
            qInfo() << "Successful authentication for user " << user;
            m->update_status_bar("Welcome " + user);
        }
        if (authenticated == 2) {
            qInfo() << "Successful authentication for user " << user << " (superuser)";
            m->update_status_bar("Welcome " + user + "(superuser)");
            m->superUser();
        }
        m->logInSuccesfully();
        this->close();
    }
    else {
        qInfo("Wrong username or password");
        m->update_status_bar("WRONG USERNAME OR PASSWORD!");
        ui->usr_lineEdit->clear();
        ui->psw_lineEdit->clear();
        ui->usr_lineEdit->setFocus();
        return;
    }
}

QString UsrPswDialog::read_credentials(QString sourceDir, bool decrypt)
{
    QFile targetFile(sourceDir);
    QString content;
    if (!targetFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Unable to read credentials file";
        QMessageBox::information(this, "Unable to open file", targetFile.errorString());
    }
    else
    {
        QTextStream contentStream(&targetFile);
        content = contentStream.readAll();
        if (decrypt) {
            SimpleCrypt crypto(cryptokey);
            content = crypto.decryptToString(content);
            qDebug() << "Credentials file read";
        }
    }
    targetFile.close();
    return content;
}

void UsrPswDialog::write_credentials(QString content, QString targetDir, bool crypt)
{
    QFile targetFile(targetDir);
    if (!targetFile.open(QIODevice::WriteOnly)) {
        qCritical() << "Unable to write credentials file";
        QMessageBox::information(this, "Unable to open file", targetFile.errorString());
    }
    else
    {
        if (crypt) {
            SimpleCrypt crypto(cryptokey);
            content = crypto.encryptToString(content);
        }
        QTextStream out(&targetFile); out << content;
        targetFile.close();
        qDebug() << "Credentials file updated";
    }
    targetFile.close();
}

uint UsrPswDialog::authentication(QString uname, QString pswd)
{
    MainWindow *m = qobject_cast<MainWindow*>(this->parent());
    m->setAuthentication(0);
    QFile decrFile(decrCredDir);
    QString decrContent = read_credentials(encrCredDir, true);
    write_credentials(decrContent, decrCredDir, false);
    if (decrFile.exists() && !decrContent.isEmpty()) {
        QSettings settings(decrCredDir, QSettings::IniFormat);
        settings.beginGroup("USERS");
        if (settings.contains(uname) && settings.value(uname).toString() == pswd)
        {
            qDebug() << "User" << uname << "successfully authenticated (USER)";
            m->setAuthentication(1);
        }
        settings.endGroup();
        settings.beginGroup("SUSERS");
        if (settings.contains(uname) && settings.value(uname).toString() == pswd)
        {
            qDebug() << "User" << uname << "successfully authenticated (SUPERUSER)";
            m->setAuthentication(2);
        }
        settings.endGroup();
        decrFile.remove();
    }
    else {
        qCritical() << "Invalid credentials file";
        QMessageBox::information(this, "Error reading credentials file", "File is empty or does not exist.");
    }
    return m->getAuthentication();
}

void UsrPswDialog::on_cancel_pushButton_clicked()
{
    MainWindow *m = qobject_cast<MainWindow*>(this->parent());
    this->close();
    m->centralWidget()->show();
}

void UsrPswDialog::on_clear_pushButton_clicked()
{
}
