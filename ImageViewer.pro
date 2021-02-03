  QT += widgets
  requires(qtConfig(filedialog))
  qtHaveModule(printsupport): QT += printsupport

  win32:RC_ICONS += icon.ico

  HEADERS       = \
  analysishelper.h \
  clickimagelabel.h \
  memesort.h \
  sortcommand.h
  SOURCES       = \
    analysishelper.cpp \
    clickimagelabel.cpp \
                  main.cpp \
    memesort.cpp \
    sortcommand.cpp

  # install
  target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
  INSTALLS += target
