#include "UnixSignalNotifier.h"
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>

int UnixSignalNotifier::sighupFifeDescriptor[2] = {};
int UnixSignalNotifier::sigtermFifeDescriptor[2] = {};

UnixSignalNotifier::UnixSignalNotifier(QObject *parent) : QObject(parent)
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFifeDescriptor)){
       qFatal("Couldn't create HUP socketpair");
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFifeDescriptor)){
       qFatal("Couldn't create TERM socketpair");
    }
    socketNotifierSIGHUP = new QSocketNotifier(sighupFifeDescriptor[1], QSocketNotifier::Read, this);
    connect(socketNotifierSIGHUP, &QSocketNotifier::activated, this, &UnixSignalNotifier::handleSIGHUP);
    socketNotifierSIGTERM = new QSocketNotifier(sigtermFifeDescriptor[1], QSocketNotifier::Read, this);
    connect(socketNotifierSIGTERM, &QSocketNotifier::activated, this, &UnixSignalNotifier::handleSIGTERM);


    struct sigaction hup, term;

    hup.sa_handler = UnixSignalNotifier::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, 0)){
        qFatal("%s: Couldn't register SIGHUP handler", Q_FUNC_INFO);
    }

    term.sa_handler = UnixSignalNotifier::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = 0;
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0)){
        qFatal("%s: Couldn't register SIGTERM handler", Q_FUNC_INFO);
    }
}


void UnixSignalNotifier::handleSIGHUP()
{
    socketNotifierSIGTERM->setEnabled(false);
    char tmp;
    ::read(sighupFifeDescriptor[1], &tmp, sizeof(tmp));

    // do Qt stuff
    emit signalUnix();

    socketNotifierSIGTERM->setEnabled(true);
}

void UnixSignalNotifier::handleSIGTERM()
{
    socketNotifierSIGHUP->setEnabled(false);
    char tmp;
    ::read(sigtermFifeDescriptor[1], &tmp, sizeof(tmp));

    // do Qt stuff
    emit signalUnix();

    socketNotifierSIGHUP->setEnabled(true);
}

void UnixSignalNotifier::hupSignalHandler(int)
{
    char a = 1;
    ::write(sighupFifeDescriptor[0], &a, sizeof(a));
}

void UnixSignalNotifier::termSignalHandler(int)
{
    char a = 1;
    ::write(sigtermFifeDescriptor[0], &a, sizeof(a));
}
