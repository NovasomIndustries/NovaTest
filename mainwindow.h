#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string.h>
#include <QMainWindow>
#include <QtNetwork/QNetworkInterface>
//#include <QNetworkInterface>
//#include <QSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QFile>
#include <QSettings>
#include <QMessageBox>
//#include <QSerialPortInfo>
#include <QtSerialPort/QSerialPortInfo>
#include <QDate>
#include <QApplication>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QFileInfo>
#include <QFileDialog>
#include <iostream>

#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>



#include "usrpswdialog.h"
#include "dialogdatamatrix.h"

#define MAXPORT 100




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int update_status_bar(QString StatusBarContent);
    void setUserPsw(QString utente, QString password);
    QString getUser();
    QString getUserChar(QString uname);
    void superUser();
    void logInSuccesfully();
    QString getPsw();
    QString getParameter();
    int getAuthentication ();
    void setAuthentication (int authFlag);
    QNetworkAccessManager *manager = 0;
    void setPsw(const QString &value);

    QString getPswNlog() const;
    void setPswNlog(const QString &value);

private slots:
    void savelastsetting();

    void enablepanel();

     void disablepanel();

     void on_logIn_pushButton_clicked();

     void on_logout_pushButton_clicked();

     void on_disconnect_novasom_pushButton_clicked();

     void on_line_comboBox_activated(const QString &arg1);

     void popolate_line_combobox();

     void on_model_comboBox_activated(const QString &arg1);

     //QString getvalue(QString strKey, QSettings *settings , QString entry);

     int loadpreviousconfiguration();

     void on_connect_novasom_pushButton_clicked();

     void on_runtest_pushButton_clicked();

     int testingNovasom(QSerialPort *serialp);

     int finisciprocedurasingoloNOVASOM(QSerialPort *serialp);

     void on_GenerateParameters_PushButton_clicked();

     int checkResult();

     void generateParameters();

     void on_Write_EEprom_pushButton_clicked();

     int generateReport(QString filename);

     int createPDF(QString filename);

     void on_Print_Label_pushButton_clicked();

     int generateLabel(QString filename);

     int printLabel(QString filename);

     void on_PM_checkBox_clicked();

     void on_RM_checkBox_clicked();

     void on_testpass_checkBox_clicked();

     void on_testFail_checkBox_clicked();

     void on_save_configuration_pushButton_clicked();

     void on_load_configuration_pushButton_clicked();

     void on_NextPort_pushButton_clicked();

     //void on_pushButton_clicked();

     void on_readDataMatrix_clicked();

     QString get_OC(QString pn);

     QString get_net_name();

     void on_suser_operations_pushButton_clicked();

     int uploadReport(QString filename, QString path);

     bool compareMacs(QNetworkInterface iface);

     void replyFinished(QNetworkReply*);

     void pswNlogSlt(QString logstr);

     QString getPass();

signals:
     void pswNlogSig(QString);

private:
    Ui::MainWindow *ui;
    QString user;
    QString psw;
    QString ip;
    uint authenticated;
    QString pswNlog;
    bool pause;
};

#endif // MAINWINDOW_H
