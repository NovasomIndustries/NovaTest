#include "suseroperations.h"
#include "ui_suseroperations.h"

using namespace std;

SuserOperations::SuserOperations(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SuserOperations)
{
    ui->setupUi(this);
}

SuserOperations::~SuserOperations()
{
    delete ui;
}

void SuserOperations::backup_credentials()
{
    QFile encrFile(encrCredDir);
    QFile encrBakFile(encrCredBakDir);
    if (encrBakFile.exists())
        encrBakFile.remove();
    if (encrFile.exists()) {
        qDebug() << "Credentials file backup stored";
        encrFile.copy(encrCredBakDir);
    }
    else {
        qCritical() << "Unable to open credentials file";
        QMessageBox::critical(this, "No credentials file", encrFile.errorString());
    }
}

uint SuserOperations::user_operations()
{
    qDebug() << "User operation selected";
    QFile encrFile(decrCredDir);
    QFile decrFile(decrCredDir);
    QString group;
    if (decrFile.exists())
        decrFile.remove();
    QString decrContent = m->read_credentials(encrCredDir, true);
    m->write_credentials(decrContent, decrCredDir, false);
    ui->current_content_textBrowser->setText(decrContent);
    QString uname = ui->uname_lineEdit->text();
    QString passw = ui->passw_lineEdit->text();
    if (!uname.isEmpty() && !passw.isEmpty()) {
        backup_credentials();
        QSettings settings(decrCredDir, QSettings::IniFormat);
        if (ui->suser_checkBox->isChecked())
            group = "SUSERS";
        else
            group = "USERS";;
        settings.beginGroup(group);
        if (ui->usr_add_radioButton->isChecked()) {
            qDebug() << "User add operation selected";
            if (settings.contains(uname)) {
                qDebug() << "Username" << uname << "already existing";
                QMessageBox::StandardButton reply = QMessageBox::warning(this, "Existing user", "This user already exists. Do you want to update it?",
                                                                         QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    qDebug() << "User" << uname << "updated";
                    QString old_passw = settings.value(uname, "r").toString();
                    QString uname_temp = uname;
                    decrContent.replace(uname.append("=").append(old_passw), uname_temp.append("=").append(passw));
                    QMessageBox::information(this, "User updated", "Existing credentials updated successfully.");
                    ui->current_content_textBrowser->setText(decrContent);
                }
            }
            else {
                QString newuser = uname + "=" + passw + "\n";
                if (ui->suser_checkBox->isChecked()) {
                    qDebug() << "User" << uname << "added (Super user)";
                    decrContent.insert(decrContent.indexOf("[SUSERS]")+9, newuser);
                }
                else {
                    qDebug() << "User" << uname << "added (Normal user)";
                    decrContent.insert(decrContent.indexOf("[USERS]")+8, newuser);
                }
                ui->current_content_textBrowser->setText(decrContent);
            }
        }
        else if (ui->usr_del_radioButton->isChecked()) {
            qDebug() << "User delete operation selected";
            if (settings.contains(uname) && passw==settings.value(uname).toString()) {
                const QStringList userKeys = settings.childKeys();
                if (group=="SUSERS" && userKeys.size()==1) {
                    qDebug() << "Can't delete unique superuser" << uname;
                    QMessageBox::warning(this, "Unique Suser", "There must be at least one super user!");
                }
                else {
                    qDebug() << "User" << uname << "deleted";
                    QString credential;
                    credential = (uname.append("=").append(passw));
                    decrContent.remove(credential);
                    decrContent.replace("\n\n", "\n");
                }
            }
            else {
                QMessageBox::information(this, "Invalid credentials", "This user does not exist.");
                return 3;
            }
            ui->current_content_textBrowser->setText(decrContent);
        }
        settings.endGroup();
        decrFile.remove();
        encrFile.remove();
        m->write_credentials(decrContent, decrCredDir, false);
        m->write_credentials(decrContent, encrCredDir, true);
    }
    if (!ui->keep_decr_checkBox->isChecked() && decrFile.exists()) {
        qDebug() << "Credentials decrypted file removed";
        decrFile.remove();
    }
    else {
        qWarning() << "Credentials decrypted file stored";
        ui->current_content_textBrowser->setText(decrContent);
        QMessageBox::information(this, "File created", "Decrypted file has been created.");
        return 1;
    }
    return 0;
}

