######################################################################
# Automatically generated by qmake (3.1) Sat Aug 22 16:44:01 2020
######################################################################

QT += widgets
TEMPLATE = app
TARGET = qresampline
INCLUDEPATH += ./src

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += src/GraphicsView.hpp src/MainWindow.hpp src/ResampleDialog.hpp
FORMS += src/MainWindow.ui src/ResampleDialog.ui
SOURCES += src/GraphicsView.cpp src/main.cpp src/MainWindow.cpp src/ResampleDialog.cpp
TRANSLATIONS += src/qresampline_ru_RU.ts
