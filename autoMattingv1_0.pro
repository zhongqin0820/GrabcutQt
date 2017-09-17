#-------------------------------------------------
#
# Project created by QtCreator 2017-07-17T21:35:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = autoMattingv1_0
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
FORMS += \
        mainwindow.ui
INCLUDEPATH += ./maxflow/adjacency_list
#LIBS += -lcxcore200
#LIBS += -lcxcore210
DEPENDPATH += .
HEADERS += ./mainwindow.h \
    ./maxflow/adjacency_list/block.h \
    ./maxflow/adjacency_list/graph.h \
    ./Color.h \
    ./Global.h \
    ./GMM.h \
    ./GrabCut.h \
    ./Image.h \
    gallerywindow.h
SOURCES += ./main.cpp \
    ./mainwindow.cpp \
    ./maxflow/adjacency_list/graph.cpp \
    ./maxflow/adjacency_list/maxflow.cpp \
    ./Color.cpp \
    ./GMM.cpp \
    ./GrabCut.cpp \
    gallerywindow.cpp
#system mirror files
RESOURCES += sdi.qrc
#added opencv dependency
INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2

LIBS += -L/usr/local/lib \
 -lopencv_core \
 -lopencv_highgui \
 -lopencv_imgproc \

DISTFILES +=
CONFIG +=console
