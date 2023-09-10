#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "suseroperations.h"
#include "usrpswdialog.h"
#include <QDebug>
#include <QString>
#include <QDir>
#include <QProcess>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define QT_NO_DEBUG_OUTPUT
#define PG_USE_TFTP_ON_TARGET
#define PG_UPLOAD_REPORT_ON_FTP

using namespace std;

bool superuser = false;
int connectedNOVAsom=0;
QSerialPort *serialport;
QSerialPort serial[MAXPORT];

/*
QString userConfigDir = "C:/NOVATEST_SP/CONFIG/user/";
QString ConfigDir = "C:/NOVATEST_SP/CONFIG/CONF";
QString lan = "C:/NOVATEST_SP/CONFIG/xampp/htdocs/";
QString LogDir = "C:/NOVATEST_SP/CONFIG/log/";
QString MACdir = "C:/NOVATEST_SP/CONFIG/log/MAC/";
QString Fail ="C:/NOVATEST_SP/CONFIG/log/FAIL/";
QString directory = "C:/NOVATEST_SP/TESTED/";
QString labeltemplate= "C:/NOVATEST_SP/CONFIG/CONF/zebra_template.txt";
*/

QString userConfigDir = "/NOVAtestSMPU/CONFIG/user/";
QString ConfigDir = "/NOVAtestSMPU/CONFIG/CONF";
//QString lan = "/NOVAtestSMPU/CONFIG/xampp/htdocs/";
//var/www/html
QString lan = "/var/www/html/";
QString LogDir = "/NOVAtestSMPU/CONFIG/log/";
QString MACdir = "/NOVAtestSMPU/CONFIG/log/MAC/";
QString Fail ="/NOVAtestSMPU/CONFIG/log/FAIL/";
QString directory = "/NOVAtestSMPU/TESTED/";
QString labeltemplate= "/NOVAtestSMPU/CONFIG/CONF/zebra_template.txt";

char *my_itoa(int num, char *str)
{
        if(str == NULL)
        {
            return NULL;
        }
        sprintf(str, "%d", num);
        return str;
}

void clearstdstr( char *stringa){
    for(uint i=0;i<strlen(stringa);i++){
        stringa[i]='\0';
    }
    return;
}

