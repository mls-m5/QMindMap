
#include <iostream>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qtextedit.h>
#include <QtCore>
#include <qpainter.h>
#include <QtGui>
#include "physics.h"
#include <qrect.h>

#include "canvas.h"


int main(int argc, char **argv) {
    QApplication app(argc, argv);

    auto groupBox = new QGroupBox;
    auto boxlayout = new QVBoxLayout;


    auto button = new Canvas(0);
    auto text = new TextEntryBox(0);
    text->setFixedHeight(100);
    text->setCanvas(button);
    boxlayout->addWidget(button);
    boxlayout->addWidget(text);

    button->textEdit = text;
//    button->show();

    groupBox->setLayout(boxlayout);
    groupBox->resize(800,800);

    groupBox->show();

    return app.exec();
}
