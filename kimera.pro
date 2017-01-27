TEMPLATE        = app
CONFIG         += qt x11 warn_on release
QT             += network
TARGET          = kimera-bin
VERSION         = 2.11
isEmpty( target.path ) {
  target.path   = /usr/lib/kimera-$$VERSION
}
system(sed -e 's:=kimera-bin$:=$$target.path/kimera-bin:' tools/kimera > kimera && chmod +x kimera)
message(This project will be installed in $$target.path)
isEmpty( script.path ) {
  script.path     = /usr/bin
}
message(Kimera startup script will be installed in $$script.path)
script.files    = kimera
dic.files       = dic/hiragana.dic \
                  dic/katakana.dic \
                  dic/hankakukana.dic \
                  dic/numeralsymbols.dic \
                  dic/dakuten.dic \
                  dic/kanainput.dic \
                  dic/zenkakualphabet.dic
dic.path        = $$target.path/dic
image.files     = images/red_dictionary.png \
                  images/property_icon.png \
                  images/trayicon_directinput.png \
                  images/trayicon_hiragana.png \
                  images/trayicon_katakana.png \
                  images/trayicon_zenkakueisu.png \
                  images/trayicon_hankakukana.png
image.path      = $$target.path/images
INSTALLS        = target \
                  script \
                  dic \
                  image
DEFINES        += KIMERA_VERSION=\\\"$$VERSION\\\" INSTALL_PATH=\\\"$$target.path\\\"
CONFIG( release, debug|release ) {
  DEFINES      += KIMERA_NO_DEBUG
}
INCLUDEPATH    += src ui /usr/local/include
DEPENDPATH     += src ui
HEADERS         = \
                  src/kimeraglobal.h \
                  src/maintoolbar.h \
                  src/propertydialog.h \
                  src/keyassigner.h \
                  src/dicthiragana.h \
                  src/kimeraapp.h \
                  src/inputmethod.h \
                  src/ximattribute.h \
                  src/xicattribute.h \
                  src/kanjiengine.h \
                  src/cannaengine.h \
#                  src/primeengine.h \
                  src/tomoeengine.h \
                  src/preeditarea.h \
                  src/supportattr.h \
                  src/candidatelistbox.h \
                  src/kanjiconvert.h \
                  src/ximethod.h \
                  src/xicontext.h \
                  src/inputmode.h \
                  src/style.h \
                  src/config.h \
                  src/debug.h
SOURCES         = \
                  src/main.cpp \
                  src/kimeraglobal.cpp \
                  src/maintoolbar.cpp \
                  src/propertydialog.cpp \
                  src/keyassigner.cpp \
                  src/dicthiragana.cpp \
                  src/kimeraapp.cpp \
                  src/inputmethod.cpp \
                  src/inputmethod_x11.cpp \
                  src/xicattribute.cpp \
                  src/kanjiengine.cpp \
                  src/cannaengine.cpp \
#                  src/primeengine.cpp \
                  src/tomoeengine.cpp \
                  src/preeditarea.cpp \
                  src/candidatelistbox.cpp \
                  src/kanjiconvert.cpp \
                  src/ximethod.cpp \
                  src/xicontext.cpp \
                  src/style.cpp \
                  src/config.cpp \
                  src/debug.cpp

isEmpty( no_anthy ) {
  HEADERS      += src/anthyengine.h
  SOURCES      += src/anthyengine.cpp
#  LIBS         += -ldl
  isEmpty( default_kanjiengine ) {
    default_kanjiengine = Anthy
  }
}
isEmpty( default_kanjiengine ) {
  default_kanjiengine = Canna
}
DEFINES        += DEFAULT_KANJIENGINE=\\\"$$default_kanjiengine\\\"

FORMS            = \
                  ui/keyassigner.ui \
                  ui/propertydialog.ui
DISTFILES      += \
                  AUTHORS \
                  INSTALL-ja.eucJP \
                  COPYING \
                  README-en \
                  README-ja.eucJP \
                  tools/kimera \
                  images/red_dictionary.png \
                  images/property_icon.png \
                  images/trayicon_directinput.png \
                  images/trayicon_hiragana.png \
                  images/trayicon_katakana.png \
                  images/trayicon_zenkakueisu.png \
                  images/trayicon_hankakukana.png \
                  dic/hiragana.dic \
                  dic/katakana.dic \
                  dic/hankakukana.dic \
                  dic/numeralsymbols.dic \
                  dic/dakuten.dic \
                  dic/kanainput.dic \
                  dic/zenkakualphabet.dic
DESTDIR         = .
OBJECTS_DIR     = .obj
MOC_DIR         = .moc
UI_DIR          = ui
DEFAULTCODEC    = eucJP

