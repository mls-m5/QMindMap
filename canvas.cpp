/*
 * canvas.cpp
 *
 *  Created on: 28 apr 2014
 *      Author: mattias
 */

#include "canvas.h"
#include <iostream>
#include <fstream>
#include <QtCore/QEvent>
#include <QtWidgets>


Canvas::Canvas(QWidget* parent) :
		selected(0), QPushButton(parent), font("sans", 10), textEdit(0) {
	setMinimumHeight(400);
//	{
//		auto rigidObject = new RigidObject();
//		rigidObject->rigidBody = physics.createRigidBox(btVector3(0, 0, 0),
//				btVector3(10, 10, 10), 1);
//		physics.AddRigidObject(rigidObject->rigidBody);
//		objects.push_back(rigidObject);
//		rigidObject->text = "apa";
//	}
//	{
//		auto rigidObject = new RigidObject();
//		rigidObject->rigidBody = physics.createRigidBox(btVector3(0, 0, 0),
//				btVector3(10, 10, 10), 1);
//		physics.AddRigidObject(rigidObject->rigidBody);
//		objects.push_back(rigidObject);
//		rigidObject->text = "bepa";
//	}
//	{
//		auto rigidObject = new RigidObject();
//		rigidObject->rigidBody = physics.createRigidBox(btVector3(0, 100, 0),
//				btVector3(10, 10, 10), 0);
//		physics.AddRigidObject(rigidObject->rigidBody);
//		objects.push_back(rigidObject);
//	}
	start(50);
}

Canvas::~Canvas() {
}


double linRand(double min, double max){
	return (double) rand() / RAND_MAX * (max - min) + min;
}

RigidObject* Canvas::createNewChildBox(QString text, RigidObject* parent) {
	btVector3 pos(0,0,0);
	if (selected){
		pos = selected->getOrigin();
		auto w = selected->width;
		auto h = selected->height;
		pos.setValue(linRand(-w, w) + pos.x(), linRand(-h, h) + pos.y(), 0);
	}
	return createNewBox(pos.x(), pos.y(), text, parent);
}

void Canvas::updateBox(QString text) {
	if (selected){
		selected->text = text;
	}
}

void Canvas::save() {
	QString filename("");
	if (filename.isEmpty()){
		filename = QFileDialog::getSaveFileName(this, "Spara, välj plats", "" ,"*.map");
	}
	if (filename.isEmpty()){
		return;
	}


	std::ofstream file(filename.toStdString());
	for (auto it: objects){
		if (it->level == 0){
			it->save(&file);
		}
	}
}

void Canvas::load(QString filename) {
	if (filename.isEmpty()){
		filename = QFileDialog::getOpenFileName(this, "Öppna fil", "" ,"*.map");
	}
	if (filename.isEmpty()){
		return;
	}

	std::ifstream file(filename.toStdString());

	if (!file.is_open()){
		return;
	}

	clear();

	std::string line;
	int level;
	RigidObject *parent = 0, *previous = 0;
	while (!file.eof()){
		std::getline(file, line);
		level = 0;
		for (int i = 0; i < line.size(); ++i){
			if (*line.begin() != '\t'){
				break;
			}
			line.erase(line.begin());
			++ level;
		}

		QString qline(line.c_str());
		if (previous){

			if(level > previous->level){
				parent = previous;
				previous = createNewChildBox(qline, previous);
			}
			else if(level < previous->level){
				do{
					parent = parent->rigidParent;
					if (parent == 0){
						break;
					}
				} while(parent->level >= level);
				previous = createNewChildBox(qline, parent);
			}
			else{
				previous = createNewChildBox(qline, parent);
			}
		}
		else{
			previous = createNewChildBox(qline, 0);
		}

	}
}

void Canvas::clear() {
	for (auto it: objects){
		physics.RemoveRigidObject(it->rigidBody);
		delete it;
	}
	objects.clear();
	selected = 0;
}

void Canvas::removeSelected() {
	if (selected){
		//Todo: Write recursive
		auto newSelected = selected->rigidParent;
		physics.removeRigidBody(selected->rigidBody);
		physics.RemoveRigidObject(selected->rigidBody);
		delete selected;
		selected = newSelected;
		update();
	}
}

void Canvas::transformPainter(QPainter *painter, RigidObject *object) {
	painter->translate(object->getOrigin().x(), object->getOrigin().y());
	auto angle = object->rigidBody->getWorldTransform().getRotation().getAngle()
			/ 3.1514 * 180.;
	if (object->rigidBody->getWorldTransform().getRotation().getZ() < 0) {
		angle = -angle;
	}
	painter->rotate(angle);
	painter->setFont(font);
	painter->translate(-object->width / 2., -object->height / 2);
}

