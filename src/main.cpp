/* 
 * RedisDesktopManager Crash Reporter
 * based on source from http://tomahawk-player.org
 */
#include <iostream>
#include "CrashReporter.h"
#include "appwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QUrl>
#include <QDebug>
#include <QTimer>

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );    

    if (argc < 2 || !QFile::exists(app.arguments()[1]))
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Crash Reporter");
        msgBox.setText(
            QString("<b>Usage:</b> <br />"
                    " crashReporter <i>minidumpPath</i> [<i>crashedApplicationPath</i>]\n"
                    "<br /> <br /> <b>Example:</b> <br />"
                    " crashReporter 0120EDSDSD3.dmp [/usr/share/appFolder/app]\n"
                    "<br /><br /> <b> More info </b>: <a href='https://github.com/RedisDesktop/CrashReporter'>https://github.com/RedisDesktop/CrashReporter</a>"
                    "<br/><br/> Config:<br/> APP_NAME=%1;<br/> APP_VERSION=%2;<br/> CRASH_SERVER_URL=%3<br/>"
                    "<br /><br /> ARGUMENTS: <br /> %4"
                   ).arg(QString(APP_NAME))
                    .arg(QString(APP_VERSION))
                    .arg(QString(CRASH_SERVER_URL))
                    .arg(app.arguments().join("<br />"))
        );
        msgBox.exec();
        return 1;
    }

    Config crashReporterConfig = {
        QString(APP_NAME),
        QString(CRASH_SERVER_URL),
        QString(APP_VERSION),
        QString(app.arguments()[1])
    };

    QApplication::setApplicationName(QString("%1 Crash Reporter").arg(crashReporterConfig.productName));
    QApplication::setApplicationVersion("1.0.0");

    CrashReporter reporter(crashReporterConfig);
    AppWindow window;

    QObject::connect(&reporter, &CrashReporter::success, &window, &AppWindow::onSuccess);
    QObject::connect(&reporter, &CrashReporter::error, &window, &AppWindow::onError);
    QObject::connect(&reporter, &CrashReporter::uploadingProgress, &window, &AppWindow::onProgress);

    QTimer::singleShot( 1, &reporter, SLOT( send() ) );
    window.show();

    return app.exec();
}
