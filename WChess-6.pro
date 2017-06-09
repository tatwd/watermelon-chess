#-------------------------------------------------
#
# Project created by QtCreator 2017-06-03T19:06:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WChess-6
TEMPLATE = app


SOURCES += main.cpp\
        Board.cpp \
    Stone.cpp \
    ModelDlg.cpp \
    AboutDlg.cpp \
    HelpDlg.cpp \
    SingleGame.cpp \
    Step.cpp \
    ResignDlg.cpp \
    StandAloneDlg.cpp \
    MultiGame.cpp

HEADERS  += Board.h \
    Stone.h \
    ModelDlg.h \
    AboutDlg.h \
    HelpDlg.h \
    SingleGame.h \
    Step.h \
    ResignDlg.h \
    StandAloneDlg.h \
    MultiGame.h

FORMS    += Board.ui \
    ModelDlg.ui \
    AboutDlg.ui \
    HelpDlg.ui \
    ResignDlg.ui \
    StandAloneDlg.ui

RESOURCES += \
    Resources.qrc

QT += multimedia
