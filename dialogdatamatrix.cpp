#include "dialogdatamatrix.h"
#include "ui_dialogdatamatrix.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define path "/NOVAtestSMPU/CONFIG/log/ciao.txt"

DialogDataMatrix::DialogDataMatrix(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDataMatrix)
{
    ui->setupUi(this);
    //ui->lineEdit->
    this->setWindowFlags((
    (windowFlags() | Qt::CustomizeWindowHint)
    & ~Qt::WindowCloseButtonHint
    ));
    this->setWindowTitle("Read Data Matrix");
}

DialogDataMatrix::~DialogDataMatrix()
{
    delete ui;
}


void DialogDataMatrix::on_pushButton_clicked()
{
    MainWindow *m = qobject_cast<MainWindow*>(this->parent());
    QString datamatrix= ui->lineEdit->text();
    QFile fp(path);
    qDebug() << m->getParameter();

    if (!fp.open(QIODevice::ReadWrite | QIODevice::Append)){
            std::cout << fp.errorString().toStdString() << std::endl;
            std::cout << "ERRORE APERTURA!" << std::endl;
    }else{
            std::cout << "APERTURA Ciao.txt!" << std::endl;
    }


    fp.write(m->getParameter().toUtf8());
    fp.write("\t");
    fp.write(datamatrix.toUtf8());
    //fp.write("Ciao\n");
    fp.write("\n\r\n\r");
    this->close();
}

void DialogDataMatrix::on_pushButton_2_clicked()
{
    MainWindow *m = qobject_cast<MainWindow*>(this->parent());
    this->close();
    m->centralWidget()->show();
}
