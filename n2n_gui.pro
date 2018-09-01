#-------------------------------------------------
#
# Project created by QtCreator 2015-01-02T14:30:30
#
#-------------------------------------------------
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += core gui
QT += network
QT += multimedia

CONFIG += c++11
CONFIG += qt thread warn_on debug_and_release

#win32:INCLUDEPATH += $$PWD/wpcap/Include
#win32:LIBS += -L$$PWD/wpcap/lib -lwpcap

# Single Application implementation
include(SingleApplication/singleapplication.pri)
DEFINES += QAPPLICATION_CLASS=QApplication

# Processes Application implementation
include(processes/processes.pri)

# TAP-Adapter
#include(tapdriver/tapdriver.pri)

DEFINES += QAPPLICATION_CLASS=QApplication

TARGET = n2n_gui
TEMPLATE = app

SOURCES += main.cpp\
            mainwindow.cpp \
            aboutdialog.cpp \
            advanced.cpp

HEADERS  += mainwindow.h \
            aboutdialog.h \
            advanced.h \
            version.h

FORMS    += mainwindow.ui \
            aboutdialog.ui \
            advanced.ui

OTHER_FILES +=
win32:RC_FILE += GUI_n2n.rc

RESOURCES += \
    icons.qrc

DISTFILES += \
    GUI_n2n.rc

win32 {
    DEPLOY_COMMAND = $$shell_quote($$shell_path($$[QT_INSTALL_BINS]/windeployqt))

    DEPLOY_OPTIONS = "--no-svg --no-system-d3d-compiler --no-angle --no-opengl-sw --no-translations --no-quick-import --no-webkit2"

    CONFIG(debug, debug|release) {
        DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/debug/$${TARGET}.exe))
        DEPLOY_OPTIONS += "--debug"

    } else {
        DEPLOY_TARGET = $$shell_quote($$shell_path($${OUT_PWD}/release/$${TARGET}.exe))
        DEPLOY_OPTIONS += "--release"
    }

    # Uncomment the following line to help debug the deploy command when running qmake
    warning($${DEPLOY_COMMAND} $${DEPLOY_OPTIONS} $${DEPLOY_TARGET})

    QMAKE_POST_LINK = $${DEPLOY_COMMAND} $${DEPLOY_OPTIONS} $${DEPLOY_TARGET}
}

# Deployment - Copy Dependencies to Build Folder

#dlls.path  =  $${DESTDIR}
#dlls.files += $$[QT_INSTALL_BINS]/icudt51.dll
#dlls.files += $$[QT_INSTALL_BINS]/icuin51.dll
#dlls.files += $$[QT_INSTALL_BINS]/icuuc51.dll
#dlls.files += $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll
#dlls.files += $$[QT_INSTALL_BINS]/libstdc++-6.dll
#dlls.files += $$[QT_INSTALL_BINS]/libwinpthread-1.dll
#dlls.files += $$[QT_INSTALL_BINS]/Qt5Core.dll
#dlls.files += $$[QT_INSTALL_BINS]/Qt5Network.dll
#dlls.files += $$[QT_INSTALL_BINS]/Qt5Gui.dll
#dlls.files += $$[QT_INSTALL_BINS]/Qt5Widgets.dll
#dllA.path  += $${DESTDIR}/platforms
#dllA.files += $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
#dllB.path  += $${DESTDIR}/plugins/imageformats/
#dllB.files += $$[QT_INSTALL_PLUGINS]/imageformats/qico.dll
#dllB.files += $$[QT_INSTALL_PLUGINS]/imageformats/qwbmp.dll
#INSTALLS   += dlls dllA dllB
