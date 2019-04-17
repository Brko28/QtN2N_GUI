#include "mainwindow.h"

#include "aboutdialog.h"
#include "advanced.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QThread>
#include <QCryptographicHash>
#include <QByteArray>
#include <QListWidget>
#include <QDebug>
#include <QIODevice>
#include <QSettings>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QTextTable>
#include <QScrollBar>
#include <QtNetwork>
#include <QRegExp>
#include <QSound>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    // if not running, skip
//    if (processes->getProcessState("edge.exe") == Processes::ProcessState::NotRunning) {
//        qDebug() << "[edge] Not running... Skipping kill command.";
//       }
//    if (processes->getProcessState("edge2.exe") == Processes::ProcessState::NotRunning) {
//        qDebug() << "[edge2] Not running... Skipping kill command.";
//       }
//    if (processes->getProcessState("supernode.exe") == Processes::ProcessState::NotRunning) {
//        qDebug() << "[supernode] Not running... Skipping kill command.";
//       }
//    if (processes->getProcessState("supernode2.exe") == Processes::ProcessState::NotRunning) {
//        qDebug() << "[supernode2] Not running... Skipping kill command.";
//       }
//    if (processes->getProcessState("chatserver.exe") == Processes::ProcessState::NotRunning) {
//        qDebug() << "[chatserver] Not running... Skipping kill command.";
//       }
    // if process already running
    if (processes->getProcessState("edge.exe") == Processes::ProcessState::Running) {
        QMessageBox::warning(nullptr, tr("edge.exe"), tr("edge.exe is already running.  \n\nKilling the process.\n"));
        processes->killProcessTree("edge.exe");
        processes->delay(100);
        qDebug() << "[edge] Running... Killing the process.";
       }
    if (processes->getProcessState("edge2.exe") == Processes::ProcessState::Running) {
        QMessageBox::warning(nullptr, tr("edge2.exe"), tr("edge2.exe is already running.  \n\nKilling the process.\n"));
        processes->killProcessTree("edge2.exe");
        processes->delay(100);
        qDebug() << "[edge2] Running... Killing the process.";
       }
    if (processes->getProcessState("supernode.exe") == Processes::ProcessState::Running) {
        QMessageBox::warning(nullptr, tr("supernode.exe"), tr("supernode.exe is already running.  \n\nKilling the process.\n"));
        processes->killProcessTree("supernode.exe");
        processes->delay(100);
        qDebug() << "[supernode] Running... Killing the process.";
       }
    if (processes->getProcessState("supernode2.exe") == Processes::ProcessState::Running) {
        QMessageBox::warning(nullptr, tr("supernode2.exe"), tr("supernode2.exe is already running.  \n\nKilling the process.\n"));
        processes->killProcessTree("supernode2.exe");
        processes->delay(100);
        qDebug() << "[supernode2] Running... Killing the process.";
       }
    if (processes->getProcessState("chatserver.exe") == Processes::ProcessState::Running) {
        QMessageBox::warning(nullptr, tr("chatserver.exe"), tr("chatserver.exe is already running.  \n\nKilling the process.\n"));
        processes->killProcessTree("chatserver.exe");
        processes->delay(100);
        qDebug() << "[chatserver] Running... Killing the process.";
       }

    QDir paths(QDir::currentPath());
    QString profiles = QDir::currentPath() + "/profiles/";
    if(!paths.exists(profiles))
      {
        paths.mkpath(profiles);
        qDebug() << "\n" << profiles + " <- folder is created \n";
      }

    QString logs = QDir::currentPath() + "/logs/";
    if(!paths.exists(logs))
      {
        paths.mkpath(logs);
        qDebug() << "\n" << logs + " <- folder is created \n";
      }

    QDir path(QDir::currentPath() + "/profiles/");
    QStringList files = path.entryList(QDir::Files);
    if(files.empty())
      {
        QMessageBox::information(this,"Missing Profile!","You don't have any profile.",QMessageBox::Ok);

        // create Default profile if files are is missing
        QString name = "Default.cfg";
        QSettings settings (QDir::currentPath() + "/profiles/" + name,QSettings::IniFormat);
        settings.beginGroup("network");
        settings.setValue("edge_ip",QString("88.86.108.50"));
        settings.setValue("edge_port",QString("86"));
        settings.setValue("edge_name",QString("games"));
        settings.setValue("edge_password",QString("games"));
        settings.setValue("edge_tap_ip",QString("192.168.10.10"));
        settings.setValue("edge_mask",QString("255.255.255.0"));
        settings.endGroup();

        settings.beginGroup("supernode");
        settings.setValue("supernode_port",QString("86"));
        settings.setValue("supernode_verbose",QString("false"));
        settings.endGroup();

        QMessageBox::information(this,"Creating Profile...","Default profile is created.",QMessageBox::Ok);

        ui->comboBox->addItem(name);
        ui->comboBox->show();
      }

    ui->comboBox->addItems(files);
    ui->comboBox->show();

    loadSettings();
    createActions();
    createTrayIcon();

    edge_process = new QProcess(this);
    edge_process->setReadChannel(QProcess::StandardOutput);
    edge_process->setReadChannel(QProcess::StandardError);
    connect(edge_process, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput_edge_process()));
    connect(edge_process, SIGNAL(readyReadStandardError()), this, SLOT(printOutput_edge_process()));

    sn_process = new QProcess(this);
    sn_process->setReadChannel(QProcess::StandardOutput);
    sn_process->setReadChannel(QProcess::StandardError);
    connect(sn_process, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput_sn_process()));
    connect(sn_process, SIGNAL(readyReadStandardError()), this, SLOT(printOutput_sn_process()));

    chat_server = new QProcess(this);
    chat_server->setReadChannel(QProcess::StandardOutput);
    chat_server->setReadChannel(QProcess::StandardError);
    connect(chat_server, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput_chat_server()));
    connect(chat_server, SIGNAL(readyReadStandardError()), this, SLOT(printOutput_chat_server()));

    cmd_process = new QProcess(this);
    cmd_process->setReadChannel(QProcess::StandardOutput);
    cmd_process->setReadChannel(QProcess::StandardError);
    connect(cmd_process, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput_cmd_process()));
    connect(cmd_process, SIGNAL(readyReadStandardError()), this, SLOT(printOutput_cmd_process()));

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));

    ui->lineEdit_1->setMaxLength(15);
    ui->lineEdit_2->setMaxLength(5);
    ui->lineEdit_3->setMaxLength(15);
    ui->lineEdit_4->setMaxLength(15);
    ui->lineEdit_5->setMaxLength(15);
    ui->lineEdit_6->setMaxLength(15);
    ui->lineEdit_7->setMaxLength(5);

    ui->userListWidget->setDisabled(true);
    ui->sayLineEdit->setDisabled(true);

    QIcon icon(":/GUI_n2n.png");
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("QtN2N GUI is here!");
    trayIcon->show();
    ui->roomTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    connect(ui->sayLineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    connect(ui->roomTextEdit, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(ShowContextMenu(const QPoint &)));

    QString server = ui->lineEdit_1->text();
    QString port = ui->lineEdit_2->text();
    QString network = ui->lineEdit_3->text();
    QString password = (ui->lineEdit_4->text());
    QString ip = ui->lineEdit_5->text();
    QString mask = ui->lineEdit_6->text();
    QString supernode = ui->lineEdit_7->text();
    if(server.isEmpty())
      {
        ui->lineEdit_1->setText("88.86.108.50");
      }
    if(port.isEmpty())
      {
        ui->lineEdit_2->setText("86");
      }
    if(network.isEmpty())
      {
        ui->lineEdit_3->setText("games");
      }
    if(password.isEmpty())
      {
        ui->lineEdit_4->setText("games");
      }
    if(ip.isEmpty())
      {
        ui->lineEdit_5->setText("192.168.10.10");
      }
    if(mask.isEmpty())
      {
        ui->lineEdit_6->setText("255.255.255.0");
      }
    if(supernode.isEmpty())
      {
        ui->lineEdit_7->setText("86");
      }

    QFile edge(QDir::currentPath() + "/edge.exe");
    if(!edge.exists())
      {
        QMessageBox::critical(this," Missing File! "," File <b>edge.exe</b> does not exist!<br></br><br></br><b>edge.exe</b> must be in the folder where is <b>n2n_gui.exe</b>.<br></br>",QMessageBox::Ok);
        ui->edgeConnectButton->setEnabled(false);
        ui->textEdit_1->moveCursor(QTextCursor::End);
        ui->textEdit_1->setTextColor(Qt::red);
        ui->textEdit_1->insertPlainText("ERROR:\nFile 'edge.exe' does not exist.\n");
        ui->textEdit_1->verticalScrollBar()->setValue(ui->textEdit_1->verticalScrollBar()->maximum());
      }

    QFile edge2(QDir::currentPath() + "/edge2.exe");
    if(!edge2.exists())
      {
        QMessageBox::critical(this," Missing File! "," File <b>edge2.exe</b> does not exist!<br></br><br></br><b>edge2.exe</b> must be in the folder where is <b>n2n_gui.exe</b>.<br></br>",QMessageBox::Ok);
        ui->edgeConnectButton->setEnabled(false);
        ui->textEdit_1->moveCursor(QTextCursor::End);
        ui->textEdit_1->setTextColor(Qt::red);
        ui->textEdit_1->insertPlainText( "ERROR:\nFile 'edge2.exe' does not exist.\n");
        ui->textEdit_1->verticalScrollBar()->setValue(ui->textEdit_1->verticalScrollBar()->maximum());
      }

    QFile sn(QDir::currentPath() + "/supernode.exe");
    if(!sn.exists())
      {
        QMessageBox::critical(this," Missing File! "," File <b>supernode.exe</b> does not exist!<br></br><br></br><b>supernode.exe</b> must be in the folder where is <b>n2n_gui.exe</b>.<br></br>",QMessageBox::Ok);
        ui->nodeStartButton->setEnabled(false);
        ui->textEdit_2->moveCursor(QTextCursor::End);
        ui->textEdit_2->setTextColor(Qt::red);
        ui->textEdit_2->insertPlainText("ERROR:\nFile 'supernode.exe' does not exist.\n");
        ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
      }

    QFile sn2(QDir::currentPath() + "/supernode2.exe");
    if(!sn2.exists())
      {
        QMessageBox::critical(this," Missing File! "," File <b>supernode2.exe</b> does not exist!<br></br><br></br><b>supernode2.exe</b> must be in the folder where is <b>n2n_gui.exe</b>.<br></br>",QMessageBox::Ok);
        ui->nodeStartButton->setEnabled(false);
        ui->textEdit_2->moveCursor(QTextCursor::End);
        ui->textEdit_2->setTextColor(Qt::red);
        ui->textEdit_2->insertPlainText("ERROR:\nFile 'supernode2.exe' does not exist.\n");
        ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
      }

    QFile chtsvr(QDir::currentPath() + "/chatserver.exe");
    if(!chtsvr.exists())
      {
        QMessageBox::critical(this," Missing File! "," File <b>chatserver.exe</b> does not exist!<br></br><br></br><b>chatserver.exe</b> must be in the folder where is <b>n2n_gui.exe</b>.<br></br>",QMessageBox::Ok);
        ui->nodeStartButton->setEnabled(false);
        ui->textEdit_2->moveCursor(QTextCursor::End);
        ui->textEdit_2->setTextColor(Qt::red);
        ui->textEdit_2->insertPlainText("ERROR:\nFile 'chatserver.exe' does not exist.\n");
        ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
      }

    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/hide_app_on_startup",false).toBool() == true)
      {
        QTimer::singleShot(05, this, SLOT(hide()));
        qDebug() << "\n" << "Application hide on startup";
      }
    ui->lineEdit_2->setToolTip("<b>n2n v2.0.0</b><ul><li>Default: 7654</li><li>Default: 86 for vpnhosting.cz</li></ul><b>n2n v1.3.3</b><ul><li>Default: 7654</li><li>Default: 82 for vpnhosting.cz</li></ul>");

    if(settings.value("advanced/edge_tap_ip_mode",0).toInt() == 1)
    if(settings.value("advanced/edge_version",false).toBool() == false)
      {
        ui->lineEdit_5->setText("0.0.0.0");
        ui->lineEdit_5->setDisabled(true);
      }
    if(settings.value("advanced/edge_tap_ip_mode",0).toInt() == 0)
    if(settings.value("advanced/edge_version",false).toBool() == false)
      {
        ui->lineEdit_5->setEnabled(true);
        ui->comboBox->setCurrentIndex(settings.value("network/edge_profile",0).toInt());
      }
    if(settings.value("chat/nickname").toString() == nullptr)
      {
        settings.beginGroup("chat");
        settings.setValue("nickname",QString("unknown"));
        settings.endGroup();
      }
    ui->sayLineEdit->setFocusPolicy(Qt::StrongFocus);
    ui->userListWidget->setFocusPolicy(Qt::NoFocus);
    timer = new QTimer(this);
    copyAct->setDisabled(true);
    selectAllAct->setDisabled(true);
    clearAct->setDisabled(true);
}

