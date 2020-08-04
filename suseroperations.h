#ifndef SUSEROPERATIONS_H
#define SUSEROPERATIONS_H

#include <QDialog>
#include "usrpswdialog.h"

namespace Ui {
class SuserOperations;
}

class SuserOperations : public QDialog
{
    Q_OBJECT

public:
    explicit SuserOperations(QWidget *parent = 0);
    ~SuserOperations();

private slots:
    void on_apply_pushButton_clicked();

    void on_select_file_pushButton_clicked();

    void on_done_pushButton_clicked();

    void get_backup();

    uint user_operations();

    uint mac_operations();

private:
    Ui::SuserOperations *ui;
    UsrPswDialog *m = qobject_cast<UsrPswDialog*>(this->parent());
};

#endif // SUSEROPERATIONS_H