QString getvalue(QString strKey, QSettings *settings , QString entry)
{
    return settings->value( strKey + entry, "r").toString();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->save_configuration_pushButton->hide();
    ui->load_configuration_pushButton->hide();
    ui->suser_operations_pushButton->hide();

    update_status_bar("Please LogIn...");
    setWindowTitle("NOVAtest Ver. 1.6.0");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getParameter(){
    QString param;

    param.append(ui->PN_lineEdit->text());
    param.append("_");
    param.append(ui->OC_lineEdit->text());
    param.append("_");
    param.append(ui->Lotto_lineEdit->text());
    param.append("_");
    param.append(ui->seriale_lineEdit->text());


    return param;
}

int MainWindow::update_status_bar(QString StatusBarContent){

    statusBar()->showMessage(StatusBarContent);
    return 0;
}

QString MainWindow::getUser()
{
    return user;
}

QString MainWindow::getUserChar(QString uname)
{
    if (uname==" ")
        return "A";
    else
        return uname.at(0);
}

QString MainWindow::getPsw()
{
    return psw;
}

void MainWindow::setUserPsw(QString utente, QString password)
{
    user=utente;
    psw=password;
    return;
}

void MainWindow::superUser(){
    ui->load_configuration_pushButton->show();
    ui->load_configuration_pushButton->setEnabled(true);
    ui->save_configuration_pushButton->show();
    ui->save_configuration_pushButton->setEnabled(true);
    ui->suser_operations_pushButton->show();
    ui->suser_operations_pushButton->setEnabled(true);

    superuser=true;
    logInSuccesfully();
    enablepanel();
    return;
}

QString MainWindow::get_net_name(){
    QProcess getNetName;
    QString bashCommand("ifconfig | grep flags | awk '{print $1}'");
    QString netNamesContent, firstNetName, bashError, netnamesPath = QDir::currentPath();

    netnamesPath.append("/netnames.txt");
    QFile netNamesFile(netnamesPath);

    getNetName.start("sh", QStringList()<<"-c"<<bashCommand.append(">").append(netnamesPath));
    getNetName.waitForFinished();
    bashError = (getNetName.readAllStandardError());

    if (netNamesFile.open(QIODevice::ReadOnly | QIODevice::Text) && bashError.isEmpty())
    {
        QTextStream netNamesStream(&netNamesFile);
        netNamesContent = netNamesStream.readAll();
        netNamesFile.close();
        netNamesFile.remove();
        firstNetName = netNamesContent.split(":").at(0);
    }
    else
        firstNetName = bashError;

    return firstNetName;
}

bool MainWindow:: compareMacs(QNetworkInterface iface)
{
    UsrPswDialog *m = qobject_cast<UsrPswDialog*>(this->parent());
    QString mymac = iface.hardwareAddress();
    QFile decrFile(decrCredDir);
    bool comparisonFlag = false;

    QString decrContent = m->get_file_content(encrCredDir, true);
    m->write_file_content(decrContent, decrCredDir, false);

    if (decrFile.exists())
    {
        QSettings macSettings(decrCredDir, QSettings::IniFormat);
        macSettings.beginGroup("TRUEMAC");
        const QStringList macKeys = macSettings.childKeys();
        foreach (const QString &macKey, macKeys)
        {
            QString loadedUser = macKey;
            QString loadedMac = macSettings.value(macKey).toString();

            //cout << "key: " << loadedUser.toStdString() << endl;
            //cout << "val: " << loadedMac.toStdString() << endl;
            //cout<< loadedMac.compare(mymac) <<endl;

            if (!loadedMac.compare(mymac))
            {
                comparisonFlag = true;
                decrFile.remove();
                break;
            }
            else
            {}

        }
        macSettings.endGroup();
    }
    else
        QMessageBox::information(this, "Error file reading", "File is empty or does not exist.");

    decrFile.remove();
    return comparisonFlag;
}

void MainWindow::logInSuccesfully(){
    QList<QNetworkInterface> entries = QNetworkInterface::allInterfaces();

    for (int i=0;i<entries.count();i++){
        QNetworkInterface iface= entries.at(i);
        //qDebug() << "CIAO PRIMO FOR------";
        //qDebug() << iface.name();
        //qDebug() << iface.hardwareAddress();
        //qDebug() << iface.hardwareAddress().compare("00:4E:00:01:00:05");
        if(iface.flags().testFlag(QNetworkInterface::IsUp) && compareMacs(iface)){
            //qDebug() << "TROVATO!";
            for( int j =0 ; j< iface.addressEntries().count();j++){
                //std::cout << iface.name().toUtf8().toStdString() << std::endl;
                //qDebug() << "secondociclo------";
                //qDebug() << j;
                if(iface.addressEntries().at(j).ip().protocol()==QAbstractSocket::IPv4Protocol){
                    //std::cout << iface.name().toUtf8().toStdString() << std::endl;
                    //if(strstr(iface.name().toUtf8(), get_net_name().toUtf8())){
                    if(compareMacs(iface)){
                        ip = iface.addressEntries().at(j).ip().toString();
                        //qDebug() << iface.addressEntries().at(j).ip().toString();
                        //qDebug() << iface.name();
                        //ip = iface.addressEntries().at(j).ip().toString();
                        //qDebug() << "vaccagialla";
                        //qDebug() << ip;
                        //qDebug() << iface.addressEntries().at(j).ip().protocol();
                    }else{//cout << get_net_name().toStdString() << endl;

                    }
                }
                //qDebug() << "------secondocicloend";
            }
        }
        //qDebug() << "------PRIMOCICLOEND";
    }

           if(ip==NULL){
                    //std::cout << "NOINTERNETCONNECTION";
                    //qDebug() << "NOINTERNETCONNECTION";
                    QMessageBox msgBoxwarning;
                        msgBoxwarning.setText("No internet connection found");
                        msgBoxwarning.setWindowTitle("Warning");
                        msgBoxwarning.setIcon(QMessageBox::Warning);
                        msgBoxwarning.exec();

                        update_status_bar("WARNING: no internet connection");

                }
    popolate_line_combobox();
    if (loadpreviousconfiguration()==1){ //non ottimizzato secondo me!!
        on_line_comboBox_activated(ui->line_comboBox->currentText());
        loadpreviousconfiguration();
        on_model_comboBox_activated(ui->model_comboBox->currentText());
    }
    ui->logIn_pushButton->setEnabled(false);
    ui->logout_pushButton->setEnabled(true);
    ui->line_comboBox->setEnabled(true);
}

void MainWindow::popolate_line_combobox(){

    QString filename;
    filename=(ConfigDir+"/line.ini");
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        //qDebug() << "[ERRORE]Non sono riuscito ad aprire linecombobox!";

    }else{
        //qDebug() << "File aperto correttamente";
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

void MainWindow::on_logIn_pushButton_clicked()
{
    UsrPswDialog *p = new UsrPswDialog(this);
    p->show();
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
        ui->suser_operations_pushButton->hide();
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
    ip="";

}

void MainWindow::on_disconnect_novasom_pushButton_clicked()
{
    int j=0;
    //cout << "Inizio Procedura disconnessione!!" << endl;
    for(j=0;j<connectedNOVAsom;j++) {
        //cout << "ciclo: "+ j << endl;
       if(serial[j].isOpen()==true){
           //cout << j + " è aperta!!" << endl;
           serial[j].close();
           //cout << j + " la sto chiudendo..." << endl;
           if (serial[j].isOpen()==false){
               //cout << j + "è stata chiusa!!" << endl;
           }else{
               //cout <<j +  " STILL OPEN" << endl;
           }
       }else{
           //cout << j + " Era già chiusa" << endl;
       }
    }
    if (j==connectedNOVAsom){
        //cout << "UltimoCiclo" << endl;
        connectedNOVAsom=0;
        ui->line_comboBox->setEnabled(true);
        ui->model_comboBox->setEnabled(true);
        ui->connect_novasom_pushButton->setEnabled(true);
        ui->disconnect_novasom_pushButton->setEnabled(false);
        ui->runtest_pushButton->setEnabled(false);
        ui->logout_pushButton->setEnabled(true);
    }else{
        //cout << "NON DOVREI ESSERE QUI!!" << endl;
    }
    //cout << "FuoriDaiCicli" << endl;
    savelastsetting();
    //cout << "Salvato modifiche!!" << endl;
    if(superuser){
        enablepanel();
    }
    //cout << "Sto uscendo!!" << endl;
    return;
}

void MainWindow:: savelastsetting(){

    QString filename=userConfigDir+user+".ini";
     if (filename.isEmpty())
     {
         return;
     }
     QFile file(filename);
     if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        return;
         }
     QTextStream out(&file);
     out << ui->line_comboBox->currentText().append("\n");
     out << ui->model_comboBox->currentText();
     file.close();
    return;
}

void MainWindow::on_line_comboBox_activated(const QString &arg1)
{
    QString filename=(ConfigDir+"/"+arg1+".ini");
     QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
    }else{
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

void MainWindow::on_model_comboBox_activated(const QString &arg1)
{
    QString filename;
    filename=(ConfigDir+"/"+arg1+".ini");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        //qDebug() << "[ERRORE]Non sono riuscito ad aprire "+ filename;
        update_status_bar("[ERROR] No configuration file...Please contact us");
    }else{
        //qDebug() << "File aperto correttamente";
        QString strKeyFunc("CONFIG/");
        QSettings * settings = 0;
        settings = new QSettings( filename, QSettings::IniFormat );

        QList<QCheckBox *> checkBoxes = this->findChildren<QCheckBox *>();

        for (int i=0; i<checkBoxes.count(); ++i)
        {
            QString checkBoxName = checkBoxes.at(i)->objectName();
            if (getvalue(strKeyFunc, settings , checkBoxName) == "true")
            {
                //qDebug() << "true";
                checkBoxes.at(i)->setChecked(true);
            }
            else
            {
                //qDebug() << "false";
                checkBoxes.at(i)->setChecked(false);
            }

        }
/*
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
        if ( getvalue(strKeyFunc, settings , "seqDebug() << "false";nsors_checkBox") == "true")
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
            }*/
            file.close();
    }
    ui->connect_novasom_pushButton->setEnabled(true);
    update_status_bar("Model selected: : "+ arg1);
    file.close();
}

int MainWindow:: loadpreviousconfiguration(){
     QString filename=userConfigDir+user+".ini";
     if (filename.isEmpty())
     {
         //qDebug() << "fileName is empty";
         return -1;
     }
     QFile file(filename);
     if(!file.exists()) return -1;
     //qDebug ()<< filename << "sto cercando di aprire";
     if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        //qDebug() << "Non sono riuscito ad aprire il file!";
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
/*
vi /etc/udev/rules.d/50-myusb.rules
KERNEL=="ttyUSB[0-9]*",MODE="0666"
KERNEL=="ttyACM[0-9]*",MODE="0666"
*/
int initPort(int num,QSerialPortInfo info){
    serial[num].setPort(info);
    serialport=&serial[num];
    if (serial[num].open(QIODevice::ReadWrite))
    {
        serial[num].setBaudRate(QSerialPort::Baud115200);
        serial[num].setParity(QSerialPort::NoParity);
        serial[num].setStopBits(QSerialPort::OneStop);
        serial[num].setFlowControl(QSerialPort::NoFlowControl);
        serial[num].setDataBits(QSerialPort::Data8);
        return num;
       }else{
        //somethingwrong= true;
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
            //std::cout << "Name : " << info.portName().toStdString() << std::endl;
            //std::cout << "Description : " << info.description().toStdString() << std::endl ;
            //std::cout << "Manufacturer: " << info.manufacturer().toStdString() << std::endl;

            if(initPort(i,info)!=-1){
                  i++;
            }else{
            }
        }
    /*
    int i =0;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
         if(initPort(i,info)!=-1){
               i++;
         }else{
         }
    }*/



    if(i==0){
        QMessageBox msgBox1;
            msgBox1.setText("Unable to open SerialPort.\nNo NOVAsom found.");
            msgBox1.setWindowTitle("Checking NOVAsom.");
            msgBox1.exec();
        update_status_bar("No NOVAsom found!");

        return;
    }
    ui->connect_novasom_pushButton->setEnabled(false);
    ui->disconnect_novasom_pushButton->setEnabled(true);
    ui->runtest_pushButton->setEnabled(true);
    QMessageBox msgBox2;
    QString msg;
    char ms[5];

    //itoa(i,ms,10);
    my_itoa(i,ms);
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
    //itoa(i,ciao,10);
    my_itoa(i,ciao);
    update_status_bar(messaggio+ciao);
    msgBox.setText("Please insert microSD card into NOVAsom, then power on the boards and wait that all led flashing.");
    msgBox.setWindowTitle("Checking NOVAsom.");
    msgBox.exec();
 return;
}

void MainWindow::on_runtest_pushButton_clicked()
{
    //std::cout << "cisiamo?" << std::endl;
    ui->runtest_pushButton->setEnabled(false);
    ui->disconnect_novasom_pushButton->setEnabled(false);
    ui->logout_pushButton->setEnabled(false);

    QEventLoop loop;
    for (int i=0;i<connectedNOVAsom;i++){
        //std::cout << i << std::endl;
        if (serial[i].isOpen()){
            connect(ui->NextPort_pushButton,&QPushButton::pressed,&loop,&QEventLoop::quit);
            serialport=&serial[i];
            //std::cout << "cisiamo!" << std::endl;

            testingNovasom(serialport);

            //std::cout << "cisiamostati!" << std::endl;
            ui->GenerateParameters_checkBox->setEnabled(true);
            ui->GenerateParameters_PushButton->setEnabled(true);
            ui->NextPort_pushButton->setEnabled(false);
            loop.exec();
            finisciprocedurasingoloNOVASOM(serialport);
        }else {
        }
    }

    QMessageBox msgBox;
        msgBox.setText("Test complete!");
        msgBox.setWindowTitle("Test complete!");
        msgBox.exec();

    ui->GenerateParameters_PushButton->setEnabled(false);
    ui->GenerateParameters_checkBox->setEnabled(false);
    ui->NextPort_pushButton->setEnabled(false);
    ui->Write_EEprom_pushButton->setEnabled(false);
    ui->disconnect_novasom_pushButton->setEnabled(true);
    ui->readDataMatrix->setEnabled(false);
    ui->Print_Label_pushButton->setEnabled(false);
}

int MainWindow::testingNovasom(QSerialPort *serialp){

    QByteArray buffer;
    QByteArray line;
    QString filename;

    line.append("echo 'export REFERENCE_SERVER="+ip+"' > /etc/sysconfig/system_vars\n");
    line.append(". /etc/sysconfig/system_vars\n");
    serialp->write(line);

    filename=ui->line_comboBox->currentText()+"_"+ui->model_comboBox->currentText();
    if (filename.isEmpty()){
        return -1;
        }
    QFile file(lan+filename+".sh");
    QFile file1(lan+"WebServerResult.txt");

    if(file1.exists()) {
        if(file1.remove()) {
            qDebug() << "WebServerResult.txt removed";
        }
        else {
            qCritical() << "Failed to remove WebServerResult.txt!";
            QMessageBox msgBox;
            msgBox.setText("[Error!]");
            msgBox.setWindowTitle("FileServerError-non sono riuscito ad eliminare il file risultato");
            msgBox.exec();
            return -1;
        }
    }
    else {
    }

    if(file.exists()) {
    }
    else {
        qCritical() << "File '" << file.fileName() << "' not found!";
    }
    serialp->write("\ncd /tmp;rm test.sh\n");
    QString tmp;
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
//    serialp->write("touch report.txt\n");
    serialp->write("rm report.txt\n");
    serialp->write("./test.sh >> report.txt\n");
    return 0;
}

int MainWindow::finisciprocedurasingoloNOVASOM(QSerialPort *serialp)
{
    (void)serialp;

    return 0;
}

void MainWindow::on_GenerateParameters_PushButton_clicked()
{
    int k;
    int escape;
    escape=0;

    do {
        escape++;
        k=checkResult();
        if(escape==5) {
            //nonho trovato la rete per 5 volte?? forse problema cpu?
            QMessageBox err;
            err.setText("ERROR");
            err.setIcon(QMessageBox::Critical);
            err.setWindowTitle("ERROR");
            err.setInformativeText("NOVAsom unreacheble! Problem may be network or cpu...");
            //lastupdate
            serialport->write("\n\nreboot\n");
            err.exec();

            ui->GenerateParameters_PushButton->setEnabled(false);
            ui->GenerateParameters_checkBox->setEnabled(false);
            on_disconnect_novasom_pushButton_clicked();
            qCritical() << "Failed to find WebResults.txt for 5 times!";
            return;
        }
    }
    while(k==-2);

    QFile fl(lan + "report.txt");
    if(fl.exists()) {
        fl.remove();
        qDebug() << "report.txt removed";
    }

#ifdef PG_USE_TFTP_ON_TARGET
    // For use tftp the destination file need to already exist and with write permission for the tftp user: create here an empty file with write permission to all
    QByteArray command;
    command.append("touch ");
    command.append(lan);
    command.append("report.txt");
    system(command);
    command.clear();
    command.append("chmod a+w ");
    command.append(lan);
    command.append("report.txt");
    system(command);
    QByteArray tmp;
    tmp.append("unix2dos report.txt ; tftp -l /tmp/report.txt -r report.txt -p ");
    tmp.append(ip);
    tmp.append("\n\n ");
    qDebug() << "Requested report.txt upload";
#else
    QByteArray tmp;
    tmp.append("unix2dos report.txt ; ftpput --username novasomindustries --password novasomindustries ");
    tmp.append(ip);
    tmp.append(" report.txt \n\n ");
#endif

    if(k==0) {
        //test pass
        update_status_bar("TEST PASS!!");
        ui->GenerateParameters_PushButton->setEnabled(false);
        ui->GenerateParameters_checkBox->setEnabled(false);
        ui->Write_EEprom_pushButton->setEnabled(true);
        if(ui->GenerateParameters_checkBox->isChecked()) {
            //genera parametri!
            serialport->write(tmp);
            generateParameters();
        }
        else {
            //non generare parametri!
            ui->PN_lineEdit->clear();
            ui->Lotto_lineEdit->clear();
            ui->seriale_lineEdit->clear();
            ui->MAC_lineEdit->clear();
            ui->OC_lineEdit->clear();

            ui->NextPort_pushButton->setEnabled(false);
            ui->Write_EEprom_pushButton->setEnabled(true);
        }

        /*
        QMessageBox msgBox;

        msgBox.setText("Do you want to generate parameters?");
        msgBox.setInformativeText("??");
        msgBox.setWindowTitle("??");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int video = msgBox.exec();
        QFile fl(lan + "report.txt");
        switch(video){
                case QMessageBox::Yes:

                    if(fl.exists()){
                        qDebug() << "esisteeeeeeeeeee";
                        while(fl.exists()){
                 https://start.fedoraproject.org/           fl.remove();
                        }
                        qDebug() << "remoooveeeeee";
                    }
                    serialport->write(tmp);
                    generateParameters();
                    break;
                case QMessageBox::No:
                    ui->NextPort_pushButton->setEnabled(true);
                    ui->Write_EEprom_pushButton->setEnabled(false);
                    break;
                default:
                    //should never be reached
                    break;
                }
                */
    }
    else if(k==1) {
        //test fail
        update_status_bar("TEST FAIL!!");
        serialport->write(tmp);

        QString pn,lotto;
        QString oc;
        pn=ui->model_comboBox->currentText();
        ui->PN_lineEdit->setText(pn);
        oc=get_OC(pn);
        ui->OC_lineEdit->setText(oc);
        int week,year;
        char buffer[5];
        QDate date;
        if(date.currentDate().isValid()) {
            week=date.currentDate().weekNumber();
            year=date.currentDate().year();
            /*if(getUser()=="BRC01"){
                lotto.append("B");
            }
            if(getUser()=="XD01"){
                lotto.append("X");
            }
            if(getUser()==""){
                lotto.append("A");
            }*/

            lotto.append(getUserChar(getUser()));

            /*
            if(ui->RM_checkBox->isChecked()){
                lotto.append("B");
            }else{
                lotto.append("A");
            }
            */
            year=year-2000;
            //itoa(year,buffer,10);
            my_itoa(year,buffer);
            lotto.append(buffer);
            if(week<10){
                lotto.append("0");
            }
            //itoa(week,buffer,10);
            my_itoa(week, buffer);
            lotto.append(buffer);
            //lotto.append("-");

            ui->Lotto_lineEdit->setText(lotto);
        }
        else {
            qCritical() << "Invalid current date: can't generate lot number!";
        }
        ui->MAC_lineEdit->clear();
        ui->seriale_lineEdit->clear();
        ui->GenerateParameters_PushButton->setEnabled(false);
        ui->GenerateParameters_checkBox->setEnabled(false);
        ui->NextPort_pushButton->setEnabled(false);
        ui->Write_EEprom_pushButton->setEnabled(true);
        QString titolo;
        char stdstr[20];
        QByteArray line;
        int error;
        QFile fileerror(Fail+ui->PN_lineEdit->text()+"_"+ui->Lotto_lineEdit->text()+"error.txt");
        if(fileerror.exists()) {
            fileerror.open(QIODevice::ReadWrite | QIODevice::Text);
            line=fileerror.readAll();
            error=atoi(line);
            line.clear();

        }else {
            //crea file!!
            error=0;
            line.clear();
            //clearstdstr(stdstr);
        }
        fileerror.close();
        error++;
        titolo.clear();
        my_itoa(error, stdstr);
        //itoa(error,stdstr,10);
        titolo.append(stdstr);
        clearstdstr(stdstr);
        if(!fileerror.isOpen()) {
            fileerror.open(QIODevice::ReadWrite | QIODevice::Text);
        }
        my_itoa(error, stdstr);
        //itoa(error,stdstr,10);
        fileerror.write(stdstr);
        clearstdstr(stdstr);
        fileerror.close();
    }
    else {
        qCritical() << "Unexpeted checkResult() return value!";
    }
}

int MainWindow:: checkResult(){
    QByteArray linebytearray;
    QFile file1(lan+"WebServerResult.txt");
    if(file1.exists()) {
        if (!file1.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QMessageBox::information(this, tr("Unable to open file"),file1.errorString());
            qDebug() << "Failed to open WebServerResult.txt";
            return -1;
        }
        linebytearray=file1.readAll();
        QMessageBox err;
        err.setText("TEST FAIL");
        err.setIcon(QMessageBox::Critical);
        err.setWindowTitle("Test Fail");
        err.setInformativeText("Test Fail");
        qDebug() << "WebServerResult.txt contains " << linebytearray;
        if(linebytearray.toStdString()=="OK") {
            QMessageBox msgBox;
            msgBox.setText("Check video...");
            msgBox.setInformativeText("Can you see video?");
            msgBox.setWindowTitle("Check result...");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            int video = msgBox.exec();
            switch(video) {
            case QMessageBox::Yes:
                ui->testpass_checkBox->setChecked(true);
                ui->testFail_checkBox->setChecked(false);
                qDebug() << "Test video OK";
                return 0;
                break;

            case QMessageBox::No:
                ui->testpass_checkBox->setChecked(false);
                ui->testFail_checkBox->setChecked(true);
                err.exec();
                qDebug() << "Test video failed";
                return 1;
                break;

            default:
                break;
            }
            return 0;
        }
        else if (linebytearray.toStdString()=="KO") {
            QMessageBox msgBox1;
            msgBox1.setText("Test Fail.");
            msgBox1.setWindowTitle("Check result...");
            msgBox1.exec();
            err.exec();
            ui->testpass_checkBox->setChecked(false);
            ui->testFail_checkBox->setChecked(true);
            return 1;
        }
        else {
            QMessageBox msgBox2;
            msgBox2.setText("Problema sconosciuto...\nFile risultato non corretto.Ripetere il test...");
            msgBox2.setWindowTitle("???");
            msgBox2.exec();
            ui->testpass_checkBox->setChecked(false);
            ui->testFail_checkBox->setChecked(false);
            return -1;
        }
    }
    else {
        QMessageBox msgBox3;
        msgBox3.setText("WebServer problem...No network?? ");
        msgBox3.setWindowTitle("Check result...");
        msgBox3.exec();
        qDebug() << "WebServerResult.txt not found";
        return -2;
    }
}

void MainWindow::generateParameters() {
    QString filename,pn,lotto,oc;
    filename=(ConfigDir+"/PN.ini");
    pn=ui->model_comboBox->currentText();
    ui->PN_lineEdit->setText(pn);
    oc=get_OC(pn);
    ui->OC_lineEdit->setText(oc);
    int week,year;
    char buffer[5];
    QDate date;
    if(date.currentDate().isValid()){
        week=date.currentDate().weekNumber();
        year=date.currentDate().year();
        /*if(getUser()=="BRC01"){
            lotto.append("B");
        }
        if(getUser()=="XD01"){
            lotto.append("X");
        }
        if(getUser()==""){
            lotto.append("A");
        }*/

        lotto.append(getUserChar(getUser()));

        /*
        if(ui->RM_checkBox->isChecked()){
            lotto.append("B");BRC01
        }else{
            lotto.append("A");
        }
        */
        year=year-2000;
        my_itoa(year, buffer);
        //itoa(year,buffer,10);
        lotto.append(buffer);
        if(week<10){
            lotto.append("0");
        }
        my_itoa(week, buffer);
        //itoa(week,buffer,10);
        lotto.append(buffer);
        //lotto.append("-");

        ui->Lotto_lineEdit->setText(lotto);
       }else{
        //non ho data!! sono fottuto!!
         }
    //if(){
    if(getAuthentication() == 1){

        //RESETMODE
        filename=LogDir+pn+"_"+lotto+".ini";
        QFile file2(filename);
        QByteArray riga;
        char serialStr[6], buffer[7];
        int seriale, len,i;
        if(file2.exists()){
            //continua con l'ultimo seriale;
            file2.open(QIODevice::ReadOnly | QIODevice::Text);
            riga=file2.readAll();
            file2.close();
            seriale=atoi(riga);
            seriale=seriale+1;
            my_itoa(seriale, serialStr);
            //itoa(seriale,serialStr,10);
            len=strlen(serialStr);
            if(len<5){
                for(i=0;i<5;i++){
                    serialStr[i]='\0';
                }
                for(i=0;i<5-len;i++){

                    serialStr[i]='0';
                }

                //strcat(serialStr,itoa(seriale,buffer,10));
                strcat(serialStr,my_itoa(seriale,buffer));
            }
            file2.open(QIODevice::WriteOnly | QIODevice::Text);
            file2.write(serialStr);
            file2.close();
        }else{
            //crea file e ricomincia seriale
            strcpy(serialStr,"00001");
            file2.open(QIODevice::WriteOnly | QIODevice::Text);
            file2.write(serialStr);
            file2.close();
        }
        //ui->SERIALE_textBrowser->setText(serialStr);
        ui->seriale_lineEdit->setText(serialStr);



    }else{
        //ProgressiveMODE
        //apri file, leggi ultimo seriale scritto, prendi successivo!!
        filename=LogDir+pn+".ini";
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
            //itoa(seriale,serialStr,10);
            my_itoa(seriale,serialStr);
            len=strlen(serialStr);

            if(len<6){
                for(i=0;i<6;i++){
                    serialStr[i]='\0';
                }
                for(i=0;i<6-len;i++){

                    serialStr[i]='0';
                }

                //strcat(serialStr,itoa(seriale,buffer,10));
                strcat(serialStr,my_itoa(seriale,buffer));
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
    //int l;

    if(file4.exists()){
        //il file esiste, leggi ultimo seriale, incrementa, scrivilo
        clearstdstr(stdstr);
        file4.open(QIODevice::ReadWrite | QIODevice::Text);
        line=file4.readAll();
        macseriale=atoi(line);
        line.clear();
        //itoa(macseriale,stdstr,10);
     }
     else{
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
    //itoa(macseriale,stdstr,10);
    my_itoa(macseriale,stdstr);
    //qDebug()<<macseriale;
    //std::cout << "macseriale: "<< macseriale << std::endl;
    //qDebug() <<stdstr;
    //std::cout << "stdstr: "<< stdstr << std::endl;
    if(!file4.isOpen()){
        file4.open(QIODevice::ReadWrite | QIODevice::Text);
    }

    file4.write(stdstr);
    //clearstdstr(stdstr);

    //itoa(macseriale,stdstr,16);
    //my_itoa(macseriale,stdstr);
    file4.close();

    // THE THIS PART HAS BEEN REPLACED. CHECK LINE 1325
    /*mac="00:4e:";
    mac.append(pn[11]);
    mac.append(pn[12]);
    mac.append(':');
    mac.append(pn[15]);

    l=strlen(stdstr);

//NEED TO BE REWRITE! SHITTY CODE
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
    }*/

    mac = "004e";
    QString zeros;
/*    mac.append(pn[11]);
    mac.append(pn[12]);
    mac.append(pn[15]);
*/
    mac.append(pn[12]);
    mac.append(pn[14]);
    mac.append(pn[15]);

    while (mac.size()<12)
        mac.append("0");

    while (uint (zeros.size())<5-strlen(stdstr))
        zeros.append("0");

    for (size_t i=0; i<strlen(stdstr); i++)
       zeros.append(stdstr[i]);

    for (int i=0; i<zeros.size(); i++){
        if (mac.at(i+7) != zeros[i])
            mac.replace(i+7, 1, zeros[i]);
    }

    for (int i=2; i<=14; i+=3){
        mac.insert(i, ":");
    }

    ui->MAC_lineEdit->setText(mac);


    get_OC(pn);
}

void MainWindow::on_Write_EEprom_pushButton_clicked()
{
    if(ui->testpass_checkBox->isChecked()){
        if(ui->line_comboBox->currentText()=="P-Line"){
            QString pn,mac,lotto,seriale;
            serialport->write("get_exec e2info\n");

            pn=ui->PN_lineEdit->text();
            //qDebug() << pn.toUtf8();
            seriale=ui->seriale_lineEdit->text();
            //qDebug() << seriale.toUtf8();
            lotto=ui->Lotto_lineEdit->text();
            //qDebug() << lotto.toUtf8();
            mac=ui->MAC_lineEdit->text();
            //qDebug() << mac.toUtf8();
            serialport->write("chmod 777 e2info\n");
            serialport->write("./e2info -P "+ pn.toUtf8()+"\n");
            serialport->write("./e2info -M "+ mac.toUtf8()+"\n");
            serialport->write("./e2info -S "+ seriale.toUtf8()+"\n");
            serialport->write("./e2info -L "+ lotto.toUtf8()+"\n");
        }
        QString titolo;

        titolo.append(ui->PN_lineEdit->text());
        titolo.append("_");
        titolo.append(ui->seriale_lineEdit->text());
        titolo.append("_");
        titolo.append(ui->Lotto_lineEdit->text());
        generateReport(titolo);
        uploadReport(titolo+"report.pdf",directory);


        ui->NextPort_pushButton->setEnabled(true);
        ui->Write_EEprom_pushButton->setEnabled(false);
        ui->Print_Label_pushButton->setEnabled(true);
        ui->readDataMatrix->setEnabled(true);


    }else{
        //se il test è fallito??//
        QString titolo;
        QByteArray line;
        int error;
        QFile fileerror(Fail+ui->PN_lineEdit->text()+"_"+ui->Lotto_lineEdit->text()+"error.txt");
        fileerror.open(QIODevice::ReadWrite | QIODevice::Text);
        line=fileerror.readAll();
        error=atoi(line);
        line.clear();
        fileerror.close();

        titolo.append(ui->PN_lineEdit->text());
        titolo.append("_");
        titolo.append(ui->Lotto_lineEdit->text());
        titolo.append("_");
        titolo.append("FAIL");
        titolo.append("_");
        char errorline[10];

        //titolo.append(itoa(error,errorline,10));
        titolo.append(my_itoa(error,errorline));
        generateReport(titolo);

        ui->NextPort_pushButton->setEnabled(true);
        ui->Write_EEprom_pushButton->setEnabled(false);
        ui->Print_Label_pushButton->setEnabled(false);
    }
}

int MainWindow::generateReport(QString filename){
    QFile report(directory+filename+".txt");
    qDebug() << "Generate report " << filename;
    if(!report.open(QIODevice::ReadWrite|QIODevice::Text)) {
        qCritical() << "Failed to create report file!";
        return -1;
    }
    QFile original(lan + "report.txt");
    if(!original.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qCritical() << "Failed to open '" << original.fileName() << "'";
        return -1;
    }
    qCritical() << original.fileName() << " found";
    QByteArray rep;

    rep=original.readAll();
    report.write("#");
    report.write(filename.toUtf8());
    report.write("\n");
    report.write("#");
    report.write(user.toUtf8());
    report.write("\n");
    if(ui->testpass_checkBox->isChecked()) {
        report.write("#TEST PASS!\n");
    }
    else {
        report.write("#TEST FAIL!");
    }

    report.write("\nPN: ");
    report.write(ui->PN_lineEdit->text().toUtf8());
    report.write("\nOC: ");
    report.write(ui->OC_lineEdit->text().toUtf8());
    report.write("\nSeriale: ");
    report.write(ui->seriale_lineEdit->text().toUtf8());
    report.write("\nLotto: ");
    report.write(ui->Lotto_lineEdit->text().toUtf8());
    report.write("\nMAC ADDRESS: ");
    report.write(ui->MAC_lineEdit->text().toUtf8());
    report.write("\n");
    report.write(rep);
    report.close();
    createPDF(filename);
    return 0;
}

int MainWindow:: createPDF(QString filename){

    QFile file(directory+filename+".txt");

    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::information(this, tr("Unable to open file"),file.errorString());
        return -1;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(directory+filename+"report.pdf");

    QPainter painter;
    painter.begin(&printer);
    QString line;

    QPixmap pixmap1 ("C:/NOVATEST_SP/CONFIG/img/upper.png");
    QPixmap pixmap2 ("C:/NOVATEST_SP/CONFIG/img/lower.png");

    painter.drawPixmap(10,10,9500,750, pixmap1);
    painter.drawPixmap(10,12300,9500,750,pixmap2);
    //painter.drawPixmap();

    int x,y;
    x=80;
    y=1000;
    while (!file.atEnd()) {
           line = file.readLine();
           painter.drawText(x,y,line);
           line.clear();
           y=y+120;
       }
    file.close();
    painter.end();
    return 0;
}

void MainWindow::on_Print_Label_pushButton_clicked()
{
    QString titolo;

    titolo.append(ui->PN_lineEdit->text());
    titolo.append("_");
    titolo.append(ui->seriale_lineEdit->text());
    titolo.append("_");
    titolo.append(ui->Lotto_lineEdit->text());
    titolo.append("_");
    titolo.append("label.txt");

    generateLabel(titolo);
    sleep(1);
    printLabel(titolo);
    qDebug() << "Label printed";
    ui->Print_Label_pushButton->setEnabled(false);
}

int MainWindow::generateLabel(QString filename){


    QFile source(labeltemplate);
    QFile destination(directory+filename);
    QFile destination1(directory+"last_label.txt");
    int len;


    if (!source.open(QIODevice::ReadOnly | QIODevice::Text)){
        update_status_bar("Impossibile generare etichetta!Contatta gestore sw!");
        return -1;
    }else{
    }

    QByteArray text;
    char *textstring;

    text=source.readAll();

    len=text.count();
    textstring=(char *)malloc(sizeof(char)*(len+1));
    if(textstring==NULL){
        update_status_bar("MEMORY ERROR!Impossibile generare etichetta!Contatta gestore sw!");
        return -1;
    }
    strcpy(textstring,text.constData());

    text.clear();
    int count;
    count=0;
    for(int i=0; i<len;i++){
        if(textstring[i]!='$'){
            text.append(textstring[i]);
        }else{
            if(count == 0){
                //quanti '$' ci sono??
                text.append('$');
            }
            if(count== 1){
                //tutto
                text.append(ui->PN_lineEdit->text());
                //text.append(ui->OC_lineEdit->text());
                text.append(ui->Lotto_lineEdit->text());
                text.append(ui->seriale_lineEdit->text());
                //qDebug()
            }
            if(count == 2){
                //PN
                text.append(ui->PN_lineEdit->text());
            }
            if(count == 3){
                //DC
                text.append(ui->Lotto_lineEdit->text());
            }
            if(count == 4){
                //SN
                text.append(ui->seriale_lineEdit->text());
            }
            if(count == 5){
                //OC
                text.append(ui->OC_lineEdit->text());

            }
            if(count == 6){
                //quanti '$' ci sono??

                text.append('$');
            }
            count ++;
        }
    }

    if (!destination.open(QIODevice::ReadWrite | QIODevice::Text)){
        update_status_bar("Impossibile generare etichetta2!Contatta gestore sw!");
        return -1;
    }else{
    }
    if (!destination1.open(QIODevice::ReadWrite | QIODevice::Text)){
        update_status_bar("Impossibile generare etichetta2!Contatta gestore sw!");
        return -1;
    }else{

    }

    destination.write(text);
    destination.close();
    destination1.write(text);
    destination1.close();
    return 0;
}

int MainWindow::printLabel(QString filename)
{
    (void) filename;
    //system("notepad \/p \/NOVATEST_SP\/TESTED\/last_label.txt");
    system("lpr /NOVAtestSMPU/TESTED/last_label.txt");
    return 0;
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


void MainWindow::on_load_configuration_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load Configuration File"), "D:/QtProjects/NOVATest",tr("Configuration Files (*.ini)"));
    if (fileName.isEmpty())
    {
        //qDebug() << "fileName is empty";
        return;
    }
    else
    {
        //qDebug() << "fileName :"+fileName;

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            //qDebug() << "1";
            return;
        }
        QString strKeyFunc("CONFIG/");
        QSettings * settings = 0;

        QFileInfo fi(fileName);
        settings = new QSettings( fileName, QSettings::IniFormat );

        QList<QCheckBox *> checkBoxes = this->findChildren<QCheckBox *>();

        for (int i=0; i<checkBoxes.count(); ++i)
        {
            QString checkBoxName = checkBoxes.at(i)->objectName();
            if (getvalue(strKeyFunc, settings , checkBoxName) == "true")
            {
                //qDebug() << "true";
                checkBoxes.at(i)->setChecked(true);
            }
            else
            {
                //qDebug() << "false";
                checkBoxes.at(i)->setChecked(false);
            }

        }
/*
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
            }*/
        file.close();
    }
}

void MainWindow::on_save_configuration_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Configuration File"), "D:/QtProjects/",tr("Configuration Files (*.ini)"));

    if (fileName.isEmpty())
    {
        //qDebug() << "fileName is empty";
        return;
    }
    else
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),file.errorString());
            //qDebug() << "5";
            return;
        }
        QTextStream out(&file);
        out << QString("[CONFIG]\r\n");

        QList<QCheckBox *> checkBoxes = this->findChildren<QCheckBox *>();

        for (int i=0; i<checkBoxes.count(); ++i)
        {
            QString checkBoxName = checkBoxes.at(i)->objectName();
            if (checkBoxes.at(i)->isChecked())
                out << QString(checkBoxName.append("=true\r\n"));
            else
                out << QString(checkBoxName.append("=false\r\n"));
        }
/*
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
            out << QString("wifi_checkBox=false\r\n");*/
        file.close();
    }
}

