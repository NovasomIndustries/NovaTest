#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <windows.h>
#include <QDate>
#include <QTime>
#include <QPdfWriter>
#include <QPrinter>
#include <QPainter>
#include <QNetworkInterface>
#include <qnetworkinterface.h>




/*
QString ConfigDir = "C:/Michele/NOVONOVATest/NOVATesting/CONFIG";
QString directory = "C:/Michele/NOVONOVATest/NOVATesting/Tested/";
QString userConfigDir = "C:/Michele/NOVONOVATest/NOVATesting/USERCONFIG/";
QString logDir = "C:/Michele/NOVONOVATest/NOVATesting/log/";
QString MACdir = "C:/Michele/NOVONOVATest/NOVATesting/MAC/";
QString xampdirectory= "C:/xampp/htdocs/";
*/


QString filezilla = "C:/Users/M.puca/Desktop/condivisaFilezilla/";
QString ConfigDir = "C:/QtProjects/NOVATEST/CONFIG";
QString directory = "C:/QtProjects/NOVATEST/Tested/";
QString userConfigDir = "C:/QtProjects/NOVATEST/USERCONFIG/";
QString logDir = "C:/QtProjects/NOVATEST/log/";
QString MACdir = "C:/QtProjects/NOVATEST/MAC/";
QString xampdirectory= "C:/QtProjects/NOVATEST/xampp/htdocs/";


QSerialPort *serialport;
QSerialPort serial[MAXPORT];
bool somethingwrong = false;
bool superuser = false;
int connectedNOVAsom=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->save_configuration_pushButton->hide();
    ui->load_configuration_pushButton->hide();

    update_status_bar("Please LogIn...");
    setWindowTitle("NOVAtest");
}

void clearstdstr( char *stringa){
    for(int i=0;i<strlen(stringa);i++){
        stringa[i]='\0';
    }
    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_logIn_pushButton_clicked()
{
    usrpswdialog *p = new usrpswdialog(this);
    p->show();
}

void MainWindow::setUserPsw(QString utente, QString password)
{
    user=utente;
    psw=password;
    return;
}

QString MainWindow::getUser()
{
    return user;
}

QString MainWindow::getPsw()
{
    return psw;
}

int MainWindow::update_status_bar(QString StatusBarContent){

    statusBar()->showMessage(StatusBarContent);
    return 0;
}

void MainWindow::enablepanel(){
    ui->audio_checkBox->setEnabled(true);
    ui->bt_checkBox->setEnabled(true);
    ui->console_checkBox->setEnabled(true);
    ui->cpu_checkBox->setEnabled(true);
    ui->eeprom_checkBox->setEnabled(true);
    ui->eMMC_checkBox->setEnabled(true);
    ui->eth_checkBox->setEnabled(true);
    ui->hdmi_checkBox->setEnabled(true);
    ui->pcie_checkBox->setEnabled(true);
    ui->ram_checkBox->setEnabled(true);
    ui->rtc_checkBox->setEnabled(true);
    ui->sata_checkBox->setEnabled(true);
    ui->sdcard_checkBox->setEnabled(true);
    ui->sensors_checkBox->setEnabled(true);
    ui->touch_checkBox->setEnabled(true);
    ui->usb_checkBox->setEnabled(true);
    ui->wifi_checkBox->setEnabled(true);

    return;
}

void MainWindow::disablepanel(){
    ui->audio_checkBox->setEnabled(false);
    ui->bt_checkBox->setEnabled(false);
    ui->console_checkBox->setEnabled(false);
    ui->cpu_checkBox->setEnabled(false);
    ui->eeprom_checkBox->setEnabled(false);
    ui->eMMC_checkBox->setEnabled(false);
    ui->eth_checkBox->setEnabled(false);
    ui->hdmi_checkBox->setEnabled(false);
    ui->pcie_checkBox->setEnabled(false);
    ui->ram_checkBox->setEnabled(false);
    ui->rtc_checkBox->setEnabled(false);
    ui->sata_checkBox->setEnabled(false);
    ui->sdcard_checkBox->setEnabled(false);
    ui->sensors_checkBox->setEnabled(false);
    ui->touch_checkBox->setEnabled(false);
    ui->usb_checkBox->setEnabled(false);
    ui->wifi_checkBox->setEnabled(false);


    return;
}

void MainWindow::superUser(){
    ui->load_configuration_pushButton->show();
    ui->load_configuration_pushButton->setEnabled(true);
    ui->save_configuration_pushButton->show();
    ui->save_configuration_pushButton->setEnabled(true);
    superuser=true;
    logInSuccesfully();
    enablepanel();
    return;
}

void MainWindow::logInSuccesfully(){
    //QNetworkAddressEntry eth1Ip;
    QList<QNetworkInterface> entries = QNetworkInterface::allInterfaces();
           //qDebug() << entries.count();
           for (int i=0;i<entries.count();i++){
               QNetworkInterface iface= entries.at(i);
               if(iface.flags().testFlag(QNetworkInterface::IsUp )){
                   //qDebug() << iface.name();
                   for( int j =0 ; j< iface.addressEntries().count();j++){
                       if(iface.addressEntries().at(j).ip().protocol()==QAbstractSocket::IPv4Protocol){
                           if(iface.name()=="ethernet_8"){
                                qDebug() << iface.addressEntries().at(j).ip().toString();
                                ip = iface.addressEntries().at(j).ip().toString();
                                qDebug() << ip;
                                qDebug() << iface.addressEntries().at(j).ip().protocol();
                           }
                       }


               }
            }
           }


    ui->logIn_pushButton->setEnabled(false);
    ui->logout_pushButton->setEnabled(true);
    ui->line_comboBox->setEnabled(true);
    popolate_line_combobox();
    if (loadpreviousconfiguration()==1){ //non ottimizzato secondo me!!
        on_line_comboBox_activated(ui->line_comboBox->currentText());
        loadpreviousconfiguration();
        on_model_comboBox_activated(ui->model_comboBox->currentText());
    }
}

QString getvalue(QString strKey, QSettings *settings , QString entry)
{
    return settings->value( strKey + entry, "r").toString();
}

void MainWindow::on_model_comboBox_activated(const QString &arg1)
{
    QString filename;
    filename=(ConfigDir+"/"+arg1+".ini");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "[ERRORE]Non sono riuscito ad aprire "+ filename;
        update_status_bar("[ERROR] No configuration file...Please contact us");
    }else{
        qDebug() << "File aperto correttamente";
        QString strKeyFunc("CONFIG/");
        QSettings * settings = 0;
        settings = new QSettings( filename, QSettings::IniFormat );
        if ( getvalue(strKeyFunc, settings , "audio_checkBox") == "true")
            {
                qDebug() << "true";
                ui->audio_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->audio_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "bt_checkBox") == "true")
            {
                qDebug() << "true";
                ui->bt_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->bt_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "console_checkBox") == "true")
            {
                qDebug() << "true";
                ui->console_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->console_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "cpu_checkBox") == "true")
            {
                qDebug() << "true";
                ui->cpu_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->cpu_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "eeprom_checkBox") == "true")
            {
                qDebug() << "true";
                ui->eeprom_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->eeprom_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "eMMC_checkBox") == "true")
            {
                qDebug() << "true";
                ui->eMMC_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->eMMC_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "eth_checkBox") == "true")
            {
                qDebug() << "true";
                ui->eth_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->eth_checkBox->setChecked(false);
            }

        if ( getvalue(strKeyFunc, settings , "hdmi_checkBox") == "true")
            {
                qDebug() << "true";
                ui->hdmi_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->hdmi_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "pcie_checkBox") == "true")
            {
                qDebug() << "true";
                ui->pcie_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->pcie_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "ram_checkBox") == "true")
            {
                qDebug() << "true";
                ui->ram_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->ram_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "rtc_checkBox") == "true")
            {
                qDebug() << "true";
                ui->rtc_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->rtc_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "sata_checkBox") == "true")
            {
                qDebug() << "true";
                ui->sata_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->sata_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "sdcard_checkBox") == "true")
            {
                qDebug() << "true";
                ui->sdcard_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->sdcard_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "sensors_checkBox") == "true")
            {
                qDebug() << "true";
                ui->sensors_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->sensors_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "touch_checkBox") == "true")
            {
                qDebug() << "true";
                ui->touch_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->touch_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "usb_checkBox") == "true")
            {
                qDebug() << "true";
                ui->usb_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->usb_checkBox->setChecked(false);
            }
        if ( getvalue(strKeyFunc, settings , "wifi_checkBox") == "true")
            {
                qDebug() << "true";
                ui->wifi_checkBox->setChecked(true);
            }
            else
            {
                qDebug() << "false";
                ui->wifi_checkBox->setChecked(false);
            }
            file.close();
    }
    ui->connect_novasom_pushButton->setEnabled(true);
    update_status_bar("Model selected: : "+ arg1);
    file.close();
}