uint SuserOperations::mac_operations()
{
    qDebug() << "MAC address operation selected";
    QFile encrFile(encrCredDir);
    QFile decrFile(decrCredDir);
    if (decrFile.exists())
        decrFile.remove();
    QString decrContent = m->read_credentials(encrCredDir, true);
    m->write_credentials(decrContent, decrCredDir, false);
    QSettings settings(decrCredDir, QSettings::IniFormat);
    QString host = ui->hname_lineEdit->text();
    QString macnum = ui->macnum_lineEdit->text();
    if (!host.isEmpty() && !macnum.isEmpty()) {
        settings.beginGroup("TRUEMAC");
        macnum = macnum.toUpper();
        if (macnum.size()==12) {
            for (int i=2; i<=14; i+=3)
                macnum.insert(i, ":");
        }
        else if (!(macnum.size()==17)) {
            QMessageBox::information(this, "Non valid mac address", "Please enter a valid MAC address.");
            return 1;
        }
        if (ui->mac_add_radioButton->isChecked()) {
            qDebug() << "MacAddress add operation selected";
            if (settings.contains(host)) {
                qDebug() << "Host" << host << "already exixting";
                QMessageBox::StandardButton reply = QMessageBox::warning(this, "Existing host", "This host already exists. Do you want to update it?",
                                                                         QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    qDebug() << "Host" << host << "updated with MAC" << macnum;
                    QString old_mac = settings.value(host, "r").toString();
                    QString host_temp = host;
                    decrContent.replace(host.append("=").append(old_mac), host_temp.append("=").append(macnum));
                    QMessageBox::information(this, "Host updated", "Existing MAC address updated successfully.");
                    ui->current_content_textBrowser->setText(decrContent);
                }
            }
            else {
                qDebug() << "Host" << host << "added with MAC" << macnum;
                QString newmac = host + "=" + macnum + "\n";
                decrContent.insert(decrContent.indexOf("[TRUEMAC]")+10, newmac);
            }
        }
        else if (ui->mac_del_radioButton->isChecked()) {
            qDebug() << "MacAddress delete operation selected";
            if (settings.contains(host)) {
                qDebug() << "Host" << host << "with MAC" << macnum << "deleted";
                QString entry;
                entry = (host.append("=").append(macnum));
                decrContent.remove(entry);
                decrContent.replace("\n\n", "\n");
            }
            else {
                qDebug() << "Host" << host << "does not exist";
                QMessageBox::information(this, "Invalid entry", "This entry does not exist.");
            }
        }
        settings.endGroup();
        decrFile.remove();
        encrFile.remove();
        m->write_credentials(decrContent, decrCredDir, false);
        m->write_credentials(decrContent, encrCredDir, true);
    }
    if (!ui->keep_decr_checkBox->isChecked() && decrFile.exists()) {
        qDebug() << "Credentials decrypted file removed";
        decrFile.remove();
    }
    else {
        qWarning() << "Credentials decrypted file stored";
        ui->current_content_textBrowser->setText(decrContent);
        QMessageBox::information(this, "File created", "Decrypted file has been created.");
        return 1;
    }
    ui->current_content_textBrowser->setText(decrContent);
    return 0;
}

void SuserOperations::on_apply_pushButton_clicked()
{
    uint usr_ops_return_flag;

    qDebug() << "Apply button pressed";
    switch (ui->operations_tab->currentIndex()) {
    case 0:
        usr_ops_return_flag = user_operations();
        break;
    case 1:
        usr_ops_return_flag = mac_operations();
        break;
    default:
        break;
    }

    ui->usr_add_radioButton->setAutoExclusive(false);
    ui->usr_add_radioButton->setChecked(false);
    ui->usr_add_radioButton->setAutoExclusive(true);
    ui->usr_del_radioButton->setAutoExclusive(false);
    ui->usr_del_radioButton->setChecked(false);
    ui->usr_del_radioButton->setAutoExclusive(true);

    ui->mac_add_radioButton->setAutoExclusive(false);
    ui->mac_add_radioButton->setChecked(false);
    ui->mac_add_radioButton->setAutoExclusive(true);
    ui->mac_del_radioButton->setAutoExclusive(false);
    ui->mac_del_radioButton->setChecked(false);
    ui->mac_del_radioButton->setAutoExclusive(true);

    Q_UNUSED(usr_ops_return_flag);
}

void SuserOperations::on_done_pushButton_clicked()
{
    this->close();
}
