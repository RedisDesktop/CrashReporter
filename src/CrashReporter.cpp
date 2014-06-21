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
    : m_config(config)

{
    //init ui
    ui.setupUi( this );    
    setFixedSize( size() );

    // create & send request
    m_request = QSharedPointer<QNetworkRequest>(new QNetworkRequest( QUrl(config.reportUrl) ));
    QTimer::singleShot( 1, this, SLOT( send() ) );
}

CrashReporter::~CrashReporter()
{
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

    QScopedPointer<QNetworkAccessManager> nam(new QNetworkAccessManager( this ));
    m_request->setHeader( QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=rboundary" );
    m_reply = QSharedPointer<QNetworkReply>(nam->post( *m_request, body ));

    connect( m_reply.data(), &QNetworkReply::finished, this, &CrashReporter::onDone, Qt::QueuedConnection );
    connect( m_reply.data(), &QNetworkReply::uploadProgress, this, &CrashReporter::onProgress );
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
CrashReporter::onProgress( qint64 done, qint64 total )
{
    if ( total )
    {
        QString const msg = tr( "Uploaded %L1 of %L2 KB." ).arg( done / 1024 ).arg( total / 1024 );

        ui.progressBar->setMaximum( total );
        ui.progressBar->setValue( done );
        ui.progressLabel->setText( msg );
    }
}


void
CrashReporter::onDone()
{
    QByteArray data = m_reply->readAll();
    ui.progressBar->setValue( ui.progressBar->maximum() );
    ui.button->setText( tr( "Close" ) );

    QString const response = QString::fromUtf8( data );

    // Parse response
    // more info in README.md
    QScopedPointer<QJsonParseError> parsingError(new QJsonParseError);
    QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), parsingError.data());
    bool valid = (parsingError->error == QJsonParseError::NoError
                  || !document.isObject());

    if ( ( m_reply->error() != QNetworkReply::NoError ) || !valid )
    {
        QMessageBox::warning(this, "Server Error", response);
        onFail( m_reply->error(), m_reply->errorString() );
        return;
    }

    QJsonObject jsonResponse = document.object();

    if (jsonResponse.contains("ok")) {

        QMessageBox::warning(this, "Crash Report Sent!", jsonResponse.value("ok").toString());
        QFile::remove(m_config.minidumpPath);

    } else if (jsonResponse.contains("error")) {

        QMessageBox::warning(this, "Error occurred", jsonResponse.value("ok").toString());
    }
}


void
CrashReporter::onFail( int error, const QString& errorString )
{
    ui.button->setText( tr( "Close" ) );
    ui.progressLabel->setText( tr( "Failed to send crash info." ) );
    qDebug() << "Error:" << error << errorString;
}
