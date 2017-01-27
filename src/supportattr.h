#ifndef SUPPORTATTR_H
#define SUPPORTATTR_H

#include <QByteArray>

enum ValueType {
  TYPE_SEPARATOR = 0,
  TYPE_BYTE = 1,
  TYPE_WORD = 2,
  TYPE_LONG = 3,
  TYPE_CHAR = 4,
  TYPE_WINDOW = 5,
  TYPE_XIMSTYLES = 10,
  TYPE_XRECTANGLE = 11,
  TYPE_XPOINT = 12,
  TYPE_XFONTSET = 13,
  TYPE_XIMHOTKEYTRIGGERS = 15,
  TYPE_XIMHOTKEYSTATE = 16,
  TYPE_XIMSTRINGCONVERSION = 17,
  TYPE_XIMPREEDITSTATE = 18,
  TYPE_XIMRESETSTATE = 19,
  TYPE_NESTEDLIST = 0x7fff 
};

// IM and IC attributes supported class  
class SupportAttr {
public:
  SupportAttr(quint16 i, QByteArray str, ValueType t) : _id(i), _attr(str), _type(t) { }
  SupportAttr() { }

  quint16    id() const { return _id; }
  QByteArray attribute() const { return _attr; }
  ValueType  type() const { return _type; }

private:
  quint16      _id;
  QByteArray   _attr;
  ValueType    _type;
};


#endif // SUPPORTATTR_H
