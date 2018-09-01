#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Don't include additional header files from windows.h
// These must be then included explicitly
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>

#include <QMainWindow>
#include "ui_mainwindow.h"

#include <QProcess>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QtNetwork>
#include <QTcpSocket>

#include "version.h"
#include "advanced.h"
#include "aboutdialog.h"

#include "processes.h"

//#include "pcap.h"

//#include "tapdriver.h"
//#include "check64.h"
//#include "message.h"

class AboutDialog;
class Advanced;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setVisible(bool visible);
    QTimer *timer;

protected:
    void closeEvent(QCloseEvent *event);
    void customContextMenuRequested(const QPoint &pos);

private slots:
    void ShowContextMenu(const QPoint &pos);
    void loadSettings();
    void printOutput_edge_process(void);
    void printOutput_sn_process(void);
    void printOutput_chat_server(void);
    void printOutput_cmd_process(void);
    void on_actionAboutQt_triggered();
    void on_actionAbout_triggered();
    void on_actionHelp_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_edgeConnectButton_clicked();
    void on_edgeDisconnectButton_clicked();
    void on_nodeStartButton_clicked();
    void on_nodeStopButton_clicked();
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void on_actionAdvanced_triggered();
    void on_actionHide_to_Tray_triggered();
    void save_on_app_quit();
    void edge_time();
    void returnPressed();
    void on_checkBox_0_stateChanged(int arg1);
    void readyRead();
    void connected();
    void on_connectToHost();
    void copy();
    void clear();
    void selectAll();
    void on_roomTextEdit_copyAvailable(bool b);
    void on_actionAdd_TAP_Adapter_triggered();
    void on_actionDel_TAP_Adapter_triggered();
    void on_actionList_TAP_Adapters_triggered();

private:
    Ui::MainWindow *ui;
    QProcess *edge_process;
    QProcess *sn_process;
    QProcess *chat_server;
    QProcess *cmd_process;

    //running processes
    Processes *processes;

    void createActions();
    void createTrayIcon();

    QAction *helpAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
    QAction *advancedAction;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QAction *copyAct;
    QAction *clearAct;
    QAction *selectAllAct;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QMenu *ContextMenu;

    Advanced *advanced;
    QTcpSocket *socket;

    QProcess proc;
};

#endif // MAINWINDOW_H
