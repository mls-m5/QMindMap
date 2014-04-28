/*
 * physics.cpp
 *
 *  Created on: 9 jan 2013
 *      Author: mattias
 */

#include "physics.h"
#include <bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <math.h>

Physics::Physics() {

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);

//	dynamicsWorld->setGravity(btVector3(0,20,0));
//	dynamicsWorld->setGravity(btVector3(0,1,0));
	dynamicsWorld->setGravity(btVector3(0, 0, 0));
}

void Physics::Step(double t) {
	dynamicsWorld->stepSimulation(t, 10, t);
}

void Physics::clear() {
	int i;
	//remove the rigidbodies from the dynamics world and delete them
	for (i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//remove the constraints
	while (dynamicsWorld->getNumConstraints() > 0){
		dynamicsWorld->removeConstraint(dynamicsWorld->getConstraint(0));
	}

	//delete collision shapes
	for (int j=0;j<collisionShapes.size();j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}
}


void Physics::removeRigidBody(btRigidBody *body) {
	dynamicsWorld->removeCollisionObject(body);
}


Physics::~Physics() {
	clear();

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();


}

void Physics::AddConstraint(btTypedConstraint *c){
	dynamicsWorld->addConstraint(c);
}

void Physics::AddRigidObject(btRigidBody* body, int mask, int filter) {
	dynamicsWorld->addRigidBody(body, mask, filter);
//	dynamicsWorld->addRigidBody(body);
}

btCollisionShape* Physics::getCollisionBox(btVector3 size) {
	btCollisionShape *boxShape = new btBoxShape(
			btVector3(btScalar(size.x()), btScalar(size.y()), btScalar(size.z())));
	collisionShapes.push_back(boxShape);
	return boxShape;
}

btCollisionShape* Physics::getCollisionCapsule(btScalar len, btScalar radius) {
	btCollisionShape *capsuleShape = new btCapsuleShape(radius, len);
	collisionShapes.push_back(capsuleShape);
	return capsuleShape;
}

btCollisionShape* Physics::getCollisionSphere(btScalar radius) {
	btCollisionShape *sphereShape = new btSphereShape(radius);
	collisionShapes.push_back(sphereShape);
	return sphereShape;
}

void Physics::RemoveRigidObject(btRigidBody* body) {
	dynamicsWorld->removeRigidBody(body);
}

void Physics::RemoveConstraint(btTypedConstraint * constraint) {
	dynamicsWorld->removeConstraint( constraint);
}

Physics::IntersectResult Physics::Intersect(btVector3 pos, btVector3 v, int mask, void *ignoreUserPointer) {
	IntersectResult res;
	res.isHit = 0;
	btVector3 to(pos + v);

	btCollisionWorld::AllHitsRayResultCallback allResults(pos, to);
	allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
	allResults.m_collisionFilterMask = mask;
	dynamicsWorld->rayTest(pos,to, allResults);

	int nearest;
	btRigidBody * rigidBody;
	int fraction = 1.1;
	for (int i=0; i<allResults.m_hitFractions.size(); i++){
		rigidBody = (btRigidBody*)btRigidBody::upcast(allResults.m_collisionObjects[i]);
		if (ignoreUserPointer == 0 || (ignoreUserPointer != 0 && allResults.m_collisionObjects[i]->getUserPointer() != ignoreUserPointer)){
			if (allResults.m_hitFractions[i] < fraction){
				fraction = allResults.m_hitFractions[i]; //Todo: fraction is always 0
				res.rigidBody = rigidBody;
				res.normal = allResults.m_hitNormalWorld[i];
				res.isHit = true;
			}
		}
	}

	if (res.isHit){
		res.pos = pos + v * fraction;
	}
	return res;
}

btRigidBody* Physics::CreateRigidBody(btVector3 pos, btScalar ang, btScalar smass,
		btCollisionShape* cShape) {
	btTransform transform;
	transform.setIdentity();
	btQuaternion quat;
	quat.setEuler(0,0,ang);
	transform.setRotation(quat);
	transform.setOrigin(pos);
	btVector3 localInertia(0, 0, 0);
	//Objects with mass zero is static
	//Otherwise dynamic
	if (smass != 0.) {
		cShape->calculateLocalInertia(smass, localInertia);
	}
	//Motionstates is recomended, but not necessary
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(smass, motionState, cShape,
			localInertia);
	btRigidBody* Body = new btRigidBody(rbInfo);

	//Make the body only move in 2d
//	Body->setLinearFactor(btVector3(1,1,0));
//	Body->setAngularFactor(btVector3(0,0,1));
//	dynamicsWorld->addRigidBody(Body);
	return Body;
}

btRigidBody* Physics::createRigidBox(btVector3 pos, btVector3 size, double mass) {
	btScalar smass(mass);


	btCollisionShape* cShape = getCollisionBox(size);

	btRigidBody* Body = CreateRigidBody(pos, 0, smass, cShape);

	return Body;
}

btRigidBody* Physics::createRigidCapsule(btVector3 p1, btVector3 p2,
		btScalar radius, double mass) {

	btScalar smass(mass);

	btScalar len = (p1 - p2).length();

	auto ang = -atan2(p2.x() - p1.x(), p2.y() - p1.y()) ;

	btCollisionShape *cShape = getCollisionCapsule(len, radius);

	auto Body = CreateRigidBody((p1 + p2) / 2.,ang,smass, cShape);

	return Body;

}

btRigidBody* Physics::createRigidSphere(btVector3 pos,
		btScalar radius, double mass) {

	btScalar smass(mass);

	btCollisionShape *cShape = getCollisionSphere(radius);

	auto Body = CreateRigidBody(pos,0,smass, cShape);

	return Body;

}



