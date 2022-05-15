#include <cstdio>
#include <syslog.h>
#include <QtGlobal>
#include <QString>
#include <QByteArray>

#include "mainwindow.h"
#include <QApplication>

// Handler for Qt log messages that sends output to syslog as well as standard error.
void SyslogMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)

    QByteArray localMsg = msg.toLocal8Bit();
      switch (type) {
      case QtDebugMsg:
          fprintf(stderr, "[D] %s\n", localMsg.constData());
          syslog(LOG_DEBUG, "[D] %s", localMsg.constData());
          break;
      case QtInfoMsg:
          fprintf(stderr, "[I] %s\n", localMsg.constData());
          syslog(LOG_INFO, "[I] %s", localMsg.constData());
          break;
      case QtWarningMsg:
          fprintf(stderr, "[W] %s\n", localMsg.constData());
          syslog(LOG_WARNING, "[W] %s", localMsg.constData());
          break;
      case QtCriticalMsg:
          fprintf(stderr, "[E] %s\n", localMsg.constData());
          syslog(LOG_CRIT, "[E] %s", localMsg.constData());
          break;
      case QtFatalMsg:
          fprintf(stderr, "[F]: %s\n", localMsg.constData());
          syslog(LOG_ALERT, "[F] %s", localMsg.constData());
          abort();
      }
}

int main(int argc, char *argv[])
{
    // Install our message handler.
    qInstallMessageHandler(SyslogMessageHandler);

    qInfo("NovaTest started");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
