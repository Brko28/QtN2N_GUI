#include "advanced.h"

#include "mainwindow.h"
#include "aboutdialog.h"

#include <QSettings>
#include <QCloseEvent>
#include <QDir>
#include <QString>
#include <QMessageBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDebug>

Advanced::Advanced(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Advanced)
{
    ui->setupUi(this);

    loadSettings();

    ui->comboBox->addItem("Static IP",0);
    ui->comboBox->addItem("Dynamic IP",1);
    ui->comboBox->show();

    QString port = ui->lineEdit_8->text();
    QString mac = ui->lineEdit_9->text();
    QString mtu = ui->lineEdit_10->text();
    QString management_port = ui->lineEdit->text();
    QString nickname = ui->lineEdit_2->text();

    if(port.isEmpty())
      {
        ui->lineEdit_8->setText("86");
      }
    if(mac.isEmpty())
      {
        ui->lineEdit_9->setText("00:FF:06:62:95:52");
      }
    if(mtu.isEmpty())
      {
        ui->lineEdit_10->setText("1400");
      }
    if(management_port.isEmpty())
      {
        ui->lineEdit->setText("5645");
      }
    if(ui->checkBox_11->isChecked())
    if(ui->radioButton_2->isChecked())
      {
        ui->lineEdit->setEnabled(true);
      }
    if(ui->radioButton->isChecked())
      {
        ui->lineEdit->setDisabled(true);
      }
    if(nickname.isEmpty())
      {
        ui->lineEdit_2->setText("User");
      }
    ui->lineEdit_8->setToolTip("<b>n2n v2.0.0</b><ul><li>Default: 7654</li><li>Default: 86 for vpnhosting.cz</li></ul><b>n2n v1.3.3</b><ul><li>Default: 7654</li><li>Default: 82 for vpnhosting.cz</li></ul>");
    ui->lineEdit->setToolTip("Only for multiple edges on a machine");
    ui->checkBox_11->setToolTip("Only for multiple edges on a machine");
    ui->lineEdit_2->setToolTip("Enter Nickname for chat");
}

Advanced::~Advanced()
{
    delete ui;
}

static bool task(QString port,QString mac,QString mtu,QString management_port,QString nickname)
{
    if(port == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing Local Port!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(mac == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing MAC Address!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(mtu == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing MTU!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(management_port == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing Management UDP Port!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(nickname == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing Nickname!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
        return true;
      }
      }
      }
      }
      }
}

void Advanced::on_pushButton_ok_clicked()
{
    close();
}

void Advanced::on_pushButton_apply_clicked()
{
    QString port = ui->lineEdit_8->text();
    QString mac = ui->lineEdit_9->text();
    QString mtu = ui->lineEdit_10->text();
    QString management_port = ui->lineEdit->text();
    QString nickname = ui->lineEdit_2->text();

    if(task(port,mac,mtu,management_port,nickname))
      {
        //save settings on apply
        QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);

        settings.beginGroup("mainWindow");
        settings.setValue("hide_app_on_startup",ui->checkBox->isChecked());
        settings.setValue("hide_app_on_close",ui->checkBox_8->isChecked());
        settings.setValue("log_to_file",ui->checkBox_9->isChecked());
        settings.setValue("app_tray_notification",ui->checkBox_12->isChecked());
        settings.setValue("chat_tray_notification",ui->checkBox_13->isChecked());
        settings.setValue("chat_sound",ui->checkBox_14->isChecked());
        settings.endGroup();

        settings.beginGroup("advanced");
        settings.setValue("resolve_ip",ui->checkBox_1->isChecked());
        settings.setValue("edge_tap_ip_mode",ui->comboBox->currentIndex());
        settings.setValue("local_port",ui->checkBox_2->isChecked());
        settings.setValue("edge_local_port",ui->lineEdit_8->text());
        settings.setValue("tap_mac_address",ui->checkBox_3->isChecked());
        settings.setValue("edge_tap_mac_address",ui->lineEdit_9->text());
        settings.setValue("mtu",ui->checkBox_4->isChecked());
        settings.setValue("edge_mtu",ui->lineEdit_10->text());
        settings.setValue("http_tunneling",ui->checkBox_5->isChecked());
        settings.setValue("packet_forwarding",ui->checkBox_6->isChecked());
        settings.setValue("verbose",ui->checkBox_7->isChecked());
        settings.setValue("edge_version",ui->radioButton->isChecked());
        settings.setValue("multicast",ui->checkBox_10->isChecked());
        settings.setValue("management",ui->checkBox_11->isChecked());
        settings.setValue("management_port",ui->lineEdit->text());
        settings.endGroup();

        settings.beginGroup("chat");
        settings.setValue("nickname",ui->lineEdit_2->text());
        settings.endGroup();

        ui->pushButton_apply->setEnabled(false);
        qDebug() << "advanced settings is saved";
      }
}

