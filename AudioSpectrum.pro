QT       += core gui widgets multimedia

CONFIG += c++1z
CONFIG += nostrip debug_info

DESTDIR = $$PWD/_bin

#LIBS += -L"C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.1A\\Lib"

SOURCES += \
	src/LevelMeterWidget.cpp \
	src/MainWindow.cpp \
	src/WaveWidget.cpp

HEADERS += \
	src/LevelMeterWidget.h \
	src/MainWindow.h \
	src/WaveWidget.h

FORMS += \
	src/MainWindow.ui

