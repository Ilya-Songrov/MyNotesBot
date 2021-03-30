#ifndef UNIXSIGNALNOTIFIER_H
#define UNIXSIGNALNOTIFIER_H

#include <QObject>
#include <QSocketNotifier>
#include <QDebug>

class UnixSignalNotifier : public QObject
{
    Q_OBJECT

    static int sighupFifeDescriptor[2];
    static int sigtermFifeDescriptor[2];

    QSocketNotifier *socketNotifierSIGHUP;
    QSocketNotifier *socketNotifierSIGTERM;
public:
    explicit UnixSignalNotifier(QObject *parent = nullptr);

public slots:
    // Qt signal handlers.
    void handleSIGHUP();
    void handleSIGTERM();
private:
    // Unix signal handlers.
    static void hupSignalHandler(int);
    static void termSignalHandler(int);
signals:
    void signalUnix();

};

#endif // UNIXSIGNALNOTIFIER_H
