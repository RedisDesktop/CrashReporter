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

    const QString productName = "RedisDesktopManager";
    const QString reportUrl = "http://redisdesktop.com/crash-report";
    const QString applicationName = QString("%1 Crash Reporter").arg(productName);
    const QString applicationVersion = "1.0.0";
    const QString applicationUrl = "redisdesktop.com";

    QApplication::setApplicationName(applicationName);
    QApplication::setApplicationVersion(applicationVersion);
    QApplication::setOrganizationDomain(applicationUrl);

    if (argc != 2 )
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(applicationName);
        msgBox.setText("<b>Usage:</b> <br /> crashReporter <i>dumpFileName</i>\n");
        msgBox.exec();
        return 1;
    }

    CrashReporter reporter(QUrl(reportUrl),  app.arguments());
    reporter.show();

    return app.exec();
}
