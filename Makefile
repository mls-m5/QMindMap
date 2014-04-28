COMPILER=g++
RUNSTRING=./${TARGET}
OBJECTS=main.o physics.o rigidobject.o canvas.o
LIBS=
LIBS+=-lBulletDynamics -lBulletCollision -lLinearMath
LIBS+=-lQt5WebKitWidgets -lX11 -lxslt -lxml2 -lgio-2.0 -lgstapp-1.0 -lgstpbutils-1.0 -lgstvideo-1.0 -lgstaudio-1.0 -lgstbase-1.0 -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lsqlite3 -L/usr/lib/x86_64-linux-gnu -lQt5Sql -lQt5Quick -lQt5OpenGL -lQt5PrintSupport -lQt5WebKit -lQt5Qml -lQt5Widgets -lQt5Network -lQt5Location -lQt5Sensors -lQt5Gui -lQt5Core -lGL -lpthread
CPPFLAGS+=-I/usr/include/bullet -I/usr/share/qt5/mkspecs/linux-g++-64 -I. -I/usr/include/qt5 -I/usr/include/qt5/QtWebKitWidgets -I/usr/include/qt5/QtQuick -I/usr/include/qt5/QtOpenGL -I/usr/include/qt5/QtPrintSupport -I/usr/include/qt5/QtWebKit -I/usr/include/qt5/QtQml -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtNetwork -I/usr/include/qt5/QtLocation -I/usr/include/qt5/QtSensors -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore -I.
CPPFLAGS+= -g -std=c++11 -fPIC

TARGET=qtmm

all: .depend ${TARGET}

#Calculating dependincies
.depend: $(wildcard ./*.cpp ./*.h) Makefile
	$(CXX) $(CFLAGS) $(CPPFLAGS) -MM *.cpp > ./.dependtmp
	cat ./.dependtmp | sed 's/h$$/h \n\t \$(CXX) -c $$(CPPFLAGS) $$< -o $$@/' > ./.depend
	rm ./.dependtmp

${TARGET}: ${OBJECTS}
	${COMPILER} ${FLAGS} -o ${TARGET} ${OBJECTS} ${LIBS}

clean:
	rm *.o
	rm phys/*.o
	rm .depend

rebuild: clean ${TARGET}

include .depend