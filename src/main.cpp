/* 
 * RedisDesktopManager Crash Reporter
 * based on source from http://tomahawk-player.org
 */
#include <iostream>
#include "CrashReporter.h"
#include <QApplication>
#include <QMessageBox>
#include <QUrl>
#include <QDebug>

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );


    if (argc != 2 )
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Crash Reporter");
        msgBox.setText(
                    "<b>Usage:</b> <br />"
                    " crashReporter <i>dumpFileName</i> [<i>crashedApplicationPath</i>]\n"
                    "<br /> <br /> <b>Example:</b> <br />"
                    " crashReporter 0120EDSDSD3.dmp [/usr/share/appFolder/app]\n"
                    "<br /><br /> <b> More info </b>: <a href='https://github.com/RedisDesktop/CrashReporter'>https://github.com/RedisDesktop/CrashReporter</a>"
                    );
        msgBox.exec();
        return 1;
    }

    Config crashReporterConfig = {
        "RedisDesktopManager",
        "http://redisdesktop.com/crash-report",
        QByteArray(RDM_VERSION)
    };

    QApplication::setApplicationName(QString("%1 Crash Reporter").arg(crashReporterConfig.productName));
    QApplication::setApplicationVersion("1.0.0");

    CrashReporter reporter(crashReporterConfig);
    reporter.show();

    return app.exec();
}
