/*
 * RigidObject.h
 *
 *  Created on: 8 mar 2013
 *      Author: mattias
 */

#ifndef RIGIDOBJECT_H_
#define RIGIDOBJECT_H_

#include <btBulletDynamicsCommon.h>
#include <fstream>
#include <fstream>
#include <QtCore/QtCore>

#include "physics.h"

class RigidObject{
public:
	RigidObject(class Physics *physics);
	virtual ~RigidObject();

//	void Add(App *app);
	void setFilter(int filterType, int filterMask);
	btVector3 convertToWorldCoordinates(btVector3 v);
	btVector3 getOrigin(){return rigidBody->getWorldTransform().getOrigin();}
	btVector3 getAxis(int axis);
	void setOrigin(btVector3 pos);


//	virtual void save(SendArchive &ar) {
//		ar.save(rigidBody->getWorldTransform());
//		ar.save(rigidBody->getLinearVelocity());
//		ar.save(rigidBody->getAngularVelocity());
//	};
//
//	virtual void load(ReceiveArchive &ar) {
//		ar.load(rigidBody->getWorldTransform());
//		btVector3 t;
//		ar.load(t);
//		rigidBody->setLinearVelocity(t);
//		ar.load(t);
//		rigidBody->setAngularVelocity(t);
//	};

	void addChild(RigidObject *child){
		rigidChildren.push_back(child);
	}

	void removeChild(RigidObject *child){
		rigidChildren.remove(child);
	}

	RigidObject *getNextSibling(){
		if (rigidParent){
			return rigidParent->getNextChild(this);
		}
		return 0;
	}

	RigidObject *getPreviousSibling(){
		if (rigidParent){
			return rigidParent->getPreviousChild(this);
		}
		return 0;
	}

	RigidObject *getNextChild(RigidObject *c){
		for (auto it = rigidChildren.begin(); it != rigidChildren.end(); ++it){
			if (*it == c){
				it ++;
				if (it != rigidChildren.end()){
					return *it;
				}
				else{
					return 0;
				}
			}
		}
		return 0;
	}

	RigidObject *getPreviousChild(RigidObject *c){
		for (auto it = rigidChildren.begin(); it != rigidChildren.end(); ++it){
			if (*it == c){
				if (it != rigidChildren.begin()){
					it --;
					return *it;
				}
				else{
					return 0;
				}
			}
		}
		return 0;
	}

	RigidObject *getFirstChild(){
		if (rigidChildren.empty()){
			return 0;
		}
		return rigidChildren.front();
	}

	void save(std::ofstream *stream);
	void load(std::ifstream *stream);

	btRigidBody * rigidBody;
	int mask;
	int type;

	QString text;
	double width;
	double height;
	int level;
	std::list<RigidObject *> rigidChildren;
	RigidObject *rigidParent;
	Physics *physics;

};

#endif /* RIGIDOBJECT_H_ */