void Canvas::paintEvent(QPaintEvent* e) {
	auto painter = new QPainter(this);
	painter->save();
	painter->setBrush(QColor("white"));
	painter->drawRect(e->rect());
	painter->setRenderHint(QPainter::Antialiasing, true);
	for (auto it: objects){
		auto p1 = it->getOrigin();
		if (it->rigidParent){
			auto p2 = it->rigidParent->getOrigin();
			auto f = (p2 - p1).normalize() * 10 / (1 + it->level);
//			auto dist = f.length();
//			f *= (100 - dist) * 10 / (1+it->level);

			it->rigidBody->applyCentralForce(f);
			it->rigidParent->rigidBody->applyCentralForce(-f);
		}
		it->rigidBody->setAngularVelocity(it->rigidBody->getAngularVelocity() / 1.1);
		it->rigidBody->setLinearVelocity(it->rigidBody->getLinearVelocity() / 1.1);
	}

	physics.Step(.1);
	painter->translate(100, 100);


	for (auto it: objects){
		auto p1 = it->getOrigin();
		for (auto it2 : it->rigidChildren){
			auto p2 = it2->getOrigin();
			painter->drawLine(p1.x(), p1.y(), p2.x(), p2.y());
		}
	}

	for (auto it : objects) {
		painter->save();
		transformPainter(painter, it);
		if (it == selected){
			painter->setBrush(QColor(100,100,250));
			painter->drawRoundRect(0, 0, it->width, it->height, 10);
		}
		else{
			painter->drawRect(0, 0, it->width, it->height);
		}
		painter->setBrush(QColor(0, 100, 100));
		QRect r(0, 0, it->width, it->height);
		painter->setBrush(QColor("black"));
		painter->drawText(r, Qt::AlignCenter, it->text);
		painter->restore();
	}
	painter->restore();
}

void Canvas::mousePressEvent(QMouseEvent* e) {
	createNewBox(e->x() - 100, e->y() - 100, textEdit->toPlainText());
}

RigidObject* Canvas::createNewBox(qreal x, qreal y, QString text, RigidObject *parent) {
	int level = 0;
	if (selected){
		level = selected->level + 1;
	}
	QFontMetrics fm(font);
	int pixelsWide = fm.width(text);
	int pixelsHigh = fm.height();
	auto rigidObject = new RigidObject(physics);
	if (pixelsWide / pixelsHigh < 10){
		rigidObject->rigidBody = physics.createRigidSphere(btVector3(x, y, 0),
				pixelsWide / 2, (double)10 / (1 + level));
	}
	else{
		rigidObject->rigidBody = physics.createRigidBox(btVector3(x, y, 0),
				btVector3(pixelsWide / 2, pixelsHigh / 2, 100), 1);
	}
	physics.AddRigidObject(rigidObject->rigidBody);
	rigidObject->rigidBody->setLinearFactor(btVector3(1, 1, 0));
	rigidObject->rigidBody->setAngularFactor(btVector3(0, 0, 1));
	rigidObject->text = text;
	rigidObject->width = pixelsWide;
	rigidObject->height = pixelsHigh;
	objects.push_back(rigidObject);

	if (!parent){
		parent = selected;
	}

	if (parent){
		parent->addChild(rigidObject);
		rigidObject->rigidParent = parent;
		rigidObject->level = parent->level + 1;
	}
	else{
		rigidObject->level = 0;
		rigidObject->rigidParent = 0;
	}
	selected = rigidObject;
	start(50);
	return rigidObject;
}

void Canvas::mouseReleaseEvent(QMouseEvent* e) {
}

void Canvas::mouseMoveEvent(QMouseEvent* e) {
}

void Canvas::timerEvent(QTimerEvent*) {
	for (auto it : objects) {
		if (it->rigidBody->isActive()) {
			update();
			return;
		}
	}
	stop();
}

void TextEntryBox::keyPressEvent(QKeyEvent* e) {

	if (e->key() == Qt::Key_Up){
		if (!canvas->selected){
			return;
		}
		if (!canvas->selected->rigidParent){
			return;
		}
		canvas->selected = canvas->selected->rigidParent;
		canvas->update();
	}
	else if (e->key() == Qt::Key_Right){
		if (canvas->selected){
			auto res = canvas->selected->getNextSibling();
			if (res){
				canvas->selected = res;
				canvas->update();
			}
		}
	}
	else if(e->key() == Qt::Key_Left){
		if (canvas->selected){
			auto res = canvas->selected->getPreviousSibling();
			if (res){
				canvas->selected = res;
				canvas->update();
			}
		}
	}
	else if(e->key() == Qt::Key_Down){
		if (canvas->selected){
//			auto res = canvas->selected->getFirstChild();
//			if (res){
//				canvas->selected == res;
//				canvas->update();
//			}
			if (!canvas->selected->rigidChildren.empty()){
				canvas->selected = canvas->selected->rigidChildren.front();
				canvas->update();
			}
		}
	}
	else if (e->key() == Qt::Key_Delete){
		canvas->removeSelected();
	}
	else if ((e->modifiers() & Qt::ControlModifier) && (e->key() == Qt::Key_S)){
		canvas->save();
	}
	else if ((e->modifiers() & Qt::ControlModifier) && (e->key() == Qt::Key_O)){
		canvas->load();
	}
	else if (e->key() == Qt::Key_Return) {
		canvas->createNewChildBox(toPlainText());
		clear();
	} else {
		QTextEdit::keyPressEvent(e);
//		canvas->updateBox(toPlainText());
//		canvas->update();
	}
}
