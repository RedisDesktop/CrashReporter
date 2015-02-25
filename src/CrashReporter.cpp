/* 
 * RedisDesktopManager Crash Reporter
 * based on source from http://tomahawk-player.org
 */

#include "CrashReporter.h"
#include <QIcon>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

CrashReporter::CrashReporter(const Config &config)
    : m_config(config), m_request(nullptr), m_reply(nullptr)

{
    m_request = QSharedPointer<QNetworkRequest>(new QNetworkRequest( QUrl(config.reportUrl) ));
}

CrashReporter::~CrashReporter()
{
    if (!m_reply.isNull() && m_reply->isRunning())
        m_reply->abort();
}

static QByteArray
contents( const QString& path )
{
    QFile f( path );
    f.open( QFile::ReadOnly );
    return f.readAll();
}


void CrashReporter::send()
{
    if (!QFile::exists(m_config.minidumpPath)) {
        qDebug() << "Can not find minidump file: " << m_config.minidumpPath;
        return;
    }

    QByteArray body;

    // add parameters
    typedef QPair<QByteArray, QByteArray> Pair;
    QList<Pair> pairs;

    pairs << Pair( "product", m_config.productName.toUtf8() )
          << Pair( "platform", getPlatformInformation().toUtf8() )
          << Pair( "version", m_config.productVersion.toUtf8());

    foreach ( Pair const pair, pairs )
    {
        body += "--rboundary\r\n";
        body += "Content-Disposition: form-data; name=\"" +
                           pair.first  + "\"\r\n\r\n" +
                           pair.second + "\r\n";
    }

    // add minidump file
    body += "--rboundary\r\n";
    body += "Content-Disposition: form-data; name=\"upload_file_minidump\"; filename=\""
          + QFileInfo( m_config.minidumpPath ).fileName() + "\"\r\n";
    body += "Content-Type: application/octet-stream\r\n";
    body += "\r\n";
    body += contents( m_config.minidumpPath );
    body += "\r\n";
    body += "--rboundary--\r\n";

    QNetworkAccessManager* nam = new QNetworkAccessManager( this );
    m_request->setHeader( QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=rboundary" );
    m_reply = QSharedPointer<QNetworkReply>(nam->post( *m_request, body ));

    connect( m_reply.data(), &QNetworkReply::finished, this, &CrashReporter::onDone );
    connect( m_reply.data(), &QNetworkReply::uploadProgress, this, &CrashReporter::uploadingProgress );
}

QString 
CrashReporter::getPlatformInformation()
{
    //todo: collect more info

    QString platform;

#ifdef Q_OS_LINUX
    platform = "Linux";
#endif

#ifdef Q_OS_MAC    
    platform = "MacOS";
#endif

#ifdef Q_OS_WIN    
    platform = "Windows";
#endif

    return platform;
}

void
CrashReporter::onDone()
{    
    QString const response = QString::fromUtf8(m_reply->readAll());

    // Parse response
    // more info in README.md
    QScopedPointer<QJsonParseError> parsingError(new QJsonParseError);
    QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), parsingError.data());
    bool valid = (parsingError->error == QJsonParseError::NoError
                  || !document.isObject());

    if ( ( m_reply->error() != QNetworkReply::NoError ) || !valid )
    {
        emit error("Server error", m_reply->errorString());
        return;
    }

    QJsonObject jsonResponse = document.object();

    if (jsonResponse.contains("ok")) {

        QString msg = jsonResponse.value("ok").toString();
        QFile::remove(m_config.minidumpPath);
        emit success("Crash Report Sent!", msg);
        return;

    } else if (jsonResponse.contains("error")) {
        emit error("Error occurred", jsonResponse.value("error").toString());
    }
}



