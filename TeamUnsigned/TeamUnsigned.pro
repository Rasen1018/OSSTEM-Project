QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cephaloform.cpp \
    cephimageview.cpp \
    cephpreset.cpp \
    cephvalueadjustment.cpp \
    dentalimageview.cpp \
    filterbuttonform.cpp \
    fourierprocessing.cpp \
    histogram.cpp \
    listwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    panopreset.cpp \
    panoramaform.cpp \
    panovalueadjustment.cpp \
    xraydb.cpp

HEADERS += \
    cephaloform.h \
    cephimageview.h \
    cephpreset.h \
    cephvalueadjustment.h \
    dentalimageview.h \
    filterbuttonform.h \
    fourierprocessing.h \
    histogram.h \
    listwidget.h \
    mainwindow.h \
    panopreset.h \
    panoramaform.h \
    panovalueadjustment.h \
    xraydb.h

FORMS += \
    cephaloform.ui \
    filterbuttonform.ui \
    listwidget.ui \
    mainwindow.ui \
    panoramaform.ui \
    xraydb.ui


CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin

!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
