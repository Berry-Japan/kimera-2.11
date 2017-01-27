#include <qpoint.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xmd.h>

void
xEvent2XEvent(Display* dis, const void* src, unsigned int serial, XEvent& e)
{
  xEvent event;
  memcpy(&event, src, sz_xEvent);

  e.type = event.u.u.type & 0x7f;
  e.xany.serial = (serial << 16) | event.u.u.sequenceNumber;
  e.xany.display = dis;
  e.xany.send_event = event.u.u.type > 127;
  e.xkey.window = event.u.keyButtonPointer.event;
  e.xkey.root = event.u.keyButtonPointer.root;
  e.xkey.subwindow = event.u.keyButtonPointer.child;
  e.xkey.time = event.u.keyButtonPointer.time;
  e.xkey.x = cvtINT16toInt(event.u.keyButtonPointer.eventX);
  e.xkey.y = cvtINT16toInt(event.u.keyButtonPointer.eventY);
  e.xkey.x_root = cvtINT16toInt(event.u.keyButtonPointer.rootX);
  e.xkey.y_root = cvtINT16toInt(event.u.keyButtonPointer.rootY);
  e.xkey.state = event.u.keyButtonPointer.state;
  e.xkey.keycode = event.u.u.detail;
  e.xkey.same_screen = event.u.keyButtonPointer.sameScreen;
}
