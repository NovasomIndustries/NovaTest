#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "usrpswdialog.h"
#include <QSerialPortInfo>
#include <QSerialPort>


#define MAXPORT 100

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int createPDF(QString filename);
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setUserPsw(QString utente, QString password);
    int update_status_bar(QString StatusBarContent);
    QString getUser();
    void superUser();
    void logInSuccesfully();



private slots:

    int checkResult();

    int generateReport(QString filename);

    int testingNovasom(QSerialPort *serialport);

    void on_logIn_pushButton_clicked();

    void popolate_line_combobox();

    QString getPsw();

    void on_model_comboBox_activated(const QString &arg1);

    void on_line_comboBox_activated(const QString &arg1);

    void on_connect_novasom_pushButton_clicked();

    void on_disconnect_novasom_pushButton_clicked();

    void on_save_configuration_pushButton_clicked();

    void on_load_configuration_pushButton_clicked();

    void enablepanel();

    void disablepanel();

    void on_logout_pushButton_clicked();

    void on_runtest_pushButton_clicked();

    void savelastsetting();

    int loadpreviousconfiguration();

    void readSeriale(QSerialPort *serialp);

    //QString todayFileName();


    void on_testpass_checkBox_clicked();

    void on_testFail_checkBox_clicked();

    void on_SendResult_PushButton_clicked();

    void on_PM_checkBox_clicked();

    void on_RM_checkBox_clicked();

    void generateParameters();


    //void on_WriteE2prom_pushButton_clicked();

    //void on_radioButton_clicked();

    void on_pushButton_clicked();

    void on_WriteE2prom_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString user;
    QString psw;
    QString ip;
};

#endif // MAINWINDOW_H
