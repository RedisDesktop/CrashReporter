#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QSignalSpy>
#include "CrashReporter.h"

class CrashReporterTest : public QObject
{
    Q_OBJECT

public:
    CrashReporterTest() {}

private Q_SLOTS:        
    void testRequest();
};

void CrashReporterTest::testRequest()
{
    //given
    Config config = {
        "test", "http://127.0.0.1:8080/crash-report", "1.1.1", "test.dmp"
    };
    CrashReporter reporter(config);
    QSignalSpy spy(&reporter, SIGNAL(success(const QString&, const QString&)));

    //when
    reporter.send();

    //then
    QVERIFY(spy.wait(5000));
}

QTEST_MAIN(CrashReporterTest)

#include "tst_crashreportertest.moc"