void MainWindow::setVisible(bool visible)
{
    minimizeAction->setVisible(visible);
    restoreAction->setVisible(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete edge_process;
    delete sn_process;
    delete chat_server;
    delete cmd_process;
}

void MainWindow::printOutput_edge_process(void)
{
    QByteArray StandardOutput = edge_process->readAllStandardOutput();
    QByteArray StandardError = edge_process->readAllStandardError();
    ui->textEdit_1->moveCursor(QTextCursor::End);
    ui->textEdit_1->setTextColor(Qt::red);
    ui->textEdit_1->insertPlainText(StandardOutput);
    ui->textEdit_1->insertPlainText(StandardError);
    ui->textEdit_1->verticalScrollBar()->setValue(ui->textEdit_1->verticalScrollBar()->maximum());

    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/log_to_file",false).toBool() == true)
      {
        QFile edge_log(QDir::currentPath() + "/logs/" + "edge.log");
        if(!edge_log.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
      {
        edge_log.write(StandardOutput);
        edge_log.flush();
        edge_log.write(StandardError);
        edge_log.flush();
      }
      }
    qDebug() << "edge: " << StandardOutput;
    qDebug() << "edge: " << StandardError;
}

void MainWindow::printOutput_sn_process(void)
{
    QByteArray StandardOutput = sn_process->readAllStandardOutput();
    QByteArray StandardError = sn_process->readAllStandardError();
    ui->textEdit_2->moveCursor(QTextCursor::End);
    ui->textEdit_2->setTextColor(Qt::red);
    ui->textEdit_2->insertPlainText(StandardOutput);
    ui->textEdit_2->insertPlainText(StandardError);
    ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());

    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/log_to_file",false).toBool() == true)
      {
        QFile supernode_log(QDir::currentPath() + "/logs/" + "supernode.log");
        if(!supernode_log.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
      {
        supernode_log.write(StandardOutput);
        supernode_log.flush();
        supernode_log.write(StandardError);
        supernode_log.flush();
      }
      }
    qDebug() << "supernode: " << StandardOutput;
    qDebug() << "supernode: " << StandardError;
}

void MainWindow::printOutput_chat_server(void)
{
    QByteArray StandardOutput = chat_server->readAllStandardOutput();
    QByteArray StandardError = chat_server->readAllStandardError();
    ui->textEdit_2->moveCursor(QTextCursor::End);
    ui->textEdit_2->setTextColor(Qt::red);
    ui->textEdit_2->insertPlainText(StandardOutput);
    ui->textEdit_2->insertPlainText(StandardError);
    ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());

    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/log_to_file",false).toBool() == true)
      {
        QFile chat_server_log(QDir::currentPath() + "/logs/" + "chat_server.log");
        if(!chat_server_log.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
      {
        chat_server_log.write(StandardOutput);
        chat_server_log.flush();
        chat_server_log.write(StandardError);
        chat_server_log.flush();
      }
      }
    qDebug() << "\n chat_server: " << StandardOutput;
    qDebug() << "\n chat_server: " << StandardError;
}

void MainWindow::printOutput_cmd_process(void)
{
    QByteArray StandardOutput = cmd_process->readAllStandardOutput();
    QByteArray StandardError = cmd_process->readAllStandardError();
    ui->textEditInfo->moveCursor(QTextCursor::End);
    ui->textEditInfo->setTextColor(Qt::red);
    ui->textEditInfo->insertPlainText(StandardOutput);
    ui->textEditInfo->insertPlainText(StandardError);
    ui->textEditInfo->insertPlainText("\n");
    ui->textEditInfo->verticalScrollBar()->setValue(ui->textEditInfo->verticalScrollBar()->maximum());

    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/log_to_file",false).toBool() == true)
      {
        QFile cmd_process_log(QDir::currentPath() + "/logs/" + "tap-adapter.log");
        if(!cmd_process_log.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
      {
        cmd_process_log.write(StandardOutput);
        cmd_process_log.flush();
        cmd_process_log.write(StandardError);
        cmd_process_log.flush();
      }
      }
    qDebug() << "\n cmd_process_log: " << StandardOutput;
    qDebug() << "\n cmd_process_log: " << StandardError;
}

static bool task(QString server,QString port,QString network,QString password,QString ip,QString mask)
{
    if(server == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing Server IP!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(port == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing Server PORT!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(network == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing Network NAME!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(password == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Password is missing!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(ip == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing TAP-Adapter IP address!"), QString::fromLocal8Bit("The field can not be empty!"));
        return false;
      }
    else
      {
    if(mask == "")
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing Network MASK!"), QString::fromLocal8Bit("The field can not be empty!"));
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
}

void MainWindow::on_actionHelp_triggered()
{
    QFile help(QDir::currentPath() + "/n2n_gui.chm");
    if(!help.open(QIODevice::ReadOnly | QIODevice::NotOpen | QIODevice::Text))
      {
        QMessageBox::critical(nullptr, QString::fromLocal8Bit("Missing Help File!"), QString::fromLocal8Bit("Missing <b>n2n_gui.chm</b> Help File!"));
        return;
      }
    else
      {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + "/n2n_gui.chm"));
      }
}

void MainWindow::on_actionAbout_triggered()
{
    //Show About dialog
    AboutDialog aboutdialog;
    aboutdialog.isModal();
    aboutdialog.exec();
}

void MainWindow::on_actionAboutQt_triggered()
{
    //Show About Qt dialog
    QMessageBox::aboutQt(this, tr("About Application"));
}

void MainWindow::on_actionAdvanced_triggered()
{
    //Show Advanced Settings dialog
    Advanced advanced;
    advanced.isModal();
    advanced.exec();
    if(advanced.close())
      {
        QTimer::singleShot(05, this, SLOT(edge_time()));
        qDebug() << "advanced window is closed";
      }
}

void MainWindow::on_actionExit_triggered()
{
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/hide_app_on_close",false).toBool() == true)
      {
        QApplication::setQuitOnLastWindowClosed(true);
      }
    close();
}

void MainWindow::on_actionSave_triggered()
{
    //save connection to profile
    QString server = ui->lineEdit_1->text();
    QString port = ui->lineEdit_2->text();
    QString network = ui->lineEdit_3->text();
    QString password = ui->lineEdit_4->text();
    QString ip = ui->lineEdit_5->text();
    QString mask = ui->lineEdit_6->text();

    if(task(server,port,network,password,ip,mask))
      {
        QString name;
        for (int i = 0; i < 1; i--)
        {
            bool ok = true;
            name = QInputDialog::getText(this, QString::fromLocal8Bit("File Name?"), QString::fromLocal8Bit("Enter a name to save:"), QLineEdit::Normal,QString::null, &ok, Qt::MSWindowsFixedSizeDialogHint);
            if(ok)
              {
            if(name.isEmpty())
              {
                QMessageBox::warning(nullptr, QString::fromLocal8Bit("I guess!"), QString::fromLocal8Bit("You did not enter a name!"));
              }
            else
              {
                i=2;
              }
              }
            else
              {
                return;
              }
        }
        name = name + ".cfg";
        QString home = QDir::currentPath() + "/profiles/";

        QDir path;
        if(!path.exists(home))
          {
            path.mkpath(home);
          }

        QSettings profile (QDir::currentPath() + "/profiles/" + name,QSettings::IniFormat);
        profile.beginGroup("network");
        profile.setValue("edge_ip",ui->lineEdit_1->text());
        profile.setValue("edge_port",ui->lineEdit_2->text());
        profile.setValue("edge_name",ui->lineEdit_3->text());
        profile.setValue("edge_password",ui->lineEdit_4->text());
        profile.setValue("edge_tap_ip",ui->lineEdit_5->text());
        profile.setValue("edge_mask",ui->lineEdit_6->text());
        profile.endGroup();

        profile.beginGroup("supernode");
        profile.setValue("supernode_port",ui->lineEdit_7->text());
        profile.setValue("supernode_verbose",ui->checkBox_0->isChecked());
        profile.endGroup();

        ui->comboBox->addItem(name);
        ui->comboBox->show();
        ui->statusBar->showMessage("Profile is saved...");

        QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
            if(settings.value("mainWindow/app_tray_notification",false).toBool() == false)
              {
                trayIcon->showMessage("QtN2N GUI","Profile is saved...");
                qDebug() << "\n" << "Turn off app tray notification.";
              }
    }
}

void MainWindow::on_edgeConnectButton_clicked()
{
    //Start edge connection
    ui->textEdit_1->clear();
    ui->roomTextEdit->clear();

    QString server = ui->lineEdit_1->text();
    QString port = ui->lineEdit_2->text();
    QString network = ui->lineEdit_3->text();
    QString password = (ui->lineEdit_4->text());
    QString ip = ui->lineEdit_5->text();
    QString mask = ui->lineEdit_6->text();

    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);

    QString edge_tap_ip_mode;
    if(settings.value("advanced/edge_tap_ip_mode",0).toInt() == 0)
    if(settings.value("advanced/edge_version",false).toBool() == false)
      {
        edge_tap_ip_mode = "static:";
      }

    if(settings.value("advanced/edge_tap_ip_mode",0).toInt() == 1)
    if(settings.value("advanced/edge_version",false).toBool() == false)
      {
        edge_tap_ip_mode = "dhcp:";
      }

    QString multicast;
    if(settings.value("advanced/multicast",false).toBool() == true)
    if(settings.value("advanced/edge_version",false).toBool() == false)
      {
        multicast = " -E";
      }

    QString management_port;
    if(settings.value("advanced/management",false).toBool() == true)
    if(settings.value("advanced/edge_version",false).toBool() == false)
      {
        management_port = " -t " + settings.value("advanced/management_port").toString();
      }

    QString edge_http_tunneling;
    if(settings.value("advanced/http_tunneling",false).toBool() == true)
    if(settings.value("advanced/edge_version",false).toBool() == true)
      {
        edge_http_tunneling = " -t";
      }

    QString verbose;
    if(settings.value("advanced/verbose",false).toBool() == true)
      {
        verbose = " -v";
      }

    QString edge_resolve_ip;
    if(settings.value("advanced/resolve_ip",false).toBool() == true)
      {
        edge_resolve_ip = " -b";
      }

    QString edge_packet_forwarding;
    if(settings.value("advanced/packet_forwarding",false).toBool() == true)
      {
        edge_packet_forwarding = " -r";
      }

    QString edge_local_port;
    if(settings.value("advanced/local_port",false).toBool() == true)
      {
        edge_local_port = " -p " + settings.value("advanced/edge_local_port").toString();
      }

    QString edge_tap_mac_address;
    if(settings.value("advanced/tap_mac_address",false).toBool() == true)
      {
        edge_tap_mac_address = " -m " + settings.value("advanced/edge_tap_mac_address").toString();
      }

    QString edge_mtu;
    if(settings.value("advanced/mtu",false).toBool() == true)
      {
        edge_mtu = " -M " + settings.value("advanced/edge_mtu").toString();
      }

    QString commands =
            " -a " + edge_tap_ip_mode + ip + " -c " + network + " -k " + password + " -s " + mask + edge_tap_mac_address + " -l " + server + ":" + port
            + edge_local_port + edge_mtu + edge_http_tunneling + edge_packet_forwarding + verbose + edge_resolve_ip + management_port + multicast;

    if(task(server,port,network,password,ip,mask))
    {
        ui->comboBox->setEnabled(false);
        ui->lineEdit_1->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->lineEdit_3->setEnabled(false);
        ui->lineEdit_4->setEnabled(false);
        ui->lineEdit_5->setEnabled(false);
        ui->lineEdit_6->setEnabled(false);
        ui->edgeConnectButton->setEnabled(false);
        ui->edgeDisconnectButton->setEnabled(true);
        ui->actionAdvanced->setDisabled(true);
        advancedAction->setDisabled(true);
        ui->actionSave->setDisabled(true);
        ui->statusBar->showMessage("edge is started...");
        ui->textEdit_1->moveCursor(QTextCursor::End);
        ui->textEdit_1->setTextColor(Qt::blue);
        ui->textEdit_1->insertPlainText("\nedge is started...\n\n");
        ui->textEdit_1->verticalScrollBar()->setValue(ui->textEdit_1->verticalScrollBar()->maximum());

        QString edge_exe;
        QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
        if(settings.value("advanced/edge_version",false).toBool() == true)
          {
            QString edge_message = "You are Running N2N edge v1.3.3...\n\n";
            edge_exe = "edge.exe";
            if(settings.value("mainWindow/app_tray_notification",false).toBool() == false)
              {
                trayIcon->showMessage("QtN2N GUI","edge is started...\nYou are Running N2N edge v1.3.3\n\n");
                qDebug() << "\n" << "Turn off app tray notification.";
              }
            ui->textEdit_1->moveCursor(QTextCursor::End);
            ui->textEdit_1->append("<b>" + edge_message + "</b>");
            ui->textEdit_1->append("\n\n");
            ui->textEdit_1->verticalScrollBar()->setValue(ui->textEdit_1->verticalScrollBar()->maximum());
          }
        if(settings.value("advanced/edge_version",false).toBool() == false)
          {
            QString edge_message = "You are Running N2N edge v2.0.0...\n\n";
            edge_exe = "edge2.exe";
            if(settings.value("mainWindow/app_tray_notification",false).toBool() == false)
              {
                trayIcon->showMessage("QtN2N GUI","edge is started...\nYou are Running N2N edge v2.0.0\n\n");
                qDebug() << "\n" << "Turn off app tray notification.";
              }
            ui->textEdit_1->moveCursor(QTextCursor::End);
            ui->textEdit_1->append("<b>" + edge_message + "</b>");
            ui->textEdit_1->append("\n\n");
            ui->textEdit_1->verticalScrollBar()->setValue(ui->textEdit_1->verticalScrollBar()->maximum());
          }

        edge_process->start(edge_exe + QString(commands));
        if(edge_process->state() == QProcess::NotRunning)
          {
            timer->stop();
            on_edgeDisconnectButton_clicked();
            ui->textEdit_1->clear();
            ui->textEdit_1->setTextColor(Qt::red);
            ui->textEdit_1->append("\n ERROR: \n");
            ui->textEdit_1->setTextColor(Qt::blue);
            ui->textEdit_1->append(" N2N edge not running \n\n");
            ui->textEdit_1->moveCursor(QTextCursor::End);
            ui->textEdit_1->verticalScrollBar()->setValue(ui->textEdit_1->verticalScrollBar()->maximum());
            qDebug() << edge_process << "not running";
          }
        connect(timer, SIGNAL(timeout()),this, SLOT(on_connectToHost()));
        timer->start(10000);
        qDebug() << edge_exe + commands;
        ui->tabInfo->setCurrentIndex(2);
    }
}

void MainWindow::on_edgeDisconnectButton_clicked()
{
    //stop edge connection
    timer->stop();
    ui->comboBox->setEnabled(true);
    ui->lineEdit_1->setEnabled(true);
    ui->lineEdit_2->setEnabled(true);
    ui->lineEdit_3->setEnabled(true);
    ui->lineEdit_4->setEnabled(true);
    ui->lineEdit_5->setEnabled(true);
    ui->lineEdit_6->setEnabled(true);
    ui->edgeConnectButton->setEnabled(true);
    ui->edgeDisconnectButton->setEnabled(false);
    ui->actionAdvanced->setDisabled(false);
    advancedAction->setDisabled(false);
    ui->actionSave->setDisabled(false);
    ui->statusBar->showMessage("edge is stopped...");
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/app_tray_notification",false).toBool() == false)
      {
        trayIcon->showMessage("QtN2N GUI","edge is stopped...");
        qDebug() << "\n" << "Turn off app tray notification.";
      }
    ui->textEdit_1->moveCursor(QTextCursor::End);
    ui->textEdit_1->setTextColor(Qt::magenta);
    ui->textEdit_1->insertPlainText("\nedge is stopped...\n\n");
    ui->textEdit_1->verticalScrollBar()->setValue(ui->textEdit_1->verticalScrollBar()->maximum());
    edge_process->kill();
    socket->close();
    ui->userListWidget->setDisabled(true);
    ui->sayLineEdit->setDisabled(true);
    ui->userListWidget->clear();
    qDebug() << "edge is stopped...";
    QString server_messege = "Disconnect from Chat!";
    ui->roomTextEdit->moveCursor(QTextCursor::End);
    ui->roomTextEdit->setTextColor(Qt::blue);
    ui->roomTextEdit->insertPlainText("\n" + server_messege);
    ui->roomTextEdit->verticalScrollBar()->setValue(ui->roomTextEdit->verticalScrollBar()->maximum());
}

void MainWindow::on_nodeStartButton_clicked()
{
    //start Supernode connection
    ui->textEdit_2->clear();

    QString port = ui->lineEdit_7->text();

    QString supernode_log;
    if(ui->checkBox_0->isChecked())
      {
        supernode_log = " -v";
      }

    QString commands = " -l " + port + supernode_log;
    if(port.isEmpty())
      {
        QMessageBox::warning(nullptr, QString::fromLocal8Bit("Missing Supernode PORT!"), QString::fromLocal8Bit("The field can not be empty!"));
      }
    else
      {
        ui->lineEdit_7->setEnabled(false);
        ui->nodeStartButton->setEnabled(false);
        ui->nodeStopButton->setEnabled(true);
        ui->checkBox_0->setEnabled(false);
        ui->actionAdvanced->setDisabled(true);
        advancedAction->setDisabled(true);
        ui->actionSave->setDisabled(true);
        ui->statusBar->showMessage("supernode is running...");
        ui->textEdit_2->moveCursor(QTextCursor::End);
        ui->textEdit_2->setTextColor(Qt::blue);
        ui->textEdit_2->insertPlainText("\nsupernode is running...\n\n");
        ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());

        QString supernode_exe;
        QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
        if(settings.value("advanced/edge_version",false).toBool() == true)
          {
            QString supernode_message = "You are Running N2N supernode v1.3.3...";
            supernode_exe = "supernode.exe";
            if(settings.value("mainWindow/app_tray_notification",false).toBool() == false)
              {
                trayIcon->showMessage("QtN2N GUI","supernode is running...\nYou are Running N2N supernode v1.3.3\n\n");
                qDebug() << "\n" << "Turn off app tray notification.";
              }
            ui->textEdit_2->moveCursor(QTextCursor::End);
            ui->textEdit_2->append("<b>" + supernode_message + "</b>");
            ui->textEdit_2->append("\n\n");
            ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
          }
        if(settings.value("advanced/edge_version",false).toBool() == false)
          {
            QString supernode_message = "You are Running N2N supernode v2.0.0...";
            supernode_exe = "supernode2.exe";
            if(settings.value("mainWindow/app_tray_notification",false).toBool() == false)
              {
                trayIcon->showMessage("QtN2N GUI","supernode is running...\nYou are Running N2N supernode v2.0.0\n\n");
                qDebug() << "\n" << "Turn off app tray notification.";
              }
            ui->textEdit_2->moveCursor(QTextCursor::End);
            ui->textEdit_2->append("<b>" + supernode_message + "</b>");
            ui->textEdit_2->append("\n\n");
            ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
          }

        sn_process->start(supernode_exe + QString(commands));
        if(sn_process->state() == QProcess::NotRunning)
          {
            on_nodeStopButton_clicked();
            ui->textEdit_2->clear();
            ui->textEdit_2->setTextColor(Qt::red);
            ui->textEdit_2->append("\n ERROR: \n");
            ui->textEdit_2->setTextColor(Qt::blue);
            ui->textEdit_2->append(" N2N supernode not running \n\n");
            ui->textEdit_2->moveCursor(QTextCursor::End);
            ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
            qDebug() << sn_process << "not running";
          }

        chat_server->start("chatserver.exe");
        if(chat_server->state() == QProcess::NotRunning)
          {
            on_nodeStopButton_clicked();
            ui->textEdit_2->clear();
            ui->textEdit_2->setTextColor(Qt::red);
            ui->textEdit_2->append("\n ERROR: \n");
            ui->textEdit_2->setTextColor(Qt::blue);
            ui->textEdit_2->append(" Chat server not running \n\n");
            ui->textEdit_2->moveCursor(QTextCursor::End);
            ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
            qDebug() << chat_server << "not running";
          }
         bool server = (chat_server->waitForFinished(2000) == false);
         if(server == 0)
           {
             timer->stop();
             on_nodeStopButton_clicked();
             ui->textEdit_2->clear();
             ui->textEdit_2->setTextColor(Qt::red);
             ui->textEdit_2->append("\n ERROR: \n");
             ui->textEdit_2->setTextColor(Qt::blue);
             ui->textEdit_2->append(" Chat server not running, it may be that port is busy. \n\n");
             ui->textEdit_2->moveCursor(QTextCursor::End);
             ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
             qDebug() << chat_server << "not running";
           }
         qDebug() << "supernode: " << supernode_exe + commands;
         qDebug() << "chat server is running... ";
      }
}

void MainWindow::on_nodeStopButton_clicked()
{
    //stop Supernode connection
    ui->lineEdit_7->setEnabled(true);
    ui->nodeStartButton->setEnabled(true);
    ui->nodeStopButton->setEnabled(false);
    ui->checkBox_0->setEnabled(true);
    ui->actionAdvanced->setDisabled(false);
    advancedAction->setDisabled(false);
    ui->actionSave->setDisabled(false);
    ui->statusBar->showMessage("supernode is stopped...");
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/app_tray_notification",false).toBool() == false)
      {
        trayIcon->showMessage("QtN2N GUI","supernode is stopped...");
        qDebug() << "\n" << "Turn off app tray notification.";
      }
    ui->textEdit_2->moveCursor(QTextCursor::End);
    ui->textEdit_2->setTextColor(Qt::magenta);
    ui->textEdit_2->insertPlainText("\nsupernode is stopped...\n\n");
    ui->textEdit_2->verticalScrollBar()->setValue(ui->textEdit_2->verticalScrollBar()->maximum());
    sn_process->kill();
    chat_server->kill();

    qDebug("supernode is stopped...");
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    //load settings from profile cfg file
    QSettings profile (QDir::currentPath() + "/profiles/" + arg1, QSettings::IniFormat);
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);

    profile.beginGroup("network");
    QString edge_ip = profile.value("edge_ip").toString();
    ui->lineEdit_1->setText(edge_ip);
    QString edge_port = profile.value("edge_port").toString();
    ui->lineEdit_2->setText(edge_port);
    QString edge_name = profile.value("edge_name").toString();
    ui->lineEdit_3->setText(edge_name);
    QString edge_password = profile.value("edge_password").toString();
    ui->lineEdit_4->setText(edge_password);

    QString edge_tap_ip;
    if(settings.value("advanced/edge_tap_ip_mode",0).toInt() == 1)
      {
        edge_tap_ip = "0.0.0.0";
      }
    if(settings.value("advanced/edge_tap_ip_mode",0).toInt() == 0)
      {
        edge_tap_ip = profile.value("edge_tap_ip").toString();
      }
    ui->lineEdit_5->setText(edge_tap_ip);

    QString edge_mask = profile.value("edge_mask").toString();
    ui->lineEdit_6->setText(edge_mask);
    profile.endGroup();

    profile.beginGroup("supernode");
    QString supernode_port = profile.value("supernode_port").toString();
    ui->lineEdit_7->setText(supernode_port);
    bool supernode_verbose = profile.value("supernode_verbose",false).toBool();
    ui->checkBox_0->setChecked(supernode_verbose);
    profile.endGroup();

    ui->statusBar->showMessage(arg1);

    qDebug() << "ComboBox load: " << arg1;
}

void MainWindow::loadSettings()
{
    QFile file(QDir::currentPath() + "/n2n_gui.ini");

    if(!file.exists())
      {
        // create default ini file if ini file is missing.
        QMessageBox::information(this, " Missing File! ", " File <b>n2n_gui.ini</b> does not exist! ", QMessageBox::Ok);

        QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);

        settings.beginGroup("mainWindow");
        settings.setValue("geometry",saveGeometry());
        settings.setValue("state",saveState());
        settings.setValue("hide_app_on_startup",QString("false"));
        settings.setValue("hide_app_on_close",QString("false"));
        settings.setValue("log_to_file",QString("true"));
        settings.endGroup();

        settings.beginGroup("network");
        settings.setValue("edge_profile",QString("0"));
        settings.setValue("edge_ip",QString("88.86.108.50"));
        settings.setValue("edge_port",QString("86"));
        settings.setValue("edge_name",QString("games"));
        settings.setValue("edge_password",QString("games"));
        settings.setValue("edge_tap_ip",QString("192.168.10.10"));
        settings.setValue("edge_mask",QString("255.255.255.0"));
        settings.endGroup();

        settings.beginGroup("supernode");
        settings.setValue("supernode_port",QString("86"));
        settings.setValue("supernode_verbose",QString("false"));
        settings.endGroup();

        settings.beginGroup("advanced");
        settings.setValue("resolve_ip",QString("false"));
        settings.setValue("edge_tap_ip_mode",QString("0"));
        settings.setValue("local_port",QString("false"));
        settings.setValue("edge_local_port",QString("86"));
        settings.setValue("tap_mac_address",QString("false"));
        settings.setValue("edge_tap_mac_address",QString("00:FF:06:62:95:52"));
        settings.setValue("mtu",QString("false"));
        settings.setValue("edge_mtu",QString("1400"));
        settings.setValue("http_tunneling",QString("false"));
        settings.setValue("packet_forwarding",QString("true"));
        settings.setValue("verbose",QString("false"));
        settings.setValue("edge_version",QString("false"));
        settings.setValue("multicast",QString("true"));
        settings.setValue("management",QString("false"));
        settings.setValue("management_port",QString("5645"));
        settings.endGroup();

        settings.beginGroup("chat");
        settings.setValue("nickname",QString("unknown"));
        settings.endGroup();

        QMessageBox::information(this, " Creating File... ", " New file <b>n2n_gui.ini</b> is created. ", QMessageBox::Ok);

        qDebug() << "\n" << "New file <b>n2n_gui.ini</b> is created.";
      }
    else
      {
        //load latest settings when app is start
        QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);

        settings.beginGroup("mainWindow");
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("state").toByteArray());
        settings.endGroup();

        settings.beginGroup("network");
        QString edge_ip = settings.value("edge_ip").toString();
        ui->lineEdit_1->setText(edge_ip);
        QString edge_port = settings.value("edge_port").toString();
        ui->lineEdit_2->setText(edge_port);
        QString edge_name = settings.value("edge_name").toString();
        ui->lineEdit_3->setText(edge_name);
        QString edge_password = settings.value("edge_password").toString();
        ui->lineEdit_4->setText(edge_password);
        QString edge_tap_ip = settings.value("edge_tap_ip").toString();
        ui->lineEdit_5->setText(edge_tap_ip);
        QString edge_mask = settings.value("edge_mask").toString();
        ui->lineEdit_6->setText(edge_mask);
        ui->comboBox->setCurrentIndex(settings.value("edge_profile",0).toInt());
        settings.endGroup();

        settings.beginGroup("supernode");
        QString supernode_port = settings.value("supernode_port").toString();
        ui->lineEdit_7->setText(supernode_port);
        bool supernode_verbose = settings.value("supernode_verbose",false).toBool();
        ui->checkBox_0->setChecked(supernode_verbose);
        settings.endGroup();

        qDebug() << "loaded application settings";
      }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    QSettings settings_tap_ip (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("mainWindow/hide_app_on_close",false).toBool() == false)
      {
        int reponse = QMessageBox::question(this, "Close Confirmation?",
                                           "Are you sure you want to exit?<br></br><br></br>If you click <b>no</b>, "
                                           "the application will be minimized to the system tray.<br></br>"
                                           "To terminate the program, choose <b>Quit</b> in the context menu of the system tray entry.<br></br><br></br>"
                                           "If you no longer want to receive this notification, turn it off in <b>Advanced Settings</b>.<br></br>",
                                           QMessageBox::Yes|QMessageBox::No);
        if(reponse == QMessageBox::Yes)
          {
            MainWindow::close();
            QApplication::setQuitOnLastWindowClosed(true);
          }
        else if(reponse == QMessageBox::No)
          {
            event->ignore();
            MainWindow::hide();
          }
      }
    //save settings on app exit
    settings.beginGroup("mainWindow");
    settings.setValue("geometry",saveGeometry());
    settings.setValue("state",saveState());
    settings.endGroup();

    settings.beginGroup("network");
    settings.setValue("edge_profile",ui->comboBox->currentIndex());
    settings.setValue("edge_ip",ui->lineEdit_1->text());
    settings.setValue("edge_port",ui->lineEdit_2->text());
    settings.setValue("edge_name",ui->lineEdit_3->text());
    settings.setValue("edge_password",ui->lineEdit_4->text());

    if(settings_tap_ip.value("advanced/edge_tap_ip_mode",0).toInt() == 0)
      {
        settings.setValue("edge_tap_ip",ui->lineEdit_5->text());
      }

    settings.setValue("edge_mask",ui->lineEdit_6->text());
    settings.endGroup();

    settings.beginGroup("supernode");
    settings.setValue("supernode_port",ui->lineEdit_7->text());
    settings.setValue("supernode_verbose",ui->checkBox_0->isChecked());
    settings.endGroup();

    qDebug() << "saved application settings on CloseEvent";
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
        if(MainWindow::isVisible())
          {
            MainWindow::hide();
          }
        else if(MainWindow::isHidden())
          {
            MainWindow::show();
          }
        break;
    case QSystemTrayIcon::MiddleClick:
        MainWindow::hide();
        break;
    default:
        ;
    }
}

