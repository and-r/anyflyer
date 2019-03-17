#ifndef UAVNODE_H
#define UAVNODE_H
#include <irrlicht.h>
#include <cmath>
#include "settingsmgr.h"  //zeby przeciazac operator<< dla wektorów
#include <iostream>//do wykasowania
const unsigned MINPARTICLESRATE = 5;
const unsigned MAXPARTICLESRATE = 10;
const core::vector3df PARTICLESPEED(0, 0.002f, 0);
using namespace std;//do wykasowania

using namespace irr;
constexpr float PI=3.141592654;
enum class FLIGHTSTATE{FLYING=0,LANDED=1,CRASHED=2};
enum class COMPONENTTYPE{FUSELAGE=0,AIRFOIL=1,ENGINE=2,GEAR=3};
enum class FLAPSTATE{NOFLAPS=0,CRUISE=1,TAKEOFF=2,INTERMEDIATE=3,LANDING=4};
using VectorIntegralFuncType = core::vector3df (*)(core::vector3df,core::vector3df,float);
using QuaternionIntegralFuncType = core::quaternion (*)(core::vector3df,core::vector3df,float);

struct FoilProps
{
    float fControlPower;
    float fControlAngleUp;  //max angle up of the control surface, radians
    float fControlAngleDown;  //max angle down of the control surface, radians
	float fFlapAngle;		//max angle down for the surface to act as a flap,radians
    core::vector3df ControlSensit;  //influence of ailerons, elevator and rudder, limit values: -1 to 1
    core::vector3df Chart[36];  //CL/CD table of this foil
};
struct FuselageProps
{
    float fLength025;  //0.25 of fuselage length, at this distance from the front tip is the aerodynamic force applied
    float fCstream;		
    float fCcyl;
};
struct EngineProps
{
    float fMaxThrust;
};
struct GearProps
{
	bool bIntact;		//current status intact/damaged
	bool bSwivel;		//loaded from file, is the wheel self aligning
	bool bBraked;		//loaded from file
	float fDeflection;  //deflection at current frame
	float fTravel;			//loaded from file,	maximum deflection that the gear withstands
	float fSpringCoef;	//loaded from file	
	float fDampCoef;	//loaded from file
	core::vector3df  RetrAxisLocation;	//loaded from file, location of retraction axis
	core::vector3df  RetrAxisRotation;	//loaded from file, location of retraction angle
};

struct Component
{
    COMPONENTTYPE eCOMPTYPE;
    core::vector3df Location;
    core::matrix4 Rotation;
    core::vector3df Xvector;  //wektor jednostkowy kierunku X komponentu, w układzie aircraft
    core::vector3df Yvector;  //analogicznie
    core::vector3df Zvector;
	scene::IMeshSceneNode* MeshObject = nullptr;  //additional visible mesh object representing given component
    union
    {
        FoilProps FoilP;
        FuselageProps FuselageP;
        EngineProps EngineP;
		GearProps GearP;
    };
    Component(){}  //musi być konstruktor, kompilator nie tworzy konstruktora domyślnego bo w klasie jest unia z structami/klasami i nie wie
    //której klasy/structa konstruktor wywołać
};

class UavArray;  //deklaracja zapowiadająca

