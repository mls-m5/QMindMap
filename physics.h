/*
 * physics.h
 *
 *  Created on: 9 jan 2013
 *      Author: mattias
 */

#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "LinearMath/btVector3.h"
#include "common.h"
#include "rigidobject.h"

class Physics {
public:
	struct IntersectResult{
		bool isHit;
		btVector3 pos, normal;
		btRigidBody *rigidBody;
	};

	Physics();
	void Step(double t);
	virtual ~Physics();

	void clear();
	void removeRigidBody(btRigidBody *body);

	btCollisionShape* getCollisionBox(btVector3 size);
	btCollisionShape* getCollisionCapsule(btScalar len, btScalar radius);
	btCollisionShape* getCollisionSphere(btScalar radius);
	btRigidBody* createRigidBox(btVector3 pos, btVector3 size, double mass);
	btRigidBody* createRigidCapsule(btVector3 p1, btVector3 p2, btScalar radius, double mass);
	btRigidBody* createRigidSphere(btVector3 pos, btScalar radius, double mass);
	void AddRigidObject(btRigidBody *body, int mask = COL_BOX, int filter = COL_ALL);
	void AddConstraint(btTypedConstraint *c);
	void RemoveRigidObject(btRigidBody *body);
	void RemoveConstraint(btTypedConstraint *constraint);
	IntersectResult Intersect(btVector3 pos, btVector3 v, int mask = COL_BOX, void *ignoreUserPointer = 0);
private:

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	btRigidBody* CreateRigidBody(btVector3 pos, btScalar ang, btScalar smass,
			btCollisionShape* cShape);

};

#endif /* PHYSICS_H_ */
