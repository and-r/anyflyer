#include "uavnode.h"
#include "settingsmgr.h"  //zeby przeciazac operator<< dla wektorów

#include <iostream>//do wykasowania
using namespace std;//do wykasowania

float UavNode::fGaccel = 9.81;  //przyspieszenie ziemskie w m/s^2
float UavNode::fRozero = 1.225;  //gestosc powietrza na poziomie 0, kg/m^3
float UavNode::fTzero = 288.16;  //temperatura powietrza na posiomie 0, K
float UavNode::fNi = 1.48e-5;   //lepkość kinematyczna powietrza
float UavNode::fRollcoefrough = 0.1;	//rolling resistance coefficient, rough terrain
float UavNode::fRollcoefconcrete = 0.02;		//rolling resistance coefficient, concrete
float UavNode::fGripcoefrough = 0.25;		//grip coefficient, rough terrain
float UavNode::fGripcoefconcrete = 0.5;		//grip coefficient, concrete
float UavNode::fGripspeed = 1;	//speed above which there is full grip coefficient, m/s
VectorIntegralFuncType UavNode::pVfunc=nullptr;
QuaternionIntegralFuncType UavNode::pQfunc=nullptr;
UavNode::UavNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, scene::IMesh* mesh, const char type[])  //konstruktor - jedyny, argumentowy
	: scene::ISceneNode(parent, mgr, id)
{
	sType = type;
	pDriver = mgr->getVideoDriver();  //getting pointer to graphic card, from scene manager
	pCollMan = mgr->getSceneCollisionManager();
	pMeshChild = mgr->addMeshSceneNode(mesh, static_cast<scene::ISceneNode*>(this), id);
	pParticSys = mgr->addParticleSystemSceneNode(true, static_cast<scene::ISceneNode*>(this), 0);
	scene::IParticleEmitter* em = pParticSys->createBoxEmitter(core::aabbox3df(-1,-1,-1,1,1,1),
		PARTICLESPEED,MINPARTICLESRATE,MAXPARTICLESRATE,video::SColor(0,20,20,20),video::SColor(255,255,255,255),4000u,8000u,20,core::dimension2df(2.0f,2.0f),core::dimension2df(3.0f,3.0f));
	scene::IParticleAffector* particaf_scale = pParticSys->createScaleParticleAffector(core::dimension2df(50.0f, 50.0f));
	pParticSys->addAffector(particaf_scale);
	particaf_scale->drop();
	scene::IParticleAffector* particaf_fade = pParticSys->createFadeOutParticleAffector();
	pParticSys->addAffector(particaf_fade);
	
	particaf_fade->drop();
	pParticSys->setEmitter(em);
	em->drop();
	pParticSys->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	string smokepath = "aircraft/" + sType;
	smokepath += "/smoke.bmp";
	pParticSys->setMaterialTexture(0, pDriver->getTexture(smokepath.data()));
	pParticSys->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	pParticSys->getEmitter()->setMaxParticlesPerSecond(0U);

	pMeshChild->setMaterialFlag(video::EMF_LIGHTING, true);  //dynamic light
	pMeshChild->addShadowVolumeSceneNode();  //with this line, it will cast shadows 
}


void UavNode::Crash(core::vector3df collpoint)
{
    Speed=core::vector3df{};
    setPosition(collpoint);
    eSTATE=FLIGHTSTATE::CRASHED;
	pParticSys->getEmitter()->setMaxParticlesPerSecond(MAXPARTICLESRATE);
}
