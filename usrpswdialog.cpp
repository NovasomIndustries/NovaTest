#include "usrpswdialog.h"
#include "ui_usrpswdialog.h"
#include <QDebug>

QString user, password;

usrpswdialog::usrpswdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::usrpswdialog)
{
    ui->setupUi(this);
    this->setWindowFlags((
    (windowFlags() | Qt::CustomizeWindowHint)
    & ~Qt::WindowCloseButtonHint
    ));
    this->setWindowTitle("LogIn");
}

usrpswdialog::~usrpswdialog()
{
    delete ui;
}

void usrpswdialog::on_logIn_pushButton_clicked()
{

    MainWindow *m = qobject_cast<MainWindow*>(this->parent());

    user=ui->usr_lineEdit->text();
    password=ui->psw_lineEdit->text();
    if (user.isEmpty())
    {
        qDebug() << "user is empty!";
        return;
    }
    qDebug() << user;
    qDebug() << password;
    m->setUserPsw(user,password);
    if(m->getUser()== "michelepuca"){
        m->update_status_bar("Benvenuto GRANDE PROGRAMMATORE BIANCO!");
        m->superUser();
    }else{
        m->update_status_bar("Benvenuto " + user);
    }
    m->logInSuccesfully();
    this->close();
    //qobject_cast<MainWindow*>(this->parent())->update_status_bar("Benvenuto " + qobject_cast<MainWindow*>(this->parent())->getUser());




}

void usrpswdialog::on_cancel_pushButton_clicked()
{
    MainWindow *m = qobject_cast<MainWindow*>(this->parent());
    this->close();
    m->centralWidget()->show();

}

