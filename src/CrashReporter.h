/* 
 * RedisDesktopManager Crash Reporter
 * based on source from http://tomahawk-player.org
 */

#pragma once

#include <QDialog>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSharedPointer>

#include "ui_CrashReporter.h"


struct Config {
    QString productName;
    QString reportUrl;
    QString productVersion;
    QString minidumpPath;
};


class CrashReporter : public QDialog
{
    Q_OBJECT

public:
    CrashReporter( const Config& config );
    virtual ~CrashReporter( );

private:
    Ui::CrashReporter ui;
    Config m_config;
    QSharedPointer<QNetworkRequest> m_request;
    QSharedPointer<QNetworkReply> m_reply;

    QString getPlatformInformation();

public slots:
    void send();

private slots:
    void onDone();
    void onProgress( qint64 done, qint64 total );
    void onFail( int error, const QString& errorString );
};
