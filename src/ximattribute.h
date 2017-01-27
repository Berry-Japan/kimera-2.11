#ifndef XIMATTRIBUTE_H
#define XIMATTRIBUTE_H

#include "xicontext.h"

class XIMethod;

class XIMAttribute {
public:
  XIMAttribute(Window w) : _commwin(w), _xic(XIContext()) { }
  ~XIMAttribute() {}
  
  Window      commWin() const { return _commwin; } 
  XIContext   xic() const { return _xic; }
  
private:
  Window       _commwin;     // communication window
  XIContext    _xic;         // XInputContext

  friend class XIMethod;
};

#endif // XIMATTRIBUTE_H