void MainWindow::on_NextPort_pushButton_clicked()
{
    ui->Print_Label_pushButton->setEnabled(false);
    //ui->readDataMatrix->setEnabled(false);
    ui->readDataMatrix->setDisabled(true);
}



void MainWindow::on_readDataMatrix_clicked()
{
    DialogDataMatrix *d = new DialogDataMatrix(this);
    d->show();
    ui->readDataMatrix->setEnabled(false);

    //UsrPswDialog *p = new UsrPswDialog(this);
    //p->show();
}

int MainWindow::getAuthentication(){
    return authenticated;
}


void MainWindow::setAuthentication(int authFlag){
    authenticated = authFlag;
}


QString MainWindow::get_OC(QString pn){

    QString OC;

    QString strKeyFunc("OC/");
    QSettings * settings = 0;
    QString filename;
    filename="/OC.ini";
     //cout << "Cerchiamo file:" << endl;
    //cout << ConfigDir.toStdString()+filename.toStdString() << endl;
    //cout << "Cerchiamo key:" << endl;
   //cout << pn.toStdString() << endl;
    settings = new QSettings( ConfigDir+filename, QSettings::IniFormat );
    OC= getvalue(strKeyFunc, settings, pn);
    //cout << "trovato:" << endl;
   //cout << OC.toStdString() << endl;
    //ui->OC_lineEdit->setText(OC);
   return OC;
}



