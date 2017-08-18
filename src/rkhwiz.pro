#
# This file was generated from doxywizard.pro.in on Sat Jun  9 17:02:52 CEST 2012
#

######################################################################
# Automatically generated by qmake (2.01a) zo okt 19 12:50:02 2008
######################################################################

TEMPLATE     = app
DESTDIR      = bin
TARGET       = rkhwiz
DEPENDPATH  += .
INCLUDEPATH += .
QT          += xml
QT          += widgets
CONFIG      += debug
OBJECTS_DIR  = obj
MOC_DIR      = moc
RCC_DIR      = rcc
#DEFINES     += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

TRANSLATIONS = lang/SPANISH.ts

macx-g++ {
  CONFIG += x86 ppc
}

unix {
    COPYSRC = conf.ini \
        lang/SPANISH.qm \
        templates/rkhcfg.xml
    COPYDEST = $${DESTDIR}
    for(tmp,COPYSRC) {
        REALSRC = $$PWD/$$tmp
        REALDEST = $$COPYDEST/$$tmp
        QMAKE_POST_LINK += test \
            -d \
            $$dirname(REALDEST) \
            || \
            mkdir \
            $$dirname(REALDEST) \
            ;
        QMAKE_POST_LINK += cp \
            -f \
            $$REALSRC \
            $$REALDEST \
            ;
    }
}

# Input
HEADERS     += expert.h helplabel.h \
               inputbool.h inputstring.h inputint.h inputstrlist.h \
    rkhwiz.h \
    definitions.h \
    input.h \
    dtree.h \
    settingstree.h \
    variantdelegate.h \
    settingdialog.h
SOURCES     += expert.cpp \
               inputbool.cpp inputstring.cpp inputint.cpp inputstrlist.cpp \
    rkhwiz.cpp \
    dtree.cpp \
    settingstree.cpp \
    variantdelegate.cpp \
    settingdialog.cpp
LEXSOURCES  +=
RESOURCES   += rkhwiz.qrc
win32:RC_FILE += rkhwiz.rc
TMAKE_MOC = /usr/bin/moc

FORMS +=

OTHER_FILES += \
    theme.css \
    conf.ini \
    templates/rkhcfg.xml \
    Makefile
































