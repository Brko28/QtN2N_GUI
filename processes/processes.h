#ifndef PROCESSES_H
#define PROCESSES_H

#include <QFileIconProvider>
#include <QFileInfo>
#include <QIcon>
#include <QObject>

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <stdio.h>

// Need to link with IPHlpApi.Lib for GetExtendedTcpTable() used in getPorts()
#include <IPHlpApi.h>
#pragma comment(lib, "IPHlpApi.Lib")

struct Process
{
    QString name; // 1
    QString path;
    QString pid; // 2
    QString ppid;
    QString port; // 3
    QString memoryUsage; // 4
    QIcon icon;
};

struct PidAndPort
{
    QString pid;
    QString port;
};

class Processes : public QObject
{
    Q_OBJECT

public:
    // singleton
    static Processes *getInstance();
    static void release();
    static Processes *theInstance;

    // static QList<Process> getProcessesList();
    // static QList<PidAndPort> getPortsList();
    static QList<Process> getMonitoredProcessesList();

    static bool killProcess(qint64 pid);
    static bool killProcess(const QString &name);

    static bool killProcessTree(const QString &name);
    static bool killProcessTree(qint64 pid);

    static Process findByName(const QString &name);
    static Process findByPid(const QString &pid);

    static QList<Process> getRunningProcesses();
    static QList<PidAndPort> getPorts();

    static bool areThereAlreadyRunningProcesses();

    static bool isSystemProcess(QString processName);

    enum ProcessState
    {
        NotRunning,
        Starting,
        Running
    };
    Q_ENUM(ProcessState)

    ProcessState getProcessState(const QString &processName) const;

    static bool start(const QString &program, const QStringList &arguments, const QString &workingDir = QString());
    static bool start(const QString &program, const QStringList &arguments);
    static bool start(const QString &command);

    static bool startDetached(const QString &program, const QStringList &arguments, const QString &workingDir = QString());
    static bool startDetached(const QString &program, const QStringList &arguments);
    static bool startDetached(const QString &command);

    static void delay(int millisecondsToWait);

private:
    // constructor is private, because singleton
    explicit Processes();

    static QList<Process> monitoredProcessesList;

    static QStringList getProcessDetails(DWORD processID);
    static QString getSizeHumanReadable(float bytes);

    static QStringList getProcessNamesToSearchFor();

    static QString qt_create_commandline(const QString &program, const QStringList &arguments);
};

#endif // PROCESSES_H
