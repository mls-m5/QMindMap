/*
 * canvas.h
 *
 *  Created on: 28 apr 2014
 *      Author: mattias
 */

#pragma once

#include "canvas.h"

#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtextedit.h>
#include <QtCore/QtCore>

#include "physics.h"

#include "rigidobject.h"



class Canvas : public QPushButton, public QTimer {
public:
	friend class TextEntryBox;
	Canvas(QWidget* parent);

	~Canvas();

	void paintEvent(QPaintEvent* e);

	void mousePressEvent(QMouseEvent* e);

	RigidObject *createNewBox(qreal x, qreal y, QString text, RigidObject *parent = 0);
	RigidObject *createNewChildBox(QString text, RigidObject *parent = 0);
	void removeSelected();
	void updateBox(QString text);
	void save();
	void load(QString filename = "");
	void clear();

	void mouseReleaseEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);

    QTextEdit * textEdit;

    QFont font;
protected:
	void timerEvent(QTimerEvent*);
	void transformPainter(QPainter *painter, RigidObject *object);
	void stepPhysics();

    Physics physics;
    std::list<RigidObject*> objects;

    RigidObject *selected;
    QString filename;
};


class TextEntryBox : public QTextEdit {
public:
	TextEntryBox(QWidget *parent = 0):
		QTextEdit(0),
		canvas(0){}

	virtual void keyPressEvent(QKeyEvent* e);

    void setCanvas(Canvas * c){
    	canvas = c;
    }

protected:
    Canvas *canvas;
};