void MainWindow::createActions()
{
    helpAction = new QAction(tr("&Help"), this);
    connect(helpAction,SIGNAL(triggered()), this, SLOT(on_actionHelp_triggered()));

    aboutAction = new QAction(tr("A&bout"), this);
    connect(aboutAction,SIGNAL(triggered()), this, SLOT(on_actionAbout_triggered()));

    aboutQtAction = new QAction(tr("A&bout Qt"), this);
    connect(aboutQtAction,SIGNAL(triggered()), this, SLOT(on_actionAboutQt_triggered()));

    advancedAction = new QAction(tr("A&dvanced Settings"), this);
    connect(advancedAction,SIGNAL(triggered()), this, SLOT(on_actionAdvanced_triggered()));

    restoreAction = new QAction(tr("&Show"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    minimizeAction = new QAction(tr("&Hide"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT (save_on_app_quit()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    //customContextMenu
    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setShortcutContext(Qt::WindowShortcut);
    copyAct->setStatusTip(tr("Copy the current selected contents to the ""clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

    selectAllAct = new QAction(tr("Se&lect All"), this);
    selectAllAct->setShortcuts(QKeySequence::SelectAll);
    selectAllAct->setShortcutContext(Qt::WindowShortcut);
    selectAllAct->setStatusTip(tr("Select All Text"));
    connect(selectAllAct, &QAction::triggered, this, &MainWindow::selectAll);

    clearAct = new QAction(tr("Cl&ear Messages"), this);
    clearAct->setShortcuts(QKeySequence::Delete);
    clearAct->setShortcutContext(Qt::WindowShortcut);
    clearAct->setStatusTip(tr("Clear the current Chat Messages"));
    clearAct->setIcon(QIcon(":/161.png"));
    connect(clearAct, &QAction::triggered, this, &MainWindow::clear);
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(helpAction);
    helpAction->setIcon(QIcon(":/help.png"));

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(aboutAction);
    aboutAction->setIcon(QIcon(":/GUI_n2n.png"));

    trayIconMenu->addAction(aboutQtAction);
    aboutQtAction->setIcon(QIcon(":/qt.png"));

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(advancedAction);
    advancedAction->setIcon(QIcon(":/settings.png"));

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(restoreAction);
    restoreAction->setIcon(QIcon(":/restore.png"));

    trayIconMenu->addAction(minimizeAction);
    minimizeAction->setIcon(QIcon(":/minimize.png"));

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    quitAction->setIcon(QIcon(":/exit.png"));

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::on_actionHide_to_Tray_triggered()
{
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
        if(settings.value("mainWindow/app_tray_notification",false).toBool() == false)
          {
            trayIcon->showMessage("QtN2N GUI","QtN2N GUI is here...");
            qDebug() << "\n" << "Turn off app tray notification.";
          }
}

void MainWindow::save_on_app_quit()
{
    //save settings on app exit
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    QSettings settings_tap_ip (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    settings.beginGroup("mainWindow");
    settings.setValue("geometry",saveGeometry());
    settings.setValue("state",saveState());
    settings.endGroup();

    settings.beginGroup("network");
    settings.setValue("edge_profile",ui->comboBox->currentIndex());
    settings.setValue("edge_ip",ui->lineEdit_1->text());
    settings.setValue("edge_port",ui->lineEdit_2->text());
    settings.setValue("edge_name",ui->lineEdit_3->text());
    settings.setValue("edge_password",ui->lineEdit_4->text());

    if(settings_tap_ip.value("advanced/edge_tap_ip_mode",0).toInt() == 0)
      {
        settings.setValue("edge_tap_ip",ui->lineEdit_5->text());
      }

    settings.setValue("edge_mask",ui->lineEdit_6->text());
    settings.endGroup();

    settings.beginGroup("supernode");
    settings.setValue("supernode_port",ui->lineEdit_7->text());
    settings.setValue("supernode_verbose",ui->checkBox_0->isChecked());
    settings.endGroup();

    qDebug() << "\n" << "saved application settings on Quit";
}

void MainWindow::edge_time()
{
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(settings.value("advanced/edge_tap_ip_mode",0).toInt() == 1)
    if(settings.value("advanced/edge_version",false).toBool() == false)
      {
        ui->lineEdit_5->setText("0.0.0.0");
        ui->lineEdit_5->setDisabled(true);
      }
    if(settings.value("advanced/edge_tap_ip_mode",0).toInt() == 0)
    if(settings.value("advanced/edge_version",false).toBool() == false)
      {
        ui->lineEdit_5->setEnabled(true);
        ui->comboBox->setCurrentIndex(settings.value("network/edge_profile",0).toInt());
        qDebug() << "ComboBox load: " << ui->comboBox->currentText();
      }
}

void MainWindow::returnPressed()
{
    QString message = ui->sayLineEdit->text().trimmed();

    if(!message.isEmpty())
      {
        socket->write(QString(message + "\n").toUtf8());
      }
    ui->sayLineEdit->clear();
    ui->sayLineEdit->setFocus();

    qDebug() << "message: " + message;

    ui->statusBar->showMessage("Message has been sent...");

    bool connected = (socket->state() == QTcpSocket::ConnectedState);
    if(connected == 0)
      {
        QString server_messege = "Server is disconnected!";
        ui->sayLineEdit->setDisabled(true);
        ui->userListWidget->setDisabled(true);
        ui->userListWidget->clear();
        ui->roomTextEdit->moveCursor(QTextCursor::End);
        ui->roomTextEdit->setTextColor(Qt::magenta);
        ui->roomTextEdit->insertPlainText("\n" + server_messege);
        ui->roomTextEdit->verticalScrollBar()->setValue(ui->roomTextEdit->verticalScrollBar()->maximum());
        timer->start(10000);
      }
}

void MainWindow::on_checkBox_0_stateChanged(int arg1)
{
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    if(arg1 == 2)
      {
        settings.beginGroup("supernode");
        settings.setValue("supernode_verbose",ui->checkBox_0->isChecked());
        settings.endGroup();
      }
    if(arg1 == 0)
      {
        settings.beginGroup("supernode");
        settings.setValue("supernode_verbose",ui->checkBox_0->isChecked());
        settings.endGroup();
      }
    qDebug() << "Supernode Verbose = " << arg1;
}

void MainWindow::readyRead()
{
    while(socket->canReadLine())
    {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        QRegExp messageRegex("^([^:]+):(.*)$");
        QRegExp usersRegex("^/users:(.*)$");

        if(usersRegex.indexIn(line) != -1)
          {
            QStringList users = usersRegex.cap(1).split(",");
            ui->userListWidget->clear();
            foreach(QString user, users)
                new QListWidgetItem(QPixmap(":/user.png"), user, ui->userListWidget);
            qDebug() << "users who are connected to the server: " << users;
          }
        else if(messageRegex.indexIn(line) != -1)
        {
            QString datetime = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss ");
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            ui->roomTextEdit->append("<b>" + user + "</b>: " + message);
            clearAct->setEnabled(true);
            selectAllAct->setEnabled(true);
            ui->statusBar->showMessage("Chat is ON...");

            QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
            if(settings.value("mainWindow/chat_sound",false).toBool() == false)
              {
                QSound::play(QDir::currentPath() + "/notify.wav");
                qDebug() << "Chat sound is ON.";
              }
            if(settings.value("mainWindow/chat_tray_notification",false).toBool() == false)
              {
                trayIcon->showMessage("QtN2N GUI","You have MESSAGE!");
                qDebug() << "\n" << "Turn off chat tray notification. \n";
              }
            if(settings.value("mainWindow/log_to_file",false).toBool() == true)
              {
                QFile chat_log(QDir::currentPath() + "/logs/" + "chat.log");
                if(!chat_log.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
                return;
              {
                QTextStream out(&chat_log);
                out << datetime + user + ": " + message << "\n";
                out.flush();
              }
              }
            qDebug() << datetime + user + ": " + message;
        }
    }
}

void MainWindow::connected()
{
    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
    settings.beginGroup("chat");
    QString nickname = settings.value("nickname").toString();
    settings.endGroup();
    socket->write(QString("/me:" + nickname + "\n").toUtf8());
    qDebug() << "user: " + nickname + " --> connected to the chat server";
}

void MainWindow::on_connectToHost()
{
    socket->connectToHost(ui->lineEdit_5->text(), 4200);

    if(!socket->waitForConnected(1000))
        {
            qDebug() << "Error: " << socket->errorString();
            QString server_messege = "Server not found!";
            ui->roomTextEdit->moveCursor(QTextCursor::End);
            ui->roomTextEdit->setTextColor(Qt::red);
            ui->roomTextEdit->insertPlainText("\n" + server_messege);
            ui->roomTextEdit->verticalScrollBar()->setValue(ui->roomTextEdit->verticalScrollBar()->maximum());
        }
    else
        {
            timer->stop();
            ui->userListWidget->setEnabled(true);
            ui->sayLineEdit->setEnabled(true);
        }

    QSettings settings (QDir::currentPath() + "/n2n_gui.ini",QSettings::IniFormat);
        if(settings.value("mainWindow/chat_tray_notification",false).toBool() == false)
          {
            trayIcon->showMessage("QtN2N GUI","Chat is started...");
            qDebug() << "\n" << "Turn off chat tray notification. \n";
          }
    qDebug() << "server ip address: " + ui->lineEdit_5->text() + ":4200";
}

void MainWindow::ShowContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->roomTextEdit->mapToGlobal(pos);

    ContextMenu = new QMenu(this);
    ContextMenu->addSeparator();
    ContextMenu->addAction(copyAct);
    ContextMenu->addSeparator();
    ContextMenu->addAction(selectAllAct);
    ContextMenu->addSeparator();
    ContextMenu->addAction(clearAct);
    ContextMenu->addSeparator();
    ContextMenu->exec(globalPos);
}

void MainWindow::copy()
{
    ui->roomTextEdit->copy();
    qDebug() << "Copy selected text";
}

void MainWindow::selectAll()
{
    copyAct->setEnabled(true);
    selectAllAct->setDisabled(true);
    ui->roomTextEdit->selectAll();
    qDebug() << "Select All Text";
}

void MainWindow::clear()
{
    ui->roomTextEdit->clear();
    copyAct->setDisabled(true);
    selectAllAct->setDisabled(true);
    clearAct->setDisabled(true);
    qDebug() << "Clear Chat Messages";
}

void MainWindow::on_roomTextEdit_copyAvailable(bool b)
{
    if(b == true)
      {
        copyAct->setEnabled(true);
      }
    else
      {
        copyAct->setDisabled(true);
        selectAllAct->setEnabled(true);
      }
}

void MainWindow::on_actionAdd_TAP_Adapter_triggered()
{
    ui->tabInfo->setCurrentIndex(4);

    _SYSTEM_INFO sysinfo;
    GetNativeSystemInfo(&sysinfo);

    QString cmd_exe = "cmd.exe ";
    QString addAdapters = " install OemVista.inf tap0901";

    if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    {
        qDebug() << "You are running windows 32bit!";

        QFile driver(QCoreApplication::applicationDirPath() + "/driver/x86/tapinstall.exe");
        if(!driver.open(QIODevice::ReadOnly | QIODevice::NotOpen | QIODevice::Text))
          {
            QMessageBox::critical(nullptr, QString::fromLocal8Bit("Missing Driver Files!"), QString::fromLocal8Bit("Missing File <b>tapinstall.exe</b>! Cannot view TAP-Adapters."));
            return;
          }
        else
          {
            QString tapInstallDir(QCoreApplication::applicationDirPath() + "/driver/x86/tapinstall.exe");
            QString command = tapInstallDir + addAdapters;
            cmd_process->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/driver/x86");
            cmd_process->setProgram(cmd_exe);
            cmd_process->start(QString(command));
            qDebug() << cmd_exe + command;
          }
    }
    if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    {
        qDebug() << "You are running windows 64bit!";

        QFile driver(QCoreApplication::applicationDirPath() + "/driver/x64/tapinstall.exe");
        if(!driver.open(QIODevice::ReadOnly | QIODevice::NotOpen | QIODevice::Text))
          {
            QMessageBox::critical(nullptr, QString::fromLocal8Bit("Missing Driver Files!"), QString::fromLocal8Bit("Missing File <b>tapinstall.exe</b>! Cannot view TAP-Adapters."));
            return;
          }
        else
          {
            QString tapInstallDir(QCoreApplication::applicationDirPath() + "/driver/x64/tapinstall.exe");
            QString command = tapInstallDir + addAdapters;
            cmd_process->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/driver/x64");
            cmd_process->setProgram(cmd_exe);
            cmd_process->start(QString(command));
            qDebug() << cmd_exe + command;
          }
    }
}

void MainWindow::on_actionDel_TAP_Adapter_triggered()
{
    ui->tabInfo->setCurrentIndex(4);

    _SYSTEM_INFO sysinfo;
    GetNativeSystemInfo(&sysinfo);

    QString cmd_exe = "cmd.exe ";
    QString delAdapters = " remove tap0901";

    if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    {
        qDebug() << "You are running windows 32bit!";

        QFile driver(QCoreApplication::applicationDirPath() + "/driver/x86/tapinstall.exe");
        if(!driver.open(QIODevice::ReadOnly | QIODevice::NotOpen | QIODevice::Text))
          {
            QMessageBox::critical(nullptr, QString::fromLocal8Bit("Missing Driver Files!"), QString::fromLocal8Bit("Missing File <b>tapinstall.exe</b>! Cannot view TAP-Adapters."));
            return;
          }
        else
          {
            QString tapInstallDir(QCoreApplication::applicationDirPath() + "/driver/x86/tapinstall.exe");
            QString command = tapInstallDir + delAdapters;
            cmd_process->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/driver/x86");
            cmd_process->setProgram(cmd_exe);
            cmd_process->start(QString(command));
            qDebug() << cmd_exe + command;
          }
    }
    if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    {
        qDebug() << "You are running windows 64bit!";

        QFile driver(QCoreApplication::applicationDirPath() + "/driver/x64/tapinstall.exe");
        if(!driver.open(QIODevice::ReadOnly | QIODevice::NotOpen | QIODevice::Text))
          {
            QMessageBox::critical(nullptr, QString::fromLocal8Bit("Missing Driver Files!"), QString::fromLocal8Bit("Missing File <b>tapinstall.exe</b>! Cannot view TAP-Adapters."));
            return;
          }
        else
          {
            QString tapInstallDir(QCoreApplication::applicationDirPath() + "/driver/x64/tapinstall.exe");
            QString command = tapInstallDir + delAdapters;
            cmd_process->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/driver/x64");
            cmd_process->setProgram(cmd_exe);
            cmd_process->start(QString(command));
            qDebug() << cmd_exe + command;
          }
    }
}

void MainWindow::on_actionList_TAP_Adapters_triggered()
{
    ui->tabInfo->setCurrentIndex(4);

    _SYSTEM_INFO sysinfo;
    GetNativeSystemInfo(&sysinfo);

    QString cmd_exe = "cmd.exe ";
    QString listAdapters = " hwids tap0901";

    if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    {
        qDebug() << "You are running windows 32bit!";

        QFile driver(QCoreApplication::applicationDirPath() + "/driver/x86/tapinstall.exe");
        if(!driver.open(QIODevice::ReadOnly | QIODevice::NotOpen | QIODevice::Text))
          {
            QMessageBox::critical(nullptr, QString::fromLocal8Bit("Missing Driver Files!"), QString::fromLocal8Bit("Missing File <b>tapinstall.exe</b>! Cannot view TAP-Adapters."));
            return;
          }
        else
          {
            QString tapInstallDir(QCoreApplication::applicationDirPath() + "/driver/x86/tapinstall.exe");
            QString command = tapInstallDir + listAdapters;
            cmd_process->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/driver/x86");
            cmd_process->setProgram(cmd_exe);
            cmd_process->start(QString(command));
            qDebug() << cmd_exe + command;
          }
    }
    if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    {
        qDebug() << "You are running windows 64bit!";

        QFile driver(QCoreApplication::applicationDirPath() + "/driver/x64/tapinstall.exe");
        if(!driver.open(QIODevice::ReadOnly | QIODevice::NotOpen | QIODevice::Text))
          {
            QMessageBox::critical(nullptr, QString::fromLocal8Bit("Missing Driver Files!"), QString::fromLocal8Bit("Missing File <b>tapinstall.exe</b>! Cannot view TAP-Adapters."));
            return;
          }
        else
          {
            QString tapInstallDir(QCoreApplication::applicationDirPath() + "/driver/x64/tapinstall.exe");
            QString command = tapInstallDir + listAdapters;
            cmd_process->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/driver/x64");
            cmd_process->setProgram(cmd_exe);
            cmd_process->start(QString(command));
            qDebug() << cmd_exe + command;
          }
    }
}
