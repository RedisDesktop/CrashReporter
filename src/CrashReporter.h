#pragma once

#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSharedPointer>

struct Config {
    QString productName;
    QString reportUrl;
    QString productVersion;
    QString minidumpPath;
};

class CrashReporter : public QObject
{
    Q_OBJECT

public:
    CrashReporter( const Config& config );
    virtual ~CrashReporter( );

private:
    Config m_config;
    QSharedPointer<QNetworkRequest> m_request;
    QSharedPointer<QNetworkReply> m_reply;

    QString getPlatformInformation();

public slots:
    void send();

signals:
    void uploadingProgress( qint64 done, qint64 total );
    void success(const QString& title, const QString& message);
    void error(const QString& title, const QString& message);

private slots:
    void onDone();        
};
