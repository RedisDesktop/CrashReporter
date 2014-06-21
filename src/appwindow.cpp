#include "appwindow.h"
#include <QMessageBox>
#include <QDebug>

AppWindow::AppWindow() :
    QDialog(nullptr)
{
    //init ui
    ui.setupUi( this );
    setFixedSize( size() );
}

void AppWindow::onProgress( qint64 done, qint64 total )
{
    if ( total )
    {
        QString const msg = tr( "Uploaded %L1 of %L2 KB." ).arg( done / 1024 ).arg( total / 1024 );

        ui.progressBar->setMaximum( total );
        ui.progressBar->setValue( done );
        ui.progressLabel->setText( msg );
    }
}


void AppWindow::onError(const QString& title, const QString& message )
{
    ui.progressLabel->setText( tr( "Failed to send crash info." ) );
    QMessageBox::critical(this, title, message);
    qDebug() << "Error:" << message;
}

void AppWindow::onSuccess(const QString& title, const QString& message)
{
    ui.progressBar->setValue( ui.progressBar->maximum());
    QMessageBox::information(this, title, message);
}