class UavNode : public scene::ISceneNode
{
private:
    string sType;
    video::IVideoDriver* pDriver;
    scene::IMeshSceneNode* pMeshChild=nullptr;
    scene::IParticleSystemSceneNode* pParticSys = nullptr;
	scene::ISceneCollisionManager* pCollMan = nullptr;
	vector<scene::IMeshSceneNode*> vTerrain;
	int iSoftTerrainNum = 0;
    static VectorIntegralFuncType pVfunc;
    static QuaternionIntegralFuncType pQfunc;
    float fMass=1;  //żeby nie było zero
    core::vector3df Cog{};  //środek masy, w układzie lokalnym maszyny
    core::vector3df MomInert{};  //momenty bezwładności
    int iCompNum;  //liczba komponentow
	Component* pComp = nullptr;  //wskaźnik do tablicy dynamicznej komponentów
    core::vector3df FPPCamPos{};  //pozycja kamery FPP względem punktu 0 UavNode
    core::vector3df ControlSignals{};  //aktualne wychylenia powierzchni sterowych
	float fBrakeRightSignal{};
	float fBrakeLeftSignal{};
    int iPower = 75;  //aktualna moc silnika
    int iPitchTrim = 0;
	float fGearTransferTime;	//loaded from file, time of retracting or extending gear in seconds	
	float fGearTimeToTransfer;	//time to complete current operation of retracting or extending in seconds, positive: from up to down, negative: down to up
	bool bRetractableGear;		//true:retractable, false:fixed, loaded from file	
	bool bGearDown;			//current position up/down
	float fFlapTransferTime;	//loaded from file, time of retracting or extending flaps between neighboring states in seconds
	float fFlapTimeToTransfer;	//time to complete current operation of retracting or extending in seconds, positive, up to down, negative: down to up

public:
	UavNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id, scene::IMesh* mesh, const char type[]);  //constructor, one and only

    virtual ~UavNode()  //destruktor
    {
        if (pComp)
        {
            delete [] pComp;  //niszczy tablicę komponentów
        }
    }
    static float fGaccel;   //przyspieszenie ziemskie
    static float fRozero;   //gęstość powietrza na wysokości zero
    static float fTzero;    //temperatura na wysokości 0
    static float fNi;   //lepkość kinematyczna
	static float fRollcoefrough;	//rolling resistance coefficient, rough terrain
	static float fRollcoefconcrete;		//rolling resistance coefficient, concrete
	static float fGripcoefrough;		//grip coefficient, rough terrain
	static float fGripcoefconcrete;		//grip coefficient, concrete
	static float fGripspeed;	//speed above which there is full grip coefficient, m/s
    friend class UavArray;
    FLIGHTSTATE eSTATE=FLIGHTSTATE::FLYING;
	FLAPSTATE eFLAPSTATE = FLAPSTATE::NOFLAPS;
    core::vector3df Speed{};
    core::vector3df RotSpeed{};
    virtual void OnRegisterSceneNode()
    {
        ISceneNode::OnRegisterSceneNode();
    }
    virtual void render()
    {
        //tutaj nic nie ma, bo widoczne ma być tylko dziecko czyli pMeshChild
    }
    virtual const core::aabbox3d<f32>& getBoundingBox() const
    {
        return pMeshChild->getBoundingBox();
    }
    virtual u32 getMaterialCount()
    {
        return pMeshChild->getMaterialCount();
    }
    virtual video::SMaterial& getMaterial(u32 num)
    {
        return pMeshChild->getMaterial(num);
    }
	virtual void setType(string type)
	{
		sType = type;
	}
	virtual string getType()
	{
		return sType;
	}
    virtual scene::IMeshSceneNode* getMeshChild()
    {
        return pMeshChild;
    }
    virtual core::vector3df getFPPCamPos()
    {
        return FPPCamPos;
    }
    virtual void setControlSignals(const core::vector3df& signals)
    {
        ControlSignals = signals;
        //dodajemy trym do sygnału
        ControlSignals.X+=iPitchTrim/100.0f;
        if (ControlSignals.X>1)
        {
            ControlSignals.X=1;
        }
        else
        {
            if (ControlSignals.X<-1)
            {
                ControlSignals.X=-1;
            }
        }
        if (ControlSignals.Y>1)
        {
            ControlSignals.Y=1;
        }
        else
        {
            if (ControlSignals.Y<-1)
            {
                ControlSignals.Y=-1;
            }
        }
        if (ControlSignals.Z>1)
        {
            ControlSignals.Z=1;
        }
        else
        {
            if (ControlSignals.Z<-1)
            {
                ControlSignals.Z=-1;
            }
        }

    }
	virtual void SetOtherSignals(const float brakeright, const float brakeleft)
	{
		fBrakeRightSignal = brakeright;
		fBrakeLeftSignal = brakeleft;
		if (brakeright > 1)
		{
			fBrakeRightSignal = 1;
		}
		if (brakeleft > 1)
		{
			fBrakeLeftSignal = 1;
		}
	}
	virtual void GetOtherSignals(float& brakeright,float& brakeleft)
	{
		brakeright = fBrakeRightSignal;
		brakeleft = fBrakeLeftSignal;
	}
    virtual void AddPitchTrim(int incrementpitch)
    {
        iPitchTrim+=incrementpitch;
        if (iPitchTrim > 100)
        {
            iPitchTrim = 100;
        }
        else
        {
            if (iPitchTrim <-100)
            {
                iPitchTrim = -100;
            }
        }
    }

    virtual const core::vector3df& getControlSignals()
    {
        return ControlSignals;
    }
    virtual void AddPower()
    {
        iPower+=5;
        if (iPower > 100){iPower = 100;}  //upper limit
    }
    virtual void DecreasePower()
    {
        iPower-=5;
        if (iPower < 0){iPower = 0;}  //lower limit
    }
    virtual int getPower()
    {
        return iPower;
    }
	virtual void DecreaseFlaps()
	{
		if ((eFLAPSTATE == FLAPSTATE::NOFLAPS) || (eFLAPSTATE == FLAPSTATE::CRUISE)) { return; }  //there are no flaps, or in upper position, do not change anything
		if (fFlapTimeToTransfer != 0) { return; }  //flaps are currently being transferred, no action now
		fFlapTimeToTransfer = -fFlapTransferTime; //set beginning of tranfer from lower to upper position
	}
	virtual void IncreaseFlaps()
	{
		if ((eFLAPSTATE == FLAPSTATE::NOFLAPS) || (eFLAPSTATE == FLAPSTATE::LANDING)) { return; }  //there are no flaps, or in lowest position, do not change anything
		if (fFlapTimeToTransfer != 0) { return; }  //flaps are currently being transferred, no action now
		fFlapTimeToTransfer = fFlapTransferTime; //set beginning of tranfer from upper to lower position
	}
	virtual void ChangeGear()
	{
		if ((bRetractableGear == false) || (fGearTimeToTransfer != 0)) { return; }  //gear is not retractable or is curretnly being changed
		if (bGearDown == true) 
		{ fGearTimeToTransfer = -fGearTransferTime; }
		else
		{fGearTimeToTransfer = fGearTransferTime;}
	}
	virtual void getFlapsAndGear(int& flapstate, int& gearstate)
	{
		if (fFlapTimeToTransfer != 0)
		{
			flapstate = -1;
		}
		else
		{
			flapstate = static_cast<int>(eFLAPSTATE);
		}
		if (bRetractableGear == false)
		{
			gearstate = 2; //fixed gear
		}
		else
		{
			if (fGearTimeToTransfer != 0)
			{
				gearstate = -1;
			}
			else
			{
				gearstate = static_cast<int>(bGearDown); //0: up position, 1: down position
			}
		}
	}
    static void setpVfunc(VectorIntegralFuncType a)
    {
        pVfunc=a;
    }
    static void setpQfunc(QuaternionIntegralFuncType a)
    {
        pQfunc=a;
    }

    virtual void Crash(core::vector3df);

    virtual void Visibility(bool visible)
    {
        if (visible)
        {
            pMeshChild->setVisible(true);
        }
        else
        {
            pMeshChild->setVisible(false);
        }
    }

    //metody całkujące wektorowe
    static core::vector3df IntegralEulerVector(core::vector3df vm1, core::vector3df v0,float deltat)
    {
        return vm1*deltat;
    }
    static core::vector3df IntegralTrapezeVector(core::vector3df vm1, core::vector3df v0, float deltat)
    {
        return (vm1+v0)*0.5*deltat;
    }

    //metody całkujące quaternionowe
    static core::quaternion IntegralEulerQuaternion(core::vector3df rotvm1,core::vector3df rotv0, float deltaseconds)
    {
        rotvm1*=deltaseconds;
        float anglerad=rotvm1.getLength();
        rotvm1.normalize();
        core::quaternion q;
        q.fromAngleAxis(anglerad,rotvm1);
        return q;
    }
    static core::quaternion IntegralTrapezeQuaternion(core::vector3df rotvm1,core::vector3df rotv0, float deltaseconds)
    {
        rotvm1+=rotv0;
        rotvm1*=(0.5f*deltaseconds);
        float anglerad=rotvm1.getLength();
        rotvm1.normalize();
        core::quaternion q;
        q.fromAngleAxis(anglerad,rotvm1);
        return q;
    }
	core::vector3df Move(float deltaseconds, core::vector3df wind)
	{
		//    static int cntr=0;//tymczasowo!!!
		//Actions on flaps
		if (fFlapTimeToTransfer > 0)
		{
			fFlapTimeToTransfer -= deltaseconds;
			if (fFlapTimeToTransfer <= 0)
			{ 
				fFlapTimeToTransfer = 0;
				int* statenumber = reinterpret_cast<int*>(&eFLAPSTATE);
				*statenumber += 1;
			}
		}
		if (fFlapTimeToTransfer < 0)
		{
			fFlapTimeToTransfer += deltaseconds;
			if (fFlapTimeToTransfer >= 0)
			{ 
				fFlapTimeToTransfer = 0;
				int* statenumber = reinterpret_cast<int*>(&eFLAPSTATE);
				*statenumber -= 1;
			}
		}
		//Actions on retractable landing gear
		if (fGearTimeToTransfer > 0)
		{
			fGearTimeToTransfer -= deltaseconds;
			for (int i = 0; i < iCompNum; ++i)  //  setting proper angle when moving gear OUT, and setting it visible at the beginning
			{
				if (pComp[i].MeshObject !=nullptr)
				{
					pComp[i].MeshObject->setVisible(true);
					if (pComp[i].eCOMPTYPE == COMPONENTTYPE::GEAR)
					{
						float anglefraction = fGearTimeToTransfer / fGearTransferTime;
						pComp[i].MeshObject->setRotation(pComp[i].GearP.RetrAxisRotation*anglefraction);
					}
				}
			}
			if (fGearTimeToTransfer <= 0)
			{
				fGearTimeToTransfer = 0;
				bGearDown = true;
			}
		}
		if (fGearTimeToTransfer < 0)
		{
			fGearTimeToTransfer += deltaseconds;
			for (int i = 0; i < iCompNum; ++i)  //  setting proper angle when moving gear IN, and setting it INvisible at the end
			{
				if ( pComp[i].MeshObject != nullptr)
				{
					if (pComp[i].eCOMPTYPE == COMPONENTTYPE::GEAR)
					{
						float anglefraction = 1 + fGearTimeToTransfer / fGearTransferTime;
						pComp[i].MeshObject->setRotation(pComp[i].GearP.RetrAxisRotation*anglefraction);
					}
					if (fGearTimeToTransfer >= 0)
					{
						pComp[i].MeshObject->setVisible(false);
					}
				}
			}
			if (fGearTimeToTransfer >= 0)
			{
				fGearTimeToTransfer = 0;
				if (bRetractableGear == true)
				{
					bGearDown = false;

				}
			}
		}

		//int* statenumber = reinterpret_cast<int*>(&eFLAPSTATE);
		//*statenumber -= 1;
												   //najpierw wartości liniowe
		core::vector3df oldspeed = Speed;  //zapisanie poprzedniej wartości
		core::vector3df forcetotal{};  //wektor główny siły, wyzerowany
		core::vector3df momenttotal{};  //moment główny, wyzerowany

		Reaction(deltaseconds, forcetotal, momenttotal, wind);  //otrzymujemy wektor główny siły i mom. główny w układzie samolot
											
		AbsoluteTransformation.rotateVect(forcetotal);  //teraz forcetotal jest w układzie globalnym
        core::vector3df acceltotal = (forcetotal / fMass) + core::vector3df(0, -fGaccel, 0);  //suma przyśpieszeń działająca na samolot
																							  //core::vector3df acceltotal=(core::vector3df(0,0,0)/fMass)+core::vector3df(0,-fGaccel,0);  //suma przyśpieszeń działająca na dron

		Speed += acceltotal*(deltaseconds);  //scałkowane przyśpieszenie = przyrost prędkości

											 //całkowanie prędkości liniowej = położenie
		setPosition(getPosition() + (*pVfunc)(oldspeed, Speed, deltaseconds));

		//później wartości kątowe
		core::vector3df oldrotspeed = RotSpeed;  //zapisanie poprzedniej wartości

												 //tutaj trzeba obliczyć przyrost prędkości kątowej wynikający z działania  momentu głównego i dodać go do RotSpeed
												 //core::vector3df pewienwektor=(momenttotal/MomInert)*deltaseconds*180/PI;
												 //cout<<"przyrost deg/s="<<pewienwektor<<endl<<endl;
		RotSpeed += (momenttotal / MomInert)*deltaseconds;
		if (RotSpeed.X*oldrotspeed.X<0)
		{
			RotSpeed.X = 0;
		}
		if (RotSpeed.Y*oldrotspeed.Y<0)
		{
			RotSpeed.Y = 0;
		}
		if (RotSpeed.Z*oldrotspeed.Z<0)
		{
			RotSpeed.Z = 0;
		}

		//całkowanie prędkości obrotowej = orientacja
		core::quaternion rottm1(getAbsoluteTransformation());  //najpierw quaternion ze starej orientacji
		core::quaternion rott0 = (*pQfunc)(oldrotspeed, RotSpeed, deltaseconds)*rottm1;  //całkowanie właściwe
		core::vector3df rotation;
		rott0.toEuler(rotation);  //wyciągniecie kątów Eulera z quaterniona, w radianach
		rotation *= 180 / PI;  //przeliczenie na stopnie
							   //setRotation(rott0.getMatrix().getRotationDegrees());
		setRotation(rotation);
		//    if (!(cntr%20))
		//    {
		//        core::vector3df rotdiag=rott0.getMatrix().getRotationDegrees();
		//        cout<<"getrotationdegrees="<<rotdiag<<endl;
		//    }
		//    ++cntr;
		//setPosition(getPosition()+Speed*deltaseconds);  całkowanie t0
		return getPosition();
	}