void MainWindow::popolate_line_combobox(){

    QString filename;
    filename=(ConfigDir+"/line.ini");
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "[ERRORE]Non sono riuscito ad aprire linecombobox!";

    }else{
        qDebug() << "File aperto correttamente";
    }
    ui->line_comboBox->clear();
    QString line;
    QTextStream in(&file);
    while (!in.atEnd()){
        line = in.readLine();
        if(line==""){ //la riga vuota è la fine della configuraione delle schede..
            break;
        }
        ui->line_comboBox->addItem(line);
    }
    file.close();
    return;
}

void MainWindow::on_line_comboBox_activated(const QString &arg1)
{
    QString filename=(ConfigDir+"/"+arg1+".ini");
     QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "[ERRORE]Non sono riuscito ad aprire combobox!";
    }else{
        qDebug() << "File aperto correttamente";
    }
    ui->model_comboBox->clear();
    ui->connect_novasom_pushButton->setEnabled(false);
    QString line;
    QTextStream in(&file);
    while (!in.atEnd()){
        line = in.readLine();
        if(line==""){ //la riga vuota è la fine della configuraione delle schede..
            break;
        }
        ui->model_comboBox->addItem(line);
    }
     ui->model_comboBox->setEnabled(true);
     update_status_bar("Line selected : "+ arg1);
     file.close();
    return;
}

int initPort(int num,QSerialPortInfo info){
    serial[num].setPort(info);
    qDebug() << info.description();
    serialport=&serial[num];
    qDebug () << info.description();
    if (serial[num].open(QIODevice::ReadWrite))
    {
        qDebug() << "porta connessa!";
        serial[num].setBaudRate(QSerialPort::Baud115200);
        serial[num].setParity(QSerialPort::NoParity);
        //serial[num].setParity(QSerialPort::EvenParity);
        serial[num].setStopBits(QSerialPort::OneStop);
        serial[num].setFlowControl(QSerialPort::NoFlowControl);
        serial[num].setDataBits(QSerialPort::Data8);
        return num;
       }else{
        qDebug () << "non sono riuscito ad aprire la porta seriale";
        somethingwrong= true;
        return -1;
    }
}

void MainWindow::on_connect_novasom_pushButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Please connect NOVAsom to the workstation with RS232 cable.");
    msgBox.setWindowTitle("Checking NOVAsom.");
    msgBox.exec();
    disablepanel();
    int i =0;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
         qDebug() << "Name : " << info.portName();
         qDebug() << "Description : " << info.description();
         qDebug() << "Manufacturer: " << info.manufacturer();
         if(initPort(i,info)!=-1){
               qDebug () << "ho aperto la porta: "  << i;
               i++;
         }else{
            qDebug () << "non sono riuscito ad aprire la porta " << i;
         }
    }

    if(i==0){
        QMessageBox msgBox1;
            msgBox1.setText("Unable to open SerialPort.\nNo NOVAsom found.");
            msgBox1.setWindowTitle("Checking NOVAsom.");
            msgBox1.exec();
        update_status_bar("No NOVAsom found!");

        return;
    }


    //ui->console_textBrowser->setEnabled(true);
    //ui->console_textBrowser->clear();
    ui->connect_novasom_pushButton->setEnabled(false);
    ui->disconnect_novasom_pushButton->setEnabled(true);
    ui->runtest_pushButton->setEnabled(true);

    qDebug () << "trovati " << i << " NOVAsom.";
    QMessageBox msgBox2;
    QString msg;
    char ms[5];
    itoa(i,ms,10);
    msg.append(ms);
    msg.append(" Novasom found.");
    msgBox2.setText(msg);
    msgBox2.setWindowTitle("Checking NOVAsom.");
    msgBox2.exec();
    connectedNOVAsom=i;
    ui->line_comboBox->setEnabled(false);
    ui->model_comboBox->setEnabled(false);

    QString messaggio= "NOVAsom trovati : ";
    char ciao[10];
    itoa(i,ciao,10);

    update_status_bar(messaggio+ciao);
    msgBox.setText("Please insert microSD card into NOVAsom, then power on the boards and wait splash screen.");
    msgBox.setWindowTitle("Checking NOVAsom.");
    msgBox.exec();
 return;
}