void MainWindow::on_suser_operations_pushButton_clicked()
{
    SuserOperations *suser_operations = new(SuserOperations);
    suser_operations->show();
}

QString MainWindow::getPswNlog() const
{
    return pswNlog;
}

void MainWindow::setPswNlog(const QString &value)
{
    pswNlog = value;
}

void MainWindow::setPsw(const QString &value)
{
    psw = value;
}


void MainWindow::replyFinished(QNetworkReply *reply){
    //qDebug() << "reply signal************";
    connect(this, SIGNAL(pswNlogSig(QString)), this, SLOT(pswNlogSlt(QString)));
    QString answer = QString::fromUtf8(reply->readAll());
    //QByteArray byte_content = reply->readAll();
    setPswNlog(answer.toUtf8());
    emit pswNlogSig(answer);
    //qDebug() << pswNlog+" reply signal";
}

void MainWindow::pswNlogSlt(QString logstr){
    setPswNlog(logstr);
    //qDebug() << logstr+" pswNlogSlt";
}

#define FTP_URL         "ftp.emc-computers.ro"
#define FTP_USERNAME    "novaftp@emc-computers.ro"
#define FTP_PASSWORD    "XZqEgnU7"
#define FTP_PATH        "./"

int MainWindow::uploadReport(QString filename, QString path)
{
#ifdef PG_UPLOAD_REPORT_ON_FTP
    QString command;
/*
    command.append("ftp-upload -h ");
    command.append(FTP_URL);
    command.append(" -u ");
    command.append(FTP_USERNAME);
    command.append(" --password ");
    command.append(FTP_PASSWORD);
    command.append(" -d ");
    command.append(FTP_PATH);
    command.append(" --passive");
    command.append(" ");
    command.append(path);
    command.append(filename);
*/
    command.append("ftp -n <<EOF\npassive\nopen ");
    command.append(FTP_URL);
    command.append("\n user ");
    command.append(FTP_USERNAME);
    command.append(" ");
    command.append(FTP_PASSWORD);
    command.append("\nput ");
    command.append(path);
    command.append(filename);
    command.append(" ");
    command.append(filename);
    command.append("\nEOF\n");
    qDebug() << "FTP upload:<" << command << ">";
    int rc = system(command.toStdString().c_str());
    qDebug() << "FTP upload completed (" << rc << + ")";
    return rc;

#else
    /*
    QString command;
    char *c;
    command.append("sshpass -p \"ZippameloTutto\" scp -r ");
    command.append(path);
    command.append(filename);
    command.append(" mpuca@93.41.190.216:/home/mpuca/.");
    cout << "command UPLOADING" << endl;
    cout << command.toStdString() << endl;
    strcpy(c,command.toStdString());
    system(c);
    return 0;
    //return (system ("sshpass -p \"**********\" scp -r /home/mic/Desktop/sshscp mpuca@93.41.190.216:/home/mpuca/."));
    */

    /*manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
    QUrl url("http://93.41.190.216/novasomLog");
    manager->get(QNetworkRequest(url));*/

    system("wget http://93.41.190.216/novasomLog");
    QFile logfile("./novasomLog");
    QString logfileContent;

    if (logfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream logfileStream(&logfile);
        logfileContent = logfileStream.readAll();
        logfile.close();
        logfile.remove();
    }
    else
        qDebug() << "log file read error";

    QByteArray command;
    //qDebug() << getPswNlog();
    command.append("sshpass -p \"");
    command.append(logfileContent);
    //command.append("sshpass -p \"CapitanFindus\" scp -r ");
    command.append("\" scp -r ");
    //qDebug() << getPswNlog() +"after";

    command.append(path);
    command.append(filename);
    command.append(" novasomLog@93.41.190.216:/home/novasomLog/.");
    //cout << "command UPLOADING" << endl;
    //cout << command.toStdString() << endl;
    return system(command);
#endif
}
