#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QDialog>
#include "ui_CrashReporter.h"

class AppWindow : public QDialog
{
    Q_OBJECT
public:
    explicit AppWindow();

public slots:
    void onSuccess(const QString& title, const QString& message);
    void onError(const QString& title, const QString& message);
    void onProgress(qint64 done, qint64 total);

private:
    Ui::CrashReporter ui;

private slots:
    void onAppRestart();
};

#endif // APPWINDOW_H
