  QT += widgets
  requires(qtConfig(filedialog))
  qtHaveModule(printsupport): QT += printsupport

  HEADERS       = imageviewer.h \
  analysishelper.h \
  clickimagelabel.h
  SOURCES       = imageviewer.cpp \
    analysishelper.cpp \
    clickimagelabel.cpp \
                  main.cpp

  # install
  target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
  INSTALLS += target