protected:
	void Reaction(float deltaseconds, core::vector3df& forcetotal, core::vector3df& momenttotal, const core::vector3df& wind)
	{
        static int cntr{};  //tylko do sprawdzania!!!
		/*if (!(cntr % 20))
		   {
		        cout<<"brake right= "<<fBrakeRightSignal<<endl;
				cout << "brake left= " << fBrakeLeftSignal << endl << endl;
		   }*/
		int alt = int(getPosition().Y);
		static int lastalt = alt;
		static float ro = fRozero*pow((fTzero - 0.0065f*alt) / fTzero, 4.2561f);  //obliczenie gęstości powietrza
		if (abs(alt - lastalt)>100)  //obliczamy ponownie gestosc, jesli jest odpowiednia roznica wysokosci do poprz. obliczenia
		{
			ro = fRozero*pow((fTzero - 0.0065f*alt) / fTzero, 4.2561f);
			lastalt = alt;
		}
        core::vector3df uavairspeed = Speed - wind;  //układ globalny
AbsoluteTransformation.inverseRotateVect(uavairspeed);  //obracamy prędkość do układu samolot

for (int i = 0; i < iCompNum; ++i)
{
	switch (pComp[i].eCOMPTYPE)
	{
	case COMPONENTTYPE::FUSELAGE:
	{
		core::vector3df compairspeed = (RotSpeed.crossProduct(pComp[i].Location)) + uavairspeed;  //airspeed at fuselage - aircraft CS
		core::vector3df airspeedcomp(pComp[i].Xvector.dotProduct(compairspeed), pComp[i].Yvector.dotProduct(compairspeed),
			pComp[i].Zvector.dotProduct(compairspeed));  //airspeed at component, component CS
		core::vector3df airspeedcompXY(airspeedcomp.X, airspeedcomp.Y, 0);  //projection of airspeedcomp to XY plane
		float fusaoa = atan(airspeedcompXY.getLength() / airspeedcomp.Z);
		float speedmodule = compairspeed.getLength();
		float sinfusaoa = sin(fusaoa);
		float normalforce = pComp[i].FuselageP.fCcyl*speedmodule*speedmodule*sinfusaoa*sinfusaoa;
		float cosfusaoa = cos(fusaoa);
		float axialforce = pComp[i].FuselageP.fCstream*pow(speedmodule, 1.8)*cosfusaoa*cosfusaoa;
		core::vector3df forcecomp = airspeedcompXY;
		forcecomp.normalize();
		forcecomp.X *= -normalforce; //minus because force is directed opposedly to speed vector
		forcecomp.Y *= -normalforce;
		forcecomp.Z = -axialforce;
		//forcecomp is ready - force vector in comp CS
		core::vector3df compforce = pComp[i].Xvector*forcecomp.X + pComp[i].Yvector*forcecomp.Y + pComp[i].Zvector*forcecomp.Z; //force vector in aircraft CS
		forcetotal += compforce;  //force at component added to total force vector


		core::vector3df forcepoint = pComp[i].Zvector * pComp[i].FuselageP.fLength025*cos(fusaoa); //point of force application
		forcepoint += pComp[i].Location;  //here it is ready in aircraft CS
		//momenttotal += forcepoint.crossProduct(compforce);  //moment generated by component added to total moment vector

//                if (!(cntr%20))
//                {
//                    cout<<"fuselage compforce="<<compforce<<endl;
//                }

		break;
	}
	case COMPONENTTYPE::AIRFOIL:

	{
		core::vector3df compairspeed = (RotSpeed.crossProduct(pComp[i].Location)) + uavairspeed;  //obliczenie predkosci w komponentach - ukl aircraft
		float speed_y = pComp[i].Yvector.dotProduct(compairspeed);  //składowa prędkości na kierunek Y komponentu
		float speed_z = pComp[i].Zvector.dotProduct(compairspeed);  //składowa prędkości na kierunek Z komponentu
		float angleofattack = atan2(-speed_y, speed_z) * 18 / PI;
		//pComp[i].Rotation.inverseRotateVect(compairspeed);  //obrót predkosci do ukladu komponentu
		//float angleofattack=atan2(-compairspeed.Y,compairspeed.Z)*18/PI;  //kat natarcia w [deg/10], minus bo ujemna prędkość Y to dodatni kąt natarcia
		if (angleofattack < 0)
		{
			angleofattack += 36; //przejscie na tylko dodatnie katy
		}
		if (angleofattack == 36) { angleofattack = 0; }  //nie może być kąta 36

														 //interpolacja wektora z charta
		float d = angleofattack - static_cast<float>(int(angleofattack));  //część ułamkowa kąta natarcia
		int thisindex = static_cast<int>(angleofattack);
		int nextindex = thisindex + 1;
		if (nextindex >= 36)
		{
			nextindex = 0;
		}
		core::vector3df nextvector = pComp[i].FoilP.Chart[nextindex];
		core::vector3df chartforce = nextvector.getInterpolated(pComp[i].FoilP.Chart[thisindex], d);  //tutaj narazie tylko Cl,Cd, w układzie przepływu
		float vsquarey = speed_y * speed_y;
		float vsquarez = speed_z * speed_z;
		//                float vsquarey=compairspeed.Y*compairspeed.Y;  //predkosc Y do kwadratu
		//                float vsquarez=compairspeed.Z*compairspeed.Z; //predkosc Z do kwadratu

						//if (!(cntr%20)&&(i==4||i==5))
						//{
						  //cout<<i<<" comp, thisindex="<<thisindex<<" nextindex="<<nextindex<<endl;
						  //cout<<i<<" comp, compairspeed="<<compairspeed<<endl;
						  //cout<<i<<" comp, angleofattack="<<angleofattack<<endl;
						  //cout<<i<<" thisvector="<<pComp[i].FoilP.Chart[thisindex]<<endl;
						  //cout<<i<<" nextvector="<<pComp[i].FoilP.Chart[nextindex]<<endl;
						//}
		chartforce *= (vsquarey + vsquarez);  //mnożymy półprodukt przez prędkość do kwadratu
		core::vector3df dragforce = compairspeed;
		dragforce.normalize();
		dragforce *= chartforce.Z;  //opór w układzie aircraft
		core::vector3df liftforce = compairspeed.crossProduct(pComp[i].Xvector);  //siła nośna jest prostopadła do prędkości i osi X komponentu
		liftforce.normalize();
		liftforce *= chartforce.Y;  //siła nośna w układzie aircraft

		core::vector3df compforce = dragforce + liftforce;  //całkowita siła od komponentu w układzie aircraft

        //calculation of force on the airfoil's control surface
		core::vector3df controlresponse = pComp[i].FoilP.ControlSensit * ControlSignals;
		if ((controlresponse != core::vector3df(0, 0, 0)) || (static_cast<int>(eFLAPSTATE) > 1) && pComp[i].FoilP.fFlapAngle > 0)
			//if given signal corresponds to given sensitivity, or there are lowered flaps
		{
			//składowa siły steru na kierunek Z komponentu:
			float responsesum = controlresponse.X + controlresponse.Y + controlresponse.Z;
			if (responsesum > 1) { responsesum = 1; }  //lets limit it to range -1..1
			else
				if (responsesum < -1) { responsesum = -1; }
			core::vector3df controldrag;
			core::vector3df controllift;
			float foilflapangle = 0;
			if ((static_cast<int>(eFLAPSTATE) > 1) && (pComp[i].FoilP.fFlapAngle > 0))
			{
				foilflapangle = pComp[i].FoilP.fFlapAngle * ((static_cast<float>(eFLAPSTATE)-1) / 3);  //in radians
			}
			float totalangle = 0;
			if (responsesum > 0)
			{
				totalangle = pComp[i].FoilP.fControlAngleUp*responsesum - foilflapangle;
			}
			else
			{
				totalangle = pComp[i].FoilP.fControlAngleDown*responsesum - foilflapangle;
			}
//            if (!(cntr % 20) && (i==4 || i==5))
//               {
//                    cout<<i<<" totalangle= "<<totalangle*180/PI<<" responsesum="<<responsesum<<endl;
//               }

			float costotangle = cos(totalangle);
			if (totalangle > 0)  //control surface deflected up
            {
                controldrag = pComp[i].Zvector * (-sin(totalangle)*costotangle*totalangle);
                controllift = pComp[i].Yvector * -(costotangle*costotangle*totalangle);
            }
                else    //control surface deflected down
            {
                controldrag = pComp[i].Zvector * (-sin(totalangle)*costotangle*totalangle);
                controllift = pComp[i].Yvector * -(costotangle*costotangle*totalangle);
            }

            core::vector3df controlforce = controldrag + controllift;
            controlforce *= vsquarez * pComp[i].FoilP.fControlPower;
			/*if (!(cntr % 20) && (i == 4 || i == 5))
			{
				cout << i << " comp, compforce=" << compforce << endl;
				cout << i << " comp, controlforce=" << controlforce << endl;
			}*/
            compforce += controlforce;
			}


				forcetotal += compforce;  //sumujemy
				momenttotal += pComp[i].Location.crossProduct(compforce);  //dodajemy moment składowy do momentu głównego

//                if (!(cntr%20)&&(i==4||i==5))
//                {
//                    //cout<<i<<" comp, chartforce="<<chartforce<<endl;
//                    cout<<i<<"comp, angleofattack,deg="<<angleofattack*10<<endl;
//                    //cout<<i<<" comp, forcetotal="<<forcetotal<<endl;
//                    //cout<<i<<" comp, controlresponse="<<controlresponse<<endl;
//                    cout<<i<<" compforce="<<compforce<<endl;
//                }


				break;
			}
			case COMPONENTTYPE::ENGINE:
			{
				core::vector3df compforce(0, 0, iPower / 100.0f*pComp[i].EngineP.fMaxThrust);  //siła ciągu w układzie komponentu
				pComp[i].Rotation.rotateVect(compforce); //teraz w układzie samolot
				forcetotal += compforce;
				momenttotal += pComp[i].Location.crossProduct(compforce);
//                if (!(cntr%20))
//                {
//                cout<<i<<" engine, compforce="<<compforce<<endl;
//                cout<<i<<" comp, forcetotal="<<forcetotal<<endl<<endl;
//                }
				break;
			}
			case COMPONENTTYPE::GEAR:
			{
				if (bGearDown == true)
				{
					static bool touchedearlier = false;
					core::vector3df downpoint = pComp[i].Location;	//in aircraft CS
					core::vector3df uppoint = downpoint + pComp[i].Yvector*pComp[i].GearP.fTravel;  //in aircraft CS
					AbsoluteTransformation.rotateVect(downpoint);  //rotated to global CS
					AbsoluteTransformation.rotateVect(uppoint);  //also rotated to global CS
					downpoint += getPosition();		//global CS
					uppoint += getPosition();		//global CS
					core::line3df geartravel(downpoint, uppoint);
					float fGripCoef;
					float fRollCoef;
					core::vector3df collisionpoint;
					core::triangle3df triangle;
					scene::ISceneNode* touchedterrain = nullptr;
					if ((touchedterrain = pCollMan->getSceneNodeAndCollisionPointFromRay(geartravel, collisionpoint, triangle)) != nullptr)
					{

						for (int j = 0; j < vTerrain.size(); ++j)
						{
							if (static_cast<scene::ISceneNode*>(vTerrain[j]) == touchedterrain)  //if terrain is soft (grass, bare ground)
							{
								if (j<iSoftTerrainNum)
								{
									if (!(cntr % 20))
									{
									}
									fGripCoef = fGripcoefrough;
									fRollCoef = fRollcoefrough;
									pParticSys->getEmitter()->setMaxParticlesPerSecond(MAXPARTICLESRATE * u32(Speed.getLength() * 0.1f));
									//pParticSys->setVisible(true);
									touchedearlier = true;
								}
								else  //terrain is hard (concrete, asphalt)
								{
									/*if (!(cntr % 20))
									{
										cout << "terrain: hard, i = " << i << endl;
									}*/
									fGripCoef = fGripcoefconcrete;
									fRollCoef = fRollcoefconcrete;
									if (touchedearlier == false)
									{
										pParticSys->getEmitter()->setMaxParticlesPerSecond(MAXPARTICLESRATE*10);
										//pParticSys->setVisible(true);
										//cout << "touchdown! Gear: " << i << endl;
										touchedearlier = true;
									}
									else
									{
										pParticSys->getEmitter()->setMaxParticlesPerSecond(0);
										//pParticSys->setVisible(false);
									}
								}
							}

						}
						core::vector3df compairspeed = (RotSpeed.crossProduct(pComp[i].Location)) + uavairspeed;  //airspeed at wheel - aircraft CS

						//calculation of force normal to ground-------------------------------
						core::vector3df trianglenormal = triangle.getNormal();
						trianglenormal.normalize();	//unit vector in global CS
						AbsoluteTransformation.inverseRotateVect(trianglenormal);  //unit vector in aircraft CS

						core::vector3df compforce = trianglenormal;
						float deflection = (downpoint - collisionpoint).getLength();
						float deflecspeed = (deflection - pComp[i].GearP.fDeflection)/deltaseconds;	//(current deflection - past deflection) * deltatime
						//cout << i << " defl_m1:" << pComp[i].GearP.fDeflection << " defl_0:" << deflection << " deltasec:" << deltaseconds << endl;
						pComp[i].GearP.fDeflection = deflection;	//refreshing for a new frame
						compforce *= (deflection * pComp[i].GearP.fSpringCoef)+(deflecspeed*pComp[i].GearP.fDampCoef);	//normal force = spring force + damper force

						if (pComp[i].GearP.bSwivel == true)  //self-aligning wheel
						{
							//calculation of wheel's longitudinal force (from roll friction and braking)
							core::vector3df xdir = compairspeed.crossProduct(trianglenormal);  //vector along aligned's wheel axis, in plane of ground surface
							xdir.normalize();
							core::vector3df rollresist = xdir.crossProduct(trianglenormal);   //vector along wheel's speed, in plane of ground surface
							rollresist.normalize();
							float brakecoef = 0;
							if (pComp[i].GearP.bBraked == 1)
							{
								if ((pComp[i].Location.X > 0) && (fBrakeRightSignal > 0))
								{
									brakecoef = fGripCoef * fBrakeRightSignal;
								}
								if ((pComp[i].Location.X < 0) && (fBrakeLeftSignal > 0))
								{
									brakecoef = fGripCoef * fBrakeLeftSignal;
								}
							}
							rollresist *= compforce.getLength() * (fRollCoef * (1 + compairspeed.getLength() * 0.05)+brakecoef);
							/*if (!(cntr % 20))
							{
								cout << i << " rollresist: " << rollresist << " compairspeed: "<<compairspeed.getLength()<<endl;
								cout << i << " compforce: " << compforce << endl << endl;
							}*/
							compforce += rollresist;

						}
						else   //fixed wheel
						{
							//calculation of wheel side force-------------------------------------
							float sidespeed = pComp[i].Xvector.dotProduct(compairspeed);
							float realgripcoef = (abs(sidespeed) > fGripspeed ? fGripCoef : fGripCoef * (abs(sidespeed) / fGripspeed));		//whether to take full grip coef, or only fraction for low slip speed
							if (sidespeed < 0) { realgripcoef *= -1; }
							core::vector3df sideforce = pComp[i].Zvector.crossProduct(trianglenormal);
							sideforce.normalize();
							sideforce *= realgripcoef * compforce.getLength();
							compforce += sideforce;  //includes horizontal force along wheel's axis

							//calculation of wheel's longitudinal force (from roll friction and braking)
							core::vector3df frontspeed = pComp[i].Zvector * pComp[i].Zvector.dotProduct(compairspeed);
							core::vector3df frontspeednormalized = - frontspeed;
							frontspeednormalized.normalize();
							float brakecoef = 0;
							if (pComp[i].GearP.bBraked == 1)
							{
								if ((pComp[i].Location.X > 0) && (fBrakeRightSignal > 0))
								{
									brakecoef = fGripCoef * fBrakeRightSignal;
								}
								if ((pComp[i].Location.X < 0) && (fBrakeLeftSignal > 0))
								{
									brakecoef = fGripCoef * fBrakeLeftSignal;
								}
							}
							core::vector3df rollresist = frontspeednormalized * compforce.getLength() * (fRollCoef*(1 + frontspeed.getLength() * 0.05)+brakecoef);

							/*if (!(cntr % 20))
							{
								cout << i << " rollresist: " << rollresist << " frontspeed: "<<frontspeed.getLength()<<endl;
								cout << i << " compforce: " << compforce << endl << endl;
							}*/

							compforce += rollresist;
							//front *= compforce.getLength();
						}
						/*if (!(cntr % 20))
						{
							cout << i << " realgripcoefficien: " << realgripcoef << endl;
							cout << i << " Zvector: " << pComp[i].Zvector << endl;
							cout << i << " trianglenormal: " << trianglenormal << endl;
							cout << i << " sideforce: " << sideforce << endl<<endl;
						}*/
						//core::vector3df airspeedcomp(pComp[i].Xvector.dotProduct(compairspeed), pComp[i].Yvector.dotProduct(compairspeed),
							//pComp[i].Zvector.dotProduct(compairspeed));  //airspeed at component, component CS

						
						//cout << i << " damper force: " << (deflecspeed*pComp[i].GearP.fDampCoef) << endl;
						//cout << i << " spring force: " << (deflection * pComp[i].GearP.fSpringCoef) << endl<<endl;
						compforce /= ro; //to avoid influence of air density (later, compforce will be multiplied by ro)
						forcetotal += compforce;
						momenttotal += pComp[i].Location.crossProduct(compforce);
						
					}
					else
					{
						pComp[i].GearP.fDeflection = 0;
						pParticSys->getEmitter()->setMaxParticlesPerSecond(0);
						//pParticSys->setVisible(false);
						touchedearlier = false;
					}
				}

				break;
			}
			}
		}
		forcetotal *= ro;  //tak naprawdę pomnożenie przez ro/2
        momenttotal *= ro;  //analogiczne
        //if (!(cntr%20))
        //{
             //cout<<endl<<"forcetotal="<<forcetotal<<endl;
             //cout<<"momenttotal="<<momenttotal<<endl;
             //core::vector3df jakiswektor=AbsoluteTransformation.getRotationDegrees();
             //cout<<"transformation,deg= "<<jakiswektor<<endl;
             //cout<<"cntr = "<<cntr<<endl<<endl;
        //}

       ++cntr;
	}
};

#endif // UAVNODE_H
