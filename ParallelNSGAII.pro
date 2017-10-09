VERSION = 1.1
TARGET = parallelngsa2
QT -= core testlib concurrent gui

TEMPLATE = app
CONFIG-=app_bundle

CONFIG += c++11

#DEFINES += USE_OPENMP
#DEFINES += USE_MPI

 contains(DEFINES,USE_MPI){

  QMAKE_CC = /usr/local/bin/mpicc
  QMAKE_CXX = /usr/local/bin/mpic++
  QMAKE_LINK = /usr/local/bin/mpic++

  QMAKE_CFLAGS += $$system(mpicc --showme:compile)
  QMAKE_CXXFLAGS += $$system(mpic++ --showme:compile)
  QMAKE_LFLAGS += $$system(mpic++ --showme:link)

  LIBS += -L/usr/local/lib/ -lmpi

  message("MPI enabled")

   } else {
      message("MPI disabled")
 }

macx{
    INCLUDEPATH += /usr/local/include
}

HEADERS += \
    global.h \
    rand.h \
    problemdef.h

SOURCES += \
    allocate.c \
    auxiliary.c \
    crossover.c \
    crowddist.c \
    decode.c \
    dominance.c \
    fillnds.c \
    initialize.c \
    list.c \
    merge.c \
    mutation.c \
    rand.c \
    rank.c \
    report.c \
    sort.c \
    tourselect.c \
    eval.cpp \
    problemdef.cpp \
    parallelnsga2r.cpp

CONFIG(debug, debug|release) {
   DESTDIR = ./build/debug
   OBJECTS_DIR = $$DESTDIR/.obj
   MOC_DIR = $$DESTDIR/.moc
   RCC_DIR = $$DESTDIR/.qrc
   UI_DIR = $$DESTDIR/.ui
}

CONFIG(release, debug|release) {
    DESTDIR = lib
    RELEASE_EXTRAS = ./build/release
    OBJECTS_DIR = $$RELEASE_EXTRAS/.obj
    MOC_DIR = $$RELEASE_EXTRAS/.moc
    RCC_DIR = $$RELEASE_EXTRAS/.qrc
    UI_DIR = $$RELEASE_EXTRAS/.ui
}
