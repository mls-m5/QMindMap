/*
 * RigidObject.cpp
 *
 *  Created on: 8 mar 2013
 *      Author: mattias
 */

#include "rigidobject.h"

RigidObject::RigidObject(Physics *physics):
level(0),
rigidParent(0),
physics(physics)
{
	rigidBody = 0;
}

RigidObject::~RigidObject() {
	physics->removeRigidBody(rigidBody);
	physics->RemoveRigidObject(rigidBody);
	for (auto it: rigidChildren){
		delete it;
	}
}

void RigidObject::setFilter(int filterType, int filterMask) {
	type = filterType;
	mask = filterMask;

}

btVector3 RigidObject::convertToWorldCoordinates(btVector3 v) {
	return rigidBody->getWorldTransform() * v;
}

btVector3 RigidObject::getAxis(int axis) {
	return rigidBody->getWorldTransform().getBasis().getColumn(axis);
}

void RigidObject::setOrigin(btVector3 pos) {
	auto transform = rigidBody->getWorldTransform();
	transform.setOrigin(pos);
	rigidBody->setWorldTransform(transform);
}

void RigidObject::save(std::ofstream* stream) {
	for (int i = 0; i < level; ++i){
		stream->write("\t", 1);
	}
	*stream << text.toStdString() << "\n";
	for (auto it: rigidChildren){
		it->save(stream);
	}
}

void RigidObject::load(std::ifstream* stream) {
}
