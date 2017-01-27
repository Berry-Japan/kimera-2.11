#include "kimeraapp.h"
#include "inputmethod.h"
#include "config.h"
#include "debug.h"

static InputMethod* inpmethod = 0;

KimeraApp::KimeraApp(int &argc, char **argv) : QApplication(argc, argv)
{
  qAddPostRoutine( cleanup_ptr );   // delete object
  setQuitOnLastWindowClosed( false );
  InputMethod::registerXIMServerKimera();  // Register Kimera to X
}

//
// Returns a pointer to the application global inputmethod.
//
InputMethod* 
KimeraApp::inputmethod()
{
  DEBUG_TRACEFUNC();
  if ( !inpmethod ) {
    inpmethod = new InputMethod();
    Q_CHECK_PTR( inpmethod );
  }
  
  return inpmethod;
}


void
KimeraApp::cleanup_ptr()
{
  DEBUG_TRACEFUNC();
  if (inpmethod)
    delete inpmethod;
  inpmethod = 0;
}


bool 
KimeraApp::x11EventFilter(XEvent* event)
{
  if ( !inpmethod )
    return false;

  if (event->type == ClientMessage) {
    XimEvent e((QEvent::Type)KimeraApp::XIMClientMessage, event, sizeof(XEvent));
    QApplication::sendEvent(inpmethod, &e);
    return false;
  } else if (event->type >= SelectionClear && event->type <= SelectionNotify) {
    XimEvent e((QEvent::Type)KimeraApp::Selection, event, sizeof(XEvent));
    QApplication::sendEvent(inpmethod, &e);
    return true;
  }
  
  return false;
}


bool
KimeraApp::isXIMInputtingEnabled()
{
  return  inputmethod()->isXIMInputtingEnabled();
}
