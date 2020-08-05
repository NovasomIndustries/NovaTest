#include "usrpswdialog.h"
#include "ui_usrpswdialog.h"
#include "simplecrypt.h"
#include <QDebug>
#include <QSettings>
#include <QString>
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

    MainWindow *m = qobject_cast<MainWindow*>(this->parent());

    user=ui->usr_lineEdit->text();
    password=ui->psw_lineEdit->text();
    if (user.isEmpty())
    {
        //qDebug() << "user is empty!";
        return;
    }

    uint authenticated = authentication(user, password);
    //cout << "Authorization: " << authenticated << endl;

    //if((user=="BRC01" && password=="Operatore01") || user=="michelepuca" || (user=="XD01" && password=="Operatore01")){
    //if(open file --> read file -->if(user==keystring[0] && password==keystring[1])-->encrypt){
    /*if(authenticated != 0){
        m->setUserPsw(user, passwCredord);
        if(authenticated == 2){
            m->update_status_bar("Benvenuto GRANDE PROGRAMMATORE BIANCO!");
            m->superUser();
        }else{
            m->update_status_bar("Benvenuto " + user);
        }
        m->logInSuccesfully();
        this->close();

    }else{
        m->update_status_bar("WRONG USERNAME OR PASSWORD!");
        ui->usr_lineEdit->clear();
        ui->psw_lineEdit->clear();
        //qDebug() << "Wrong login";
        ui->usr_lineEdit->setFocus();
        return;
    }*/

    if (authenticated == 1 || authenticated == 2){
        m->setUserPsw(user, password);
        if (authenticated == 1)
            m->update_status_bar("Benvenuto " + user);
        if (authenticated == 2){
            m->update_status_bar("Benvenuto GRANDE PROGRAMMATORE BIANCO!");
            m->superUser();
        }
        m->logInSuccesfully();
        this->close();
    }
    else{
        m->update_status_bar("WRONG USERNAME OR PASSWORD!");
        ui->usr_lineEdit->clear();
        ui->psw_lineEdit->clear();
        ui->usr_lineEdit->setFocus();
        return;
    }

    //qDebug() << user;
    //qDebug() << password;
    //m->setUserPsw(user,password);
    //if(m->getUser()== "michelepuca"){
    //    m->update_status_bar("Benvenuto GRANDE PROGRAMMATORE BIANCO!");
    //    m->superUser();
    //}else{
    //    m->update_status_bar("Benvenuto " + user);
    //}
    //m->logInSuccesfully();
    //this->close();
    //qobject_cast<MainWindow*>(this->parent())->update_status_bar("Benvenuto " + qobject_cast<MainWindow*>(this->parent())->getUser());

}

QString UsrPswDialog::get_file_content(QString sourceDir, bool decrypt)
{
    QFile targetFile(sourceDir);
    QString content;
    if (!targetFile.open(QIODevice::ReadOnly))
        QMessageBox::information(this, "Unable to open file", targetFile.errorString());
    else
    {
        QTextStream contentStream(&targetFile);
        content = contentStream.readAll();
        if (decrypt)
        {
            SimpleCrypt crypto(cryptokey);
            content = crypto.decryptToString(content);
        }
    }
    targetFile.close();
    return content;
}

void UsrPswDialog::write_file_content(QString content, QString targetDir, bool crypt)
{
    QFile targetFile(targetDir);
    if (!targetFile.open(QIODevice::WriteOnly))
        QMessageBox::information(this, "Unable to open file", targetFile.errorString());
    else
    {
        if (crypt)
        {
            SimpleCrypt crypto(cryptokey);
            content = crypto.encryptToString(content);
        }
        QTextStream out(&targetFile); out << content;
        targetFile.close();
    }
    targetFile.close();
}

uint UsrPswDialog::authentication(QString uname, QString pswd)
{

    MainWindow *m = qobject_cast<MainWindow*>(this->parent());
    m->setAuthentication(0);

    QFile decrFile(decrCredDir);

    QString decrContent = get_file_content(encrCredDir, true);
    write_file_content(decrContent, decrCredDir, false);

    if (decrFile.exists() && !decrContent.isEmpty())
    {
        QSettings settings(decrCredDir, QSettings::IniFormat);

        settings.beginGroup("USERS");
        if (settings.contains(uname) && settings.value(uname).toString() == pswd)
        {
            //cout <<"User: "<< uname.toStdString() <<endl;
            //cout <<"Password: "<< settings.value(uname).toString().toStdString()<<endl;
            m->setAuthentication(1);
        }
        settings.endGroup();

        settings.beginGroup("SUSERS");
        if (settings.contains(uname) && settings.value(uname).toString() == pswd)
        {
            //cout <<"SUser: "<< uname.toStdString() <<endl;
            //cout <<"Password: "<< settings.value(uname).toString().toStdString()<<endl;
            m->setAuthentication(2);
        }
        settings.endGroup();

        decrFile.remove();
    }
    else
        QMessageBox::information(this, "Error file reading", "File is empty or does not exist.");

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