void MainWindow::on_disconnect_novasom_pushButton_clicked()
{
    int j=0;
    for(j=0;j<connectedNOVAsom;j++) {
       if(serial[j].isOpen()==true){
           qDebug () << "chiusura in corso della porta " << j << ".";
           serial[j].close();
           if (serial[j].isOpen()==false){
               qDebug () << "chiusura porta " << j << " eseguita correttamente.";
           }else{
               qDebug () << "impossibile chiudere porta " << j;
           }

       }else{
           qDebug () << "Porta " << j << " gia chiusa; qualcosa che non va??(probabile)";
       }
    }
    if (j==connectedNOVAsom){
        qDebug () << "tutte le porte sono state chiuse correttamente.";
        connectedNOVAsom=0;
        ui->line_comboBox->setEnabled(true);
        ui->model_comboBox->setEnabled(true);
        ui->connect_novasom_pushButton->setEnabled(true);
        ui->disconnect_novasom_pushButton->setEnabled(false);
        ui->runtest_pushButton->setEnabled(false);
        ui->logout_pushButton->setEnabled(true);


    }else{
                     qDebug () << "non sono riuscito a chiudere tutte le porte. Riprovare!";
    }
    savelastsetting();
    if(superuser){
        enablepanel();
    }
    return;
}

void MainWindow::on_save_configuration_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Configuration File"), "D:/QtProjects/",tr("Configuration Files (*.ini)"));

    if (fileName.isEmpty())
    {
        qDebug() << "fileName is empty";
        return;
    }
    else
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            return;
        }
        QTextStream out(&file);
        out << QString("[CONFIG]\r\n");
        if ( ui->audio_checkBox->isChecked() )
            out << QString("audio_checkBox=true\r\n");
        else
            out << QString("audio_checkBox=false\r\n");
        if ( ui->bt_checkBox->isChecked() )
            out << QString("bt_checkBox=true\r\n");
        else
            out << QString("bt_checkBox=false\r\n");
        if ( ui->console_checkBox->isChecked() )
            out << QString("console_checkBox=true\r\n");
        else
            out << QString("console_checkBox=false\r\n");
        if ( ui->cpu_checkBox->isChecked() )
            out << QString("cpu_checkBox=true\r\n");
        else
            out << QString("cpu_checkBox=false\r\n");
        if ( ui->eeprom_checkBox->isChecked() )
            out << QString("eeprom_checkBox=true\r\n");
        else
            out << QString("eeprom_checkBox=false\r\n");
        if ( ui->eMMC_checkBox->isChecked() )
            out << QString("eMMC_checkBox=true\r\n");
        else
            out << QString("eMMC_checkBox=false\r\n");
        if ( ui->eth_checkBox->isChecked() )
            out << QString("eth_checkBox=true\r\n");
        else
            out << QString("eth_checkBox=false\r\n");
        if ( ui->hdmi_checkBox->isChecked() )
            out << QString("hdmi_checkBox=true\r\n");
        else
            out << QString("hdmi_checkBox=false\r\n");
        if ( ui->ram_checkBox->isChecked() )
            out << QString("ram_checkBox=true\r\n");
        else
            out << QString("ram_checkBox=false\r\n");
        if ( ui->rtc_checkBox->isChecked() )
            out << QString("rtc_checkBox=true\r\n");
        else
            out << QString("rtc_checkBox=false\r\n");
        if ( ui->sdcard_checkBox->isChecked() )
            out << QString("sdcard_checkBox=true\r\n");
        else
            out << QString("sdcard_checkBox=false\r\n");
        if ( ui->sensors_checkBox->isChecked() )
            out << QString("sensors_checkBox=true\r\n");
        else
            out << QString("sensors_checkBox=false\r\n");
        if ( ui->touch_checkBox->isChecked() )
            out << QString("touch_checkBox=true\r\n");
        else
            out << QString("touch_checkBox=false\r\n");
        if ( ui->usb_checkBox->isChecked() )
            out << QString("usb_checkBox=true\r\n");
        else
            out << QString("usb_checkBox=false\r\n");
        if ( ui->wifi_checkBox->isChecked() )
            out << QString("wifi_checkBox=true\r\n");
        else
            out << QString("wifi_checkBox=false\r\n");
        file.close();
    }
}

void MainWindow::on_load_configuration_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load Configuration File"), "D:/QtProjects/NOVATest",tr("Configuration Files (*.ini)"));
    if (fileName.isEmpty())
    {
        qDebug() << "fileName is empty";
        return;
    }
    else
    {
        qDebug() << "fileName :"+fileName;

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            return;
        }
        QString strKeyFunc("CONFIG/");
        QSettings * settings = 0;

            QFileInfo fi(fileName);
            settings = new QSettings( fileName, QSettings::IniFormat );
            if ( getvalue(strKeyFunc, settings , "audio_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->audio_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->audio_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "bt_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->bt_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->bt_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "console_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->console_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->console_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "cpu_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->cpu_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->cpu_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "eeprom_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->eeprom_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->eeprom_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "eMMC_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->eMMC_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->eMMC_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "eth_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->eth_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->eth_checkBox->setChecked(false);
                }

            if ( getvalue(strKeyFunc, settings , "hdmi_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->hdmi_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->hdmi_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "ram_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->ram_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->ram_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "rtc_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->rtc_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->rtc_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "sdcard_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->sdcard_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->sdcard_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "sensors_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->sensors_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->sensors_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "touch_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->touch_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->touch_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "usb_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->usb_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->usb_checkBox->setChecked(false);
                }
            if ( getvalue(strKeyFunc, settings , "wifi_checkBox") == "true")
                {
                    qDebug() << "true";
                    ui->wifi_checkBox->setChecked(true);
                }
                else
                {
                    qDebug() << "false";
                    ui->wifi_checkBox->setChecked(false);
                }
            file.close();
    }
}

