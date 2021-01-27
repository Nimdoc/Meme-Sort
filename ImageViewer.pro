  QT += widgets
  requires(qtConfig(filedialog))
  qtHaveModule(printsupport): QT += printsupport

  HEADERS       = imageviewer.h \
  clickimagelabel.h
  SOURCES       = imageviewer.cpp \
    clickimagelabel.cpp \
                  main.cpp

  # install
  target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
  INSTALLS += target
