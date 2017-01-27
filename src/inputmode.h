#ifndef INPUTMODE_H
#define INPUTMODE_H

#include "kimeraglobal.h"

class InputMode {
public:
  InputMode(int mode = Kimera::Mode_Hiragana | Kimera::Mode_RomaInput) : _mode(mode) { }
  int id() const { return _mode; }

  InputMode& merge(const InputMode& mode)
  {
    if (mode.id() & Kimera::Mode_ModeMask)
      _mode = (mode.id() & Kimera::Mode_ModeMask) | (_mode & ~Kimera::Mode_ModeMask);
    
    if (mode.id() & Kimera::Mode_InputMask)
      _mode = (mode.id() & Kimera::Mode_InputMask) | (_mode & ~Kimera::Mode_InputMask);

    return *this;
  }

  friend bool operator==(const InputMode& m1, const InputMode& m2) { return m1.id() == m2.id(); }
  friend bool operator!=(const InputMode& m1, const InputMode& m2) { return m1.id() != m2.id(); }

private:
  int  _mode;

  InputMode& operator=(const InputMode&);
};

#endif  // INPUTMODE_H