void MainWindow::on_logout_pushButton_clicked()
{
    if(connectedNOVAsom>0){
        on_disconnect_novasom_pushButton_clicked();
    }
    if(superuser==true){
        superuser=false;
        ui->save_configuration_pushButton->hide();
        ui->load_configuration_pushButton->hide();
        disablepanel();
    }
    if(user.isNull()==false && psw.isNull()==false){
        user.clear();
        psw.clear();
    }
    ui->logIn_pushButton->setEnabled(true);
    ui->logout_pushButton->setEnabled(false);
    ui->line_comboBox->setEnabled(false);
    ui->model_comboBox->setEnabled(false);
    ui->connect_novasom_pushButton->setEnabled(false);
}

void MainWindow::on_runtest_pushButton_clicked()
{
    //ui->testFail_checkBox->setEnabled(true);
    //ui->testpass_checkBox->setEnabled(true);
    ui->PM_checkBox->setEnabled(true);
    ui->RM_checkBox->setEnabled(true);
    ui->runtest_pushButton->setEnabled(false);
    ui->disconnect_novasom_pushButton->setEnabled(false);
    ui->logout_pushButton->setEnabled(false);

    QEventLoop loop;

   for (int i=0;i<connectedNOVAsom;i++){
       if (serial[i].isOpen()){
           connect(ui->WriteE2prom_pushButton,&QPushButton::pressed,&loop,&QEventLoop::quit);
           serialport=&serial[i];
           testingNovasom(serialport);
           //checkResult();
           ui->SendResult_PushButton->setEnabled(true);
           ui->WriteE2prom_pushButton->setEnabled(false);
           loop.exec();
       }else qDebug()<<" la porta e' chiusa!";
   }

   QMessageBox msgBox;
       msgBox.setText("Test complete!");
       msgBox.setWindowTitle("Test complete!");
       msgBox.exec();

   ui->SendResult_PushButton->setEnabled(false);
   ui->WriteE2prom_pushButton->setEnabled(false);
   ui->pushButton->setEnabled(false);
   ui->disconnect_novasom_pushButton->setEnabled(true);

}


void MainWindow::generateParameters(){

    QString filename,pn,lotto;
    filename=(ConfigDir+"/PN.ini");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "[ERRORE]Non sono riuscito ad aprire "+ filename;
    }else{
        qDebug() << "File aperto correttamente";
        QString strKeyFunc("PN/");
        QSettings * settings = 0;
        settings = new QSettings( filename, QSettings::IniFormat );
        qDebug() << ui->model_comboBox->currentText();
        pn = getvalue(strKeyFunc, settings , ui->model_comboBox->currentText());
        qDebug() << pn;
    }
    file.close();
    //ui->PN_textBrowser->setText(pn);
    ui->PN_lineEdit->setText(pn);
    int week,year;
    char buffer[5];
    QDate date;
    if(date.currentDate().isValid()){
        week=date.currentDate().weekNumber();
        year=date.currentDate().year();
        qDebug() << week;
        qDebug() << year;
        if(ui->RM_checkBox->isChecked()){
            lotto.append("B");
        }else{
            lotto.append("A");
        }
        itoa(week,buffer,10);
        lotto.append(buffer);
        //buffer=NULL;
        lotto.append("-");
        itoa(year,buffer,10);
        lotto.append(buffer);
        qDebug() << lotto;
        //ui->LOTTO_textBrowser->setText(lotto);
        ui->Lotto_lineEdit->setText(lotto);
       }else{
        //non ho data!! sono fottuto!!
    }
    if(ui->RM_checkBox->isChecked()){
        filename=logDir+pn+"_"+lotto+".ini";
        QFile file2(filename);
        QByteArray riga;
        char serialStr[7], buffer[7];
        int seriale, len,i;
        if(file2.exists()){
            //continua con l'ultimo seriale;
            file2.open(QIODevice::ReadOnly | QIODevice::Text);
            riga=file2.readAll();
            file2.close();
            seriale=atoi(riga);
            seriale=seriale+1;
            itoa(seriale,serialStr,10);
            len=strlen(serialStr);
            if(len<6){
                for(i=0;i<6;i++){
                    serialStr[i]='\0';
                }
                for(i=0;i<6-len;i++){

                    serialStr[i]='0';
                }

                strcat(serialStr,itoa(seriale,buffer,10));
            }
            file2.open(QIODevice::WriteOnly | QIODevice::Text);
            file2.write(serialStr);
            file2.close();
        }else{
            //crea file e ricomincia seriale
            strcpy(serialStr,"000000");
            file2.open(QIODevice::WriteOnly | QIODevice::Text);
            file2.write(serialStr);
            file2.close();
        }
        //ui->SERIALE_textBrowser->setText(serialStr);
        ui->seriale_lineEdit->setText(serialStr);



    }else{
        //ProgressiveMODE
        //apri file, leggi ultimo seriale scritto, prendi successivo!!
        filename=logDir+pn+".ini";
        QFile file3(filename);
        char serialStr[7];
        QByteArray riga;
        char buffer[7];
        int seriale, len,i;
        if(file3.exists()){
            //il file esiste, leggi ultimo seriale, incrementa, scrivilo
            file3.open(QIODevice::ReadOnly | QIODevice::Text);
            riga=file3.readAll();
            file3.close();
            seriale=atoi(riga);
            seriale=seriale+1;
            itoa(seriale,serialStr,10);
            len=strlen(serialStr);

            if(len<6){
                for(i=0;i<6;i++){
                    serialStr[i]='\0';
                }
                for(i=0;i<6-len;i++){

                    serialStr[i]='0';
                }

                strcat(serialStr,itoa(seriale,buffer,10));
            }
            file3.open(QIODevice::WriteOnly | QIODevice::Text);
            file3.write(serialStr);
            file3.close();
        }else{

            //crea file!!
            strcpy(serialStr,"000001");
            file3.open(QIODevice::WriteOnly | QIODevice::Text);
            file3.write(serialStr);
            file3.close();
        }
        //ui->SERIALE_textBrowser->setText(serialStr);
        ui->seriale_lineEdit->setText(serialStr);
    }

    //MAC

    char stdstr[20];
    filename=MACdir+pn+".ini";
    QFile file4(filename);
    QByteArray line;
    QString mac;
    int macseriale;
    int l;

    if(file4.exists()){
                //il file esiste, leggi ultimo seriale, incrementa, scrivilo
                clearstdstr(stdstr);
                file4.open(QIODevice::ReadWrite | QIODevice::Text);
                line=file4.readAll();
                macseriale=atoi(line);
                line.clear();
                //itoa(macseriale,stdstr,10);

            }else{

                //crea file!!
                macseriale=-1;
                line.clear();
                clearstdstr(stdstr);
                //itoa(macseriale,stdstr,10);
                //file4.open(QIODevice::ReadWrite | QIODevice::Text);
                //file4.write(stdstr);
            }
    file4.close();

    macseriale++;
    itoa(macseriale,stdstr,10);

    if(!file4.isOpen()){
        file4.open(QIODevice::ReadWrite | QIODevice::Text);
    }

    file4.write(stdstr);
    clearstdstr(stdstr);

    itoa(macseriale,stdstr,16);

    file4.close();

    mac="00:4e:";
    mac.append(pn[11]);
    mac.append(pn[12]);
    mac.append(':');
    mac.append(pn[15]);

    l=strlen(stdstr);



    if(l<5){
        mac.append("0:");
        if(l<4){
            mac.append('0');
            if(l<3){

                mac.append("0:");
                if(l<2){

                    mac.append('0');
                    mac.append(stdstr[0]);
                }else{
                    mac.append(stdstr[0]);
                    mac.append(stdstr[1]);
                }
            }else{

                mac.append(stdstr[0]);
                mac.append(':');
                mac.append(stdstr[1]);
                mac.append(stdstr[2]);
            }
        }else{
            mac.append(stdstr[0]);
            mac.append(stdstr[1]);
            mac.append(':');
            mac.append(stdstr[2]);
            mac.append(stdstr[3]);
        }
    }else{
        //e se è maggiore di 5?? so cazzi!!
        mac.append(stdstr[0]);
        mac.append(':');
        mac.append(stdstr[1]);
        mac.append(stdstr[2]);
        mac.append(':');
        mac.append(stdstr[3]);
        mac.append(stdstr[4]);
    }





    //ui->MAC_textBrowser->setText(mac);
    ui->MAC_lineEdit->setText(mac);

}

