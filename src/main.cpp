#include "kimeraapp.h"
#include "maintoolbar.h"
#include "style.h"
#include <QTextCodec>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <unistd.h>
#include <pwd.h>

void 
outputMessage(QtMsgType type, const char *msg)
{
  switch ( type ) {
#ifndef KIMERA_NO_DEBUG
  case QtDebugMsg:
    fprintf(stderr, "%s\n", msg);
    break;
    
  case QtWarningMsg:
    fprintf(stderr, "Warning: %s\n", msg);
    break;
#endif

  case QtFatalMsg:
    fprintf(stderr, "Fatal: %s\n", msg);
#ifndef KIMERA_NO_DEBUG
    abort();          // dump core on purpose
#endif
    break;

  default:
    break;
  }
}


int 
main(int argc, char* argv[])
{
  qInstallMsgHandler(outputMessage);
  QApplication::setStyle(new Style());
  KimeraApp app(argc, argv);

  // Check lock file
  struct passwd* pass = getpwuid( getuid() );
  QDir::setCurrent("/tmp");
  QFile lockfile(QString(".kimera_") + QString(pass->pw_name) + ".lock");
  if ( !lockfile.exists() ) {
    qFatal("No such lock file: %s/%s", qPrintable(QDir::current().path()),
           qPrintable(lockfile.fileName()));
    return -1;
  } else {
    lockfile.remove();
  }

  // Sets codec, eucJP.
  QTextCodec*  codec = QTextCodec::codecForName("eucJP");  
  Q_CHECK_PTR( codec );
  qDebug("codec: %s", codec->name().data());
  QTextCodec::setCodecForTr( codec );

  // Shows main widget
  MainToolBar* mainwidget = new MainToolBar();
  mainwidget->move( mainwidget->loadPos() );
  mainwidget->show();    // Need to show at least once for initiation
  return app.exec();
}
