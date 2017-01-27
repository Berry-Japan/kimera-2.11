#ifndef STYLE_H
#define STYLE_H

#include <QPlastiqueStyle>

class Style : public QPlastiqueStyle {
  Q_OBJECT

public:    
  int styleHint( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const;
};

#endif  // STYLE_H