void MainWindow::readSeriale(QSerialPort *serialp){//inutileperil momento!


    QByteArray data ;
    serialp->waitForBytesWritten(2500);

    while ( serialp->waitForReadyRead(500) == 1 )
    {
        data = serialp->readAll();

    }
    qDebug() << data.size();
    qDebug() << data;

    return;
}

int MainWindow::testingNovasom(QSerialPort *serialp){





    QByteArray buffer, bufferseriale;
    QByteArray line;
    QDate date;
    QTime time;
    QString filename;

    line.append("echo \"REFERENCE_SERVER="+ip+"\" > /etc/sysconfig/system_vars\n");
    line.append(". /etc/sysconfig/system_vars\n");
    //line
    serialp->write(line);

    //filename=ui->line_comboBox->currentText()+"_"+ui->model_comboBox->currentText()+date.currentDate().toString("_dd_MM_yyyy")+time.currentTime().toString("_hh_mm_ss_zzz");
    filename=ui->line_comboBox->currentText()+"_"+ui->model_comboBox->currentText();
    qDebug() << filename;
    if (filename.isEmpty()){
        qDebug() << "fileName is empty";
        return -1;
        }
    QFile file(xampdirectory+filename+".sh");
    QFile file1(xampdirectory+"WebServerResult.txt");

    if(file1.exists()){
        if(file1.remove()){
            qDebug()<< "File risultato eliminato con successo!";
        }else
        qDebug() << "non sono riuscito ad eliminare il file del precedente risultato!problema!!";


    }else{
        qDebug()<< "Prima volta questo test!";

    }

    if(file.exists()){
       //do nothing



    }else{

        //il file non esiste!lo compilo!

        if (!file.open(QIODevice::ReadWrite|QIODevice::Text)){
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            return -1;
           }

        file.write("#!/bin/sh\n\n#");
        buffer=filename.toUtf8();
        file.write(buffer);
        file.write("\n");
        buffer="#collaudatore: "+user.toUtf8();
        file.write(buffer);

        file.write("\n\n\n");

        //serialp->write("touch report.txt\n");
        //qDebug() << "GNOPoooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo";
        //qDebug() << buffer;
        serialp->write("echo "+buffer+" >> report.txt\n");
        //serialp->write("\n\n >> report.txt\n");

        //file.write("echo \"Informazioni sul sistema:\"\necho -e \"hardware:\t$(uname -m)\"\necho -e \"OS:\t$(uname -o)\"\n");
        file.write("ERROR=\"\"\n");
        if(ui->audio_checkBox->isChecked()){
            //test sull'audio
            //file.write("echo \"Checking Audio\"\nAUDIO=`dmesg | grep asoc | wc -l`\nif [ -n \"$AUDIO\" ]\n\tthen\n\techo \"Audio found!\"\n\techo \"eseguire test audio? [Y/N]\"\n\tread ANSWER\n\tif [ \"$ANSWER\" = \"Y\" ]\n\t\tthen\n\t\techo \"Plug in device\"\n\t\techo \"premere un tasto per continuare...\"\n\t\tread\n\t\tgplay test_audio.mp3\n\t\tfi\n\telse\n\t\techo \"Audio Not Found!\"\n\t\tunset AUDIO\n\tfi\n\n\n");
            file.write("AUDIO=`dmesg | grep asoc | wc -l`\nif [ -n \"$AUDIO\" ]\nthen\necho Audio OK >> report.txt\nelse\nunset AUDIO\necho Audio KO >> report.txt\nfi\n");
            file.write("[ \"$AUDIO\" == \"\" ] && ERROR=\"Audio Error\"\n");
           }
        if(ui->bt_checkBox->isChecked()){
            //test sul bluetooth
           }
        if(ui->console_checkBox->isChecked()){

            //test su console
           }
        if(ui->cpu_checkBox->isChecked()){
            //test su cpu
            //file.write("echo Checking CPU...\nNCORE=`cat /proc/cpuinfo | grep processor | wc -l`\necho $NCORE\nCPU=`dmesg | grep processors | grep $NCORE`\necho $CPU\nif [ -n \"$CPU\" ]\nthen\necho \"CPU OK\"\nelse\necho \"CPU kooooooo\"\nunset CPU\nfi\n");
            file.write("NCORE=`cat /proc/cpuinfo | grep processor | wc -l`\nCPU=`dmesg | grep processors | grep $NCORE`\nif [ -n \"$CPU\" ]\nthen\necho CPU OK >> report.txt\nelse\n\nunset CPU\n echo CPU KO >> report.txt\nfi\n");
            file.write("[ \"$CPU\" == \"\" ] && ERROR=\"CPU Error\"\n");
           }
        if(ui->eeprom_checkBox->isChecked()){
            //test su eeprom
        }
        if(ui->eMMC_checkBox->isChecked()){
            //test sulla eMMC
            //file.write("echo Checking eMMC...\ntime sh -c \"dd if=/dev/zero of=/dev/mmcblk1 bs=4K count=1 && sync\"\nif [ $? -ge 0 ]\nthen\necho \"EMMC OK\"\nEMMC=\"ok\"\nelse\necho \"EMMC KO\"\nunset EMMC\nfi\n");
            file.write("time sh -c \"dd if=/dev/zero of=/dev/mmcblk1 bs=4K count=1 && sync\" &> /dev/null\nif [ $? -ge 0 ]\nthen\nEMMC=\"ok\"\necho eMMC ok >> report.txt\nelse\nunset EMMC\necho eMMC KO >> report.txt\nfi\n");
            file.write("[ \"$EMMC\" == \"\" ] && ERROR=\"eMMC Error\"\n");
           }
        if(ui->eth_checkBox->isChecked()){
            //test sull'ethernet
            //file.write("echo Checking Ethernet...\nETH=`dmesg | grep eth0`\nif [ -n \"$ETH\" ]\nthen\necho \"Ethernet found\"\nelse\necho \"Ethernet KO\"\nunset ETH\nfi\n");
            file.write("ETH=`dmesg | grep eth0`\nif [ -n \"$ETH\" ]\nthen\necho Ethernet OK >> report.txt\nelse\nunset ETH\necho Ethernet KO >> report.txt\nfi\n");
            file.write("[ \"$ETH\" == \"\" ] && ERROR=\"Ethernet Error\"\n");
           }
        if(ui->hdmi_checkBox->isChecked()){
            //test sull'hdmi
           }
        if(ui->pcie_checkBox->isChecked()){
            //test sul connettore PCIe
        }
        if(ui->ram_checkBox->isChecked()){
            //test sulla RAM
            //file.write("echo Checking RAM\nRAM=`cat /proc/meminfo | grep MemTotal `\nif [ -n \"$RAM\" ]\nthen\necho \"$RAM\"\nelse\necho \"RAM KO\"\nunset RAM\nfi\n");
            file.write("RAM=`cat /proc/meminfo | grep MemTotal `\nif [ -n \"$RAM\" ]\nthen\necho RAM OK >> report.txt\nelse\nunset RAM\necho RAM KO >> report.txt\nfi\n");
            file.write("[ \"$RAM\" == \"\" ] && ERROR=\"RAM Error\"\n");
        }
        if(ui->rtc_checkBox->isChecked()){
            //test sull'rtc
            //file.write("echo Checking RTC...\nRTC=`dmesg | grep rtc | grep setting `\nif [ -n \"$RTC\" ]\nthen\necho \"RTC ok\"\nelse\necho \"RTC KO\"\nunset RTC\nfi\n");
            file.write("RTC=`dmesg | grep rtc | grep setting `\nif [ -n \"$RTC\" ]\nthen\necho RTC OK >> report.txt\nelse\nunset RTC\n echo RTC KO >> report.txt\nfi\n");
            file.write("[ \"$RTC\" == \"\" ] && ERROR=\"RTC Error\"\n");
           }
        if(ui->sata_checkBox->isChecked()){
            //test sulla porta sata
           }
        if(ui->sdcard_checkBox->isChecked()){
            //test su sdcard
           }
        if(ui->sensors_checkBox->isChecked()){
            //test sui sensori
           }
        if(ui->touch_checkBox->isChecked()){
            //test sul touch
           }
        if(ui->usb_checkBox->isChecked()){
            //test sulle porte USB
            //file.write("echo \"Checking USB...\"\nUSB=`ls /sys/bus/usb/devices | grep usb | wc -l`\nif [ $USB -eq `dmesg | grep \"1 port\" | wc -l` ]\n\tthen\n\t\techo \"Trovate $USB porte usb\"\n\telse\n\t\techo \" Problema su porta usb...\"\n\t\tunset USB\n\tfi\n\n\n");
            //serialp->write("echo \"echo \"Checking USB...\"\nUSB=`ls /sys/bus/usb/devices | grep usb | wc -l`\nif [ $USB -eq `dmesg | grep \"1 port\" | wc -l` ]\n\tthen\n\t\techo \"Trovate $USB porte usb\"\n\telse\n\t\techo \" Problema su porta usb...\"\n\t\tunset USB\n\tfi\n\n\n \" >> test.sh\n");
           }
        if(ui->wifi_checkBox->isChecked()){
            //test sul wifi
            //file.write("echo Checking WiFi...\nWiFi=`dmesg | grep wifi | grep Done`\nif [ -n \"$WiFi\" ]\nthen\necho \"WiFi found\"\nelse\necho \"WiFi not found\"\nunset WiFi\nfi\n");
            file.write("WiFi=`dmesg | grep wifi | grep Done`\nif [ -n \"$WiFi\" ]\nthen\necho WIFI OK >> report.txt\nelse\nunset WiFi\necho WIFI KO >> report.txt\nfi\n");
            file.write("[ \"$WiFi\" == \"\" ] && ERROR=\"WiFi Error\"\n");
           }
        file.write("if [ \"$ERROR\" == \"\" ]; then\nRES=\"OK\"\nelse\nRES=\"KO\"\nfi\n");
        //file.write("rm b*\nwget http://192.168.0.83/b.php?argument1=\$RES\n");
        file.write("rm b*\nget_exec b.php?argument1=\$RES\n");
        //QString tmp;
        //QByteArray tmp2;
        //tmp2.append("rm b*\nwget http://");
        //tmp2.append("$REFERENCE_SERVER");
        //tmp2.append("/b.php?argument1=\$RES\n");
        //qDebug() << tmp;

        //tmp2=tmp;
        //qDebug()<< tmp2;

        //file.write(tmp2);


    }


   //file.write("rm b*\nwget http://192.168.0.83/b.php?argument1=\$RES\n");
    serialp->write("\ncd /tmp;rm test.sh\n");
    QString tmp;
    //tmp="wget http://192.168.0.83/"+ filename +".sh\n";
    tmp="wget http://"+ip+"/"+ filename +".sh\n";
    buffer.clear();
    buffer=tmp.toUtf8();
    serialp->write(buffer);
    serialp->write("\n");
    tmp.clear();
    buffer.clear();
    tmp="mv ./"+filename+".sh ./test.sh\n";
    buffer=tmp.toUtf8();
    serialp->write(buffer);
    serialp->write("\n");
    serialp->write("dos2unix ./test.sh\nchmod 777 test.sh\n");

    //serialp->write("wget http://192.168.0.83/test.sh\ndos2unix ./test.sh\nchmod 777 test.sh\n");
    serialp->write("touch report.txt\n");
    serialp->write("./test.sh >> report.txt\n");


    return 0;
}

