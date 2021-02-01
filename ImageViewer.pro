  QT += widgets
  requires(qtConfig(filedialog))
  qtHaveModule(printsupport): QT += printsupport

  win32:RC_ICONS += icon.ico

  HEADERS       = imageviewer.h \
  analysishelper.h \
  clickimagelabel.h \
  sortcommand.h
  SOURCES       = imageviewer.cpp \
    analysishelper.cpp \
    clickimagelabel.cpp \
                  main.cpp \
    sortcommand.cpp

  # install
  target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
  INSTALLS += target