void Advanced::loadSettings()
{
    //load latest settings when app is start
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);

    settings.beginGroup("mainWindow");
    bool hide_app_on_startup = settings.value("hide_app_on_startup",false).toBool();
    ui->checkBox->setChecked(hide_app_on_startup);

    bool hide_app_on_close = settings.value("hide_app_on_close",false).toBool();
    ui->checkBox_8->setChecked(hide_app_on_close);

    bool log_to_file = settings.value("log_to_file",false).toBool();
    ui->checkBox_9->setChecked(log_to_file);

    bool app_tray_notification = settings.value("app_tray_notification",false).toBool();
    ui->checkBox_12->setChecked(app_tray_notification);

    bool chat_tray_notification = settings.value("chat_tray_notification",false).toBool();
    ui->checkBox_13->setChecked(chat_tray_notification);

    bool chat_sound = settings.value("chat_sound",false).toBool();
    ui->checkBox_14->setChecked(chat_sound);
    settings.endGroup();

    settings.beginGroup("advanced");
    bool resolve_ip = settings.value("resolve_ip",false).toBool();
    ui->checkBox_1->setChecked(resolve_ip);

    ui->comboBox->setCurrentIndex(settings.value("edge_tap_ip_mode",0).toInt());

    bool local_port = settings.value("local_port",false).toBool();
    ui->checkBox_2->setChecked(local_port);

    QString edge_local_port = settings.value("edge_local_port").toString();
    ui->lineEdit_8->setText(edge_local_port);

    bool tap_mac_address = settings.value("tap_mac_address",false).toBool();
    ui->checkBox_3->setChecked(tap_mac_address);

    QString edge_tap_mac_address = settings.value("edge_tap_mac_address").toString();
    ui->lineEdit_9->setText(edge_tap_mac_address);

    bool mtu = settings.value("mtu",false).toBool();
    ui->checkBox_4->setChecked(mtu);

    QString edge_mtu = settings.value("edge_mtu").toString();
    ui->lineEdit_10->setText(edge_mtu);

    bool http_tunneling = settings.value("http_tunneling",false).toBool();
    ui->checkBox_5->setChecked(http_tunneling);

    bool packet_forwarding = settings.value("packet_forwarding",false).toBool();
    ui->checkBox_6->setChecked(packet_forwarding);

    bool verbose = settings.value("verbose",false).toBool();
    ui->checkBox_7->setChecked(verbose);

    bool edge_version = settings.value("edge_version",false).toBool();
    ui->radioButton->setChecked(edge_version);

    bool multicast = settings.value("multicast",false).toBool();
    ui->checkBox_10->setChecked(multicast);

    bool management = settings.value("management",false).toBool();
    ui->checkBox_11->setChecked(management);
    QString management_port = settings.value("management_port").toString();
    ui->lineEdit->setText(management_port);
    settings.endGroup();

    settings.beginGroup("chat");
    QString nickname = settings.value("nickname").toString();
    ui->lineEdit_2->setText(nickname);
    settings.endGroup();

    if(edge_version == false)
      {
        ui->radioButton_2->setChecked(true);
        ui->lineEdit->setDisabled(true);
      }
    if(edge_version == true)
      {
        ui->comboBox->setEnabled(false);
        ui->checkBox_10->setEnabled(false);
        ui->checkBox_11->setEnabled(false);
        ui->lineEdit->setEnabled(false);
      }
    qDebug() << &"advanced settings is loaded" [ edge_version];
}

void Advanced::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Hide application on StartUp = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_8_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Hide application on Close = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_9_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Log Verbose to file = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_1_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Periodically resolve supernode IP Address = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_5_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Use http tunneling = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_6_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Enable packet forwarding through n2n community = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_7_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Increase edge Verbose = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Local port for connecting to supernode = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_3_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "MAC address for the TAP interface = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_4_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "MTU (Default: 1400) = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_10_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Enable multicast MAC addresses = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_11_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Management UDP Port (Default: 5645) = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_12_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Turn off app tray notification = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_13_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Turn off chat tray notification = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_checkBox_14_stateChanged(int arg1)
{
    if(arg1 == 2)
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Turn off chat sound = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_radioButton_2_toggled(bool checked)
{
    if(checked == true)
      {
        if(ui->checkBox_11->isChecked())
        if(ui->radioButton_2->isChecked())
          {
            ui->lineEdit->setEnabled(true);
          }
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    qDebug() << "v2 = " << checked;
}

void Advanced::on_comboBox_currentIndexChanged(int index)
{
    if(index == 1)
      {
        ui->checkBox_6->setChecked(true);
        ui->checkBox_6->setDisabled(true);
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    if(index == 0)
      {
        ui->checkBox_6->setEnabled(true);
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    qDebug() << "comboBox = " << index;
}

void Advanced::on_lineEdit_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
      {
        return;
      }
    else
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "5645 = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_lineEdit_2_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
        return;
      }
    else
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "Nickname: = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_lineEdit_8_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
      {
        return;
      }
    else
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "86 = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_lineEdit_9_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
      {
        return;
      }
    else
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "00:FF:06:62:95:52 = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_lineEdit_10_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
      {
        return;
      }
    else
      {
        ui->pushButton_apply->setEnabled(true);
        ui->pushButton_ok->setDisabled(true);
        qDebug() << "1400 = " << arg1;
      }
    QString nickname = ui->lineEdit_2->text();
    if(nickname.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_8 = ui->lineEdit_8->text();
    if(lineEdit_8.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_9 = ui->lineEdit_9->text();
    if(lineEdit_9.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit_10 = ui->lineEdit_10->text();
    if(lineEdit_10.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
    QString lineEdit = ui->lineEdit->text();
    if(lineEdit.isEmpty())
      {
        ui->pushButton_apply->setDisabled(true);
        ui->pushButton_ok->setDisabled(true);
      }
}

void Advanced::on_radioButton_clicked(bool checked)
{
    QString nickname = ui->lineEdit_2->text();
    if(checked == true)
      {
        if(nickname.isEmpty())
          {
            ui->pushButton_apply->setDisabled(true);
            ui->pushButton_ok->setDisabled(true);
          }
      }

}