int MainWindow:: checkResult(){
    QByteArray linebytearray;
    //Sleep(10000);
    QFile file1(xampdirectory+"WebServerResult.txt");
    //do{}while(!file1.exists());
    if(file1.exists()){
        //il risultato del test esiste!!

        if (!file1.open(QIODevice::ReadWrite|QIODevice::Text)){
            QMessageBox::information(this, tr("Unable to open file"),file1.errorString());
            return -1;
           }
       linebytearray=file1.readAll();
       if(linebytearray.toStdString()=="OK"){

           QMessageBox msgBox;
               msgBox.setText("Test Pass.\nCheck video...");
               msgBox.setWindowTitle("Check result...");
               msgBox.exec();

           ui->testpass_checkBox->setChecked(true);
           ui->testFail_checkBox->setChecked(false);
           //ui->testpass_checkBox->setCheckable(true);
           //ui->testFail_checkBox->setCheckable(true);

           qDebug() << "TEST PASSSSSSSSSSSSSSSSSSSSSS!";
           return 0;
       }else if (linebytearray.toStdString()=="KO") {
           //Test Fail!!
           QMessageBox msgBox1;
               msgBox1.setText("Test Fail.");
               msgBox1.setWindowTitle("Check result...");
               msgBox1.exec();
           ui->testpass_checkBox->setChecked(false);
           ui->testFail_checkBox->setChecked(true);
           qDebug() << "TEST FAILLLLLLLLLLLL!";
           return 1;
       }else{
           //test ????
           qDebug() << "TEST booooooooooooooooo!";

           QMessageBox msgBox2;
               msgBox2.setText("Problema sconosciuto...\nFile risultato non corretto.Ripetere il test...");
               msgBox2.setWindowTitle("???");
               msgBox2.exec();
           ui->testpass_checkBox->setChecked(false);
           ui->testFail_checkBox->setChecked(false);
            return -1;
       }

    }else{
        //il risultato del test non esiste!!!Problemi sul webserver???
        QMessageBox msgBox3;
            msgBox3.setText("WebServer problem...No network?? ");
            msgBox3.setWindowTitle("Check result...");
            msgBox3.exec();
        qDebug()<<"Risultato non trovato...Problema di rete??";
        return -1;
    }





}


