#ifndef ADVANCED_H
#define ADVANCED_H

#include <QDialog>
#include "ui_advanced.h"

#include "mainwindow.h"
#include "aboutdialog.h"

namespace Ui {
class Advanced;
}

class Advanced : public QDialog
{
    Q_OBJECT

public:
    explicit Advanced(QWidget *parent = nullptr);
    ~Advanced();

private slots:
    void loadSettings();
    void on_pushButton_ok_clicked();
    void on_pushButton_apply_clicked();
    void on_checkBox_stateChanged(int arg1);
    void on_checkBox_8_stateChanged(int arg1);
    void on_checkBox_9_stateChanged(int arg1);
    void on_checkBox_1_stateChanged(int arg1);
    void on_checkBox_5_stateChanged(int arg1);
    void on_checkBox_6_stateChanged(int arg1);
    void on_checkBox_7_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void on_checkBox_3_stateChanged(int arg1);
    void on_checkBox_4_stateChanged(int arg1);
    void on_checkBox_10_stateChanged(int arg1);
    void on_checkBox_11_stateChanged(int arg1);
    void on_checkBox_12_stateChanged(int arg1);
    void on_checkBox_13_stateChanged(int arg1);
    void on_checkBox_14_stateChanged(int arg1);
    void on_lineEdit_2_textChanged(const QString &arg1);
    void on_lineEdit_8_textChanged(const QString &arg1);
    void on_lineEdit_9_textChanged(const QString &arg1);
    void on_lineEdit_10_textChanged(const QString &arg1);
    void on_lineEdit_textChanged(const QString &arg1);
    void on_radioButton_2_toggled(bool checked);
    void on_comboBox_currentIndexChanged(int index);
    void on_radioButton_clicked(bool checked);

private:
    Ui::Advanced *ui;
};

#endif // ADVANCED_H
