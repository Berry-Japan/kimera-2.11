#include "style.h"

int
Style::styleHint(StyleHint hint, const QStyleOption * option, const QWidget * widget, QStyleHintReturn * returnData) const
{
  int ret = 0;
  switch (hint) {
  case SH_ToolButton_PopupDelay:
    ret = 1;
    break;

  default:
    ret = QPlastiqueStyle::styleHint(hint, option, widget, returnData);
  }
  return ret;
}