void MainWindow:: savelastsetting(){

    QString filename=userConfigDir+user+".ini";
     if (filename.isEmpty())
     {
         qDebug() << "fileName is empty";
         return;
     }
     QFile file(filename);
     qDebug ()<< filename << "sto cercando di aprire";
     if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        qDebug() << "Non sono riuscito ad aprire il file!";
        return;
         }
     QTextStream out(&file);
     out << ui->line_comboBox->currentText().append("\n");
     //out << " ";
     out << ui->model_comboBox->currentText();
     file.close();
    return;
}

int MainWindow:: loadpreviousconfiguration(){
     QString filename=userConfigDir+user+".ini";
     if (filename.isEmpty())
     {
         qDebug() << "fileName is empty";
         return -1;
     }
     QFile file(filename);
     if(!file.exists()) return -1;
     qDebug ()<< filename << "sto cercando di aprire";
     if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug() << "Non sono riuscito ad aprire il file!";
        return -1;
         }
     QString line;
     QTextStream in(&file);
     line = in.readLine();
     //qDebug () << "ciaooooooooooooooooooooooooo " << line;
     ui->line_comboBox->setCurrentText(line);
     line = in.readLine();
     //qDebug () << "ciaooooooooooooooooooooooooo " << line;
     ui->model_comboBox->setCurrentText(line);

     file.close();
     return 1;

}

int MainWindow:: createPDF(QString filename){

    qDebug() << "sto per creare il pdf!";

    QFile file(directory+filename+".txt");
    bool f;
    f=file.exists();
    qDebug() << f;
    qDebug() << directory+filename+".txt";

    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::information(this, tr("Unable to open file"),file.errorString());
        return -1;
    }

    qDebug() << "ho aperto il file";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(directory+filename+"report.pdf");

    QByteArray bit;


    //bit= file.readAll();
    //qDebug()<< bit;

    QPainter painter;
    painter.begin(&printer);
    QString line;
    int x,y;
    x=20;
    y=30;
    qDebug() << "ho aperto il file";

    while (!file.atEnd()) {
           line = file.readLine();
           qDebug() << "la linea e':";
           qDebug() << line;
           painter.drawText(x,y,line);
           line.clear();
           //x=x+20;
           y=y+120;
       }
    //printer.newPage();
    file.close();
    painter.end();
    return 0;
}

