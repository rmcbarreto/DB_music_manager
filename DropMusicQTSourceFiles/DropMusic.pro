#-------------------------------------------------
#
# Project created by QtCreator 2018-11-14T14:46:45
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DropMusic
TEMPLATE = app
QTPLUGIN += QSQLMYSQL

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    createplaylist.cpp \
    login.cpp \
    comment.cpp \
    sharemusic.cpp \
    addinformation.cpp \
    addmusicianstogroup.cpp \
    addmusicstoalbum.cpp \
    addcomposertomusic.cpp \
    customqry.cpp \
    addconcert.cpp \
    addconcertmusic.cpp \
    informations.cpp

HEADERS += \
        mainwindow.h \
    createplaylist.h \
    login.h \
    comment.h \
    sharemusic.h \
    addinformation.h \
    addmusicianstogroup.h \
    addmusicstoalbum.h \
    addcomposertomusic.h \
    customqry.h \
    addconcert.h \
    addconcertmusic.h \
    informations.h

FORMS += \
        mainwindow.ui \
    createplaylist.ui \
    login.ui \
    comment.ui \
    sharemusic.ui \
    addinformation.ui \
    addmusicianstogroup.ui \
    addmusicstoalbum.ui \
    addcomposertomusic.ui \
    customqry.ui \
    addconcert.ui \
    addconcertmusic.ui \
    informations.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