int MainWindow::generateReport(QString filename){


    //serialport->write("ftpput --username novasis --password novasis 192.168.0.83 report.txt \n\n");
    QByteArray tmp;
    tmp.append("ftpput --username novasis --password novasis ");
    tmp.append(ip);
    tmp.append(" report.txt \n\n");
    serialport->write(tmp);
    QFile report(directory+filename+".txt");

    if(!report.open(QIODevice::ReadWrite|QIODevice::Text)){
       qDebug() << "Non sono riuscito a creare il file1!";
       return -1;
        }


    QFile original(filezilla + "report.txt");
    if(!original.open(QIODevice::ReadOnly|QIODevice::Text)){
       qDebug() << "Non sono riuscito ad aprire il file2!";
       return -1;
        }
    QByteArray rep;

    rep=original.readAll();
    //qDebug() << "MICHELEUBEST!";
    qDebug() << rep;

    report.write("#");
    report.write(filename.toUtf8());
    report.write("\n");
    report.write("#");
    report.write(user.toUtf8());
    report.write("\n");







    if(ui->testpass_checkBox->isChecked()){

        report.write("#TEST PASS gnooooo!\n");
        report.write("\nPN: ");
        report.write(ui->PN_lineEdit->text().toUtf8());
        report.write("\nSeriale: ");
        report.write(ui->seriale_lineEdit->text().toUtf8());
        report.write("\nLotto: ");
        report.write(ui->Lotto_lineEdit->text().toUtf8());
        report.write("\nMAC ADDRESS: ");
        report.write(ui->MAC_lineEdit->text().toUtf8());
        report.write("\n");

    }else{
        report.write("#TEST FAIL gnoooo!");
        report.write("\nPN: ");
        report.write(ui->PN_lineEdit->text().toUtf8());
        report.write("\nSeriale: ");
        report.write("XXX");
        report.write("\nLotto: ");
        report.write("XXX");
        report.write("\nMAC ADDRESS: ");
        report.write("00:ff:00:ff:00:ff");
        report.write("\n");
    }




    report.write(rep);
    report.close();
    //serialport->write("ftpput --username novasis --password novasis 192.168.0.83 report.txt \n\n");
    qDebug() << "ci siamo!!!";
    createPDF(filename);
    qDebug() << "ci siamo2";

    return 0;
}

void MainWindow::on_testpass_checkBox_clicked()
{
    if(ui->testpass_checkBox->isChecked()){
        ui->testFail_checkBox->setCheckState(Qt::Unchecked);
    }else{
        ui->testFail_checkBox->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_testFail_checkBox_clicked()
{
    if(ui->testFail_checkBox->isChecked()){
        ui->testpass_checkBox->setCheckState(Qt::Unchecked);
    }else{
        ui->testpass_checkBox->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_SendResult_PushButton_clicked()
{
    int k;

    //ui->testFail_checkBox->setEnabled(false);
    //ui->testpass_checkBox->setEnabled(false);


    k=checkResult();
    if(k==0){
        //test pass
        update_status_bar("TEST PASS!!");
        ui->SendResult_PushButton->setEnabled(false);
        //ui->WriteE2prom_pushButton->setEnabled(true);
        ui->pushButton->setEnabled(true);
        //generate Parameters!!!
        //qDebug() << ui->result_label->text().toInt();
        //generateParameters();

    }else if(k==1){
        //test fail
        update_status_bar("TEST FAIL!!");
        //ui->MAC_textBrowser->clear();
        ui->MAC_lineEdit->clear();
        //ui->PN_textBrowser->clear();
        ui->PN_lineEdit->clear();
        //ui->LOTTO_textBrowser->clear();
        ui->Lotto_lineEdit->clear();
        ui->SendResult_PushButton->setEnabled(false);
        ui->WriteE2prom_pushButton->setEnabled(true);
        ui->pushButton->setEnabled(true);

    }else{
        //errore!!
    }
    generateParameters();
    //questa è la generate parameter



}

void MainWindow::on_PM_checkBox_clicked()
{
    if(ui->PM_checkBox->isChecked()){
        ui->RM_checkBox->setCheckState(Qt::Unchecked);
    }else{
        ui->RM_checkBox->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_RM_checkBox_clicked()
{
    if(ui->RM_checkBox->isChecked()){
        ui->PM_checkBox->setCheckState(Qt::Unchecked);
    }else{
        ui->PM_checkBox->setCheckState(Qt::Checked);
    }
}
/*
void MainWindow::on_WriteE2prom_pushButton_clicked()
{
    //scrivi sulla Eproom
    if(ui->testpass_checkBox->isChecked()){//test pass!!
        //scrivi valori()


    }else{//test fail
        //valori fittizi!!brasa la eeprom!!


    }

    //genera report...
    qDebug() << "sto chiamando generate!!";
    generateReport();
    qDebug() << "finito generate!!";


    ui->SendResult_PushButton->setEnabled(true);
    ui->WriteE2prom_pushButton->hide();
    ui->testFail_checkBox->setEnabled(true);
    ui->testpass_checkBox->setEnabled(true);
}
*/



void MainWindow::on_pushButton_clicked()
{
    //ui->testpass_checkBox->setCheckable(false);
    //ui->testFail_checkBox->setCheckable(false);


    QString pn,mac,lotto,seriale;
    serialport->write("get_exec e2info\n");

    //if(ui->testpass_checkBox->isChecked()){
        pn=ui->PN_lineEdit->text();
        qDebug() << pn.toUtf8();
        seriale=ui->seriale_lineEdit->text();
        qDebug() << seriale.toUtf8();
        lotto=ui->Lotto_lineEdit->text();
        qDebug() << lotto.toUtf8();
        mac=ui->MAC_lineEdit->text();
        qDebug() << mac.toUtf8();
    //}else{
        /*
        pn=ui->PN_lineEdit->text();
        qDebug() << pn.toUtf8();
        seriale="XXXXXX";
        qDebug() << seriale.toUtf8();
        lotto="XX-XXXX";
        qDebug() << lotto.toUtf8();
        mac="ff:00:ff:00:ff:00";
        qDebug() << mac.toUtf8();
    }
    */

    //pn=ui->PN_textBrowser->text
    serialport->write("chmod 777 e2info\n");

    serialport->write("./e2info -P "+ pn.toUtf8()+"\n");
    serialport->write("./e2info -M "+ mac.toUtf8()+"\n");
    serialport->write("./e2info -S "+ seriale.toUtf8()+"\n");
    serialport->write("./e2info -L "+ lotto.toUtf8()+"\n");
    serialport->write("unix2dos report.txt ; ftpput --username novasis --password novasis 192.168.0.83 report.txt \n\n");
    QByteArray tmp;
    tmp.append("unix2dos report.txt ; ftpput --username novasis --password novasis ");
    tmp.append(ip);
    tmp.append(" report.txt \n\n ");
    serialport->write(tmp);

    qDebug() << "ci siamo!!!";

    QString titolo;

    titolo.append(ui->PN_lineEdit->text());
    titolo.append("_");
    titolo.append(ui->seriale_lineEdit->text());
    titolo.append("_");
    titolo.append(ui->Lotto_lineEdit->text());

    generateReport(titolo);//passagli un parametro che abbia senso!!!
    ui->WriteE2prom_pushButton->setEnabled(true);
    ui->pushButton->setEnabled(false);

}

void MainWindow::on_WriteE2prom_pushButton_clicked()
{

}
