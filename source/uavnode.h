#ifndef UAVNODE_H
#define UAVNODE_H
#include <irrlicht.h>
#include <cmath>

#include "settingsmgr.h"  //zeby przeciazac operator<< dla wektorów
#include <iostream>//do wykasowania
using namespace std;//do wykasowania

using namespace irr;
constexpr float PI=3.141592654;
enum class FLIGHTSTATE{FLYING=0,LANDED=1,CRASHED=2};
enum class COMPONENTTYPE{FUSELAGE=0,AIRFOIL=1,ENGINE=2};
using VectorIntegralFuncType = core::vector3df (*)(core::vector3df,core::vector3df,float);
using QuaternionIntegralFuncType = core::quaternion (*)(core::vector3df,core::vector3df,float);

struct FoilProps
{
    float fControlPower;
    float fControlAngleUp;  //max angle up of control surface, radians
    float fControlAngleDown;  //max angle down of control surface, radians
    core::vector3df ControlSensit;  //influence of ailerons, elevator and rudder, limit values: -1 to 1
    core::vector3df Chart[36];  //aerodynamic characteristic of this foil
};
struct FuselageProps
{
    float fLength025;
    float fCstream;
    float fCcyl;
};
struct EngineProps
{
    float fMaxThrust;
};

struct Component
{
    COMPONENTTYPE eCOMPTYPE;
    core::vector3df Location;
    core::matrix4 Rotation;
    core::vector3df Xvector;  //wektor jednostkowy kierunku X komponentu, w układzie aircraft
    core::vector3df Yvector;  //analogicznie
    core::vector3df Zvector;
    union
    {
        FoilProps FoilP;
        FuselageProps FuselageP;
        EngineProps EngineP;
    };
    Component(){}  //musi być konstruktor, kompilator nie tworzy konstruktora domyślnego bo w klasie jest unia z structami/klasami i nie wie
    //której klasy/structa konstruktor wywołać
};

class UavArray;  //deklaracja zapowiadająca

class UavNode : public scene::ISceneNode
{
private:
    core::stringw sType;
    video::IVideoDriver* pDriver;
    scene::IMeshSceneNode* pMeshChild=nullptr;
    static VectorIntegralFuncType pVfunc;
    static QuaternionIntegralFuncType pQfunc;
    float fMass=1;  //żeby nie było zero
    core::vector3df Cog{};  //środek masy, w układzie lokalnym drona
    core::vector3df MomInert{};  //momenty bezwładności
    int iCompNum;  //liczba komponentow
    Component* pComp=nullptr;  //wskaźnik do tablicy dynamicznej komponentów
    core::vector3df FPPCamPos{};  //pozycja kamery FPP względem punktu 0 UavNode
    core::vector3df ControlSignals{};  //aktualne wychylenia powierzchni sterowych
    int iPower = 75;  //aktualna moc silnika
    int iPitchTrim = 0;

public:
    UavNode(scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,scene::IMesh* mesh)  //konstruktor - jedyny, argumentowy
        : scene::ISceneNode(parent, mgr, id)
    {
        pDriver=mgr->getVideoDriver();  //od scene managera pobieramy wskaźnik do karty graficznej
        pMeshChild = mgr->addMeshSceneNode(mesh,static_cast<scene::ISceneNode*>(this),id);
        //iMaterialNum=mesh->getMeshBufferCount();
        pMeshChild->setMaterialFlag(video::EMF_LIGHTING, true);  //dynamic light
        pMeshChild->addShadowVolumeSceneNode();  //dzięki temu będzie rzucać cienie
    }
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
    friend class UavArray;
    FLIGHTSTATE eSTATE=FLIGHTSTATE::FLYING;
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
    virtual scene::IMeshSceneNode* getMeshChild()
    {
        return pMeshChild;
    }
    virtual core::vector3df getFPPCamPos()
    {
        return FPPCamPos;
    }
    virtual core::stringw getType()
    {
        return sType;
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
        if (iPower > 100){iPower = 100;}  //górne ograniczenie
    }
    virtual void DecreasePower()
    {
        iPower-=5;
        if (iPower < 0){iPower = 0;}  //dolne ograniczenie
    }
    virtual int getPower()
    {
        return iPower;
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
	core::vector3df Move(unsigned deltatime, core::vector3df wind)
	{
		//    static int cntr=0;//tymczasowo!!!

		float deltaseconds = deltatime / 1000.0f;  //czas w sekundach

												   //najpierw wartości liniowe
		core::vector3df oldspeed = Speed;  //zapisanie poprzedniej wartości
		core::vector3df forcetotal{};  //wektor główny siły, wyzerowany
		core::vector3df momenttotal{};  //moment główny, wyzerowany

		Reaction(forcetotal, momenttotal, wind);  //otrzymujemy wektor główny siły i mom. główny w układzie drona
												  //    cout<<"forcetotal,drone cs:"<<forcetotal<<endl;
												  //    core::matrix4 M=getAbsoluteTransformation();
												  //    M.rotateVect(forcetotal);
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
	void Reaction(core::vector3df& forcetotal, core::vector3df& momenttotal, const core::vector3df& wind)
	{
        //static int cntr{};  //tylko do sprawdzania!!!
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

		for (int i = 0; i<iCompNum; ++i)
		{
			switch (pComp[i].eCOMPTYPE)
			{
			case COMPONENTTYPE::FUSELAGE:
			{
                core::vector3df compairspeed = (RotSpeed.crossProduct(pComp[i].Location)) + uavairspeed;  //obliczenie predkosci w kadlubie - ukl aircraft
                core::vector3df airspeedcomp(pComp[i].Xvector.dotProduct(compairspeed),pComp[i].Yvector.dotProduct(compairspeed),
                                             pComp[i].Zvector.dotProduct(compairspeed));  //airspeed at component, component CS
                core::vector3df airspeedcompXY(airspeedcomp.X,airspeedcomp.Y,0);  //projection of airspeedcomp to XY plane
                float fusaoa = atan(airspeedcompXY.getLength()/airspeedcomp.Z);
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


                core::vector3df forcepoint=pComp[i].Zvector * pComp[i].FuselageP.fLength025*cos(fusaoa); //point of force application
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

//				                if (!(cntr%20)&&(i==4||i==5))
//				                {
//				                  cout<<i<<" comp, thisindex="<<thisindex<<" nextindex="<<nextindex<<endl;
//				                  cout<<i<<" comp, compairspeed="<<compairspeed<<endl;
//				                  cout<<i<<" comp, angleofattack="<<angleofattack<<endl;
//				                  cout<<i<<" thisvector="<<pComp[i].FoilP.Chart[thisindex]<<endl;
//				                  cout<<i<<" nextvector="<<pComp[i].FoilP.Chart[nextindex]<<endl;
//				                }
				chartforce *= (vsquarey + vsquarez);  //mnożymy półprodukt przez prędkość do kwadratu
				core::vector3df dragforce = compairspeed;
				dragforce.normalize();
                dragforce *= chartforce.Z;  //opór w układzie aircraft
				core::vector3df liftforce = compairspeed.crossProduct(pComp[i].Xvector);  //siła nośna jest prostopadła do prędkości i osi X komponentu
				liftforce.normalize();
				liftforce *= chartforce.Y;  //siła nośna w układzie aircraft

				core::vector3df compforce = dragforce + liftforce;  //całkowita siła od komponentu w układzie aircraft
                //teraz obliczamy siły od powierzchni sterowych
                core::vector3df controlresponse = pComp[i].FoilP.ControlSensit * ControlSignals;
                if (controlresponse != core::vector3df(0,0,0)) //if given signal corresponds to given sensitivity
				{
                    //składowa siły steru na kierunek Z komponentu:
                    float responsesum=controlresponse.X+controlresponse.Y+controlresponse.Z;
                    if (responsesum > 1) {responsesum=1;}  //lets limit it to range -1..1
                    else
                        if (responsesum <-1){responsesum=-1;}
                    core::vector3df controldrag;
                    core::vector3df controllift;

                    if (responsesum > 0)
                    {
                        controldrag = pComp[i].Zvector * (-sin(pComp[i].FoilP.fControlAngleUp*responsesum)*responsesum);
                        controllift = pComp[i].Yvector * (cos(pComp[i].FoilP.fControlAngleUp*responsesum)*-responsesum);
                    }
                    else
                    {
                        controldrag = pComp[i].Zvector * (-sin(pComp[i].FoilP.fControlAngleDown*-responsesum)*-responsesum);
                        controllift = pComp[i].Yvector * (cos(pComp[i].FoilP.fControlAngleDown*responsesum)*-responsesum);
                    }

                    core::vector3df controlforce = controldrag + controllift;
                    controlforce *= vsquarez * pComp[i].FoilP.fControlPower;
                    compforce += controlforce;

//                    if (!cntr%20)
//                    {
//                         cout<<i<<" comp, controlresponse="<<controlresponse<<endl;
//                    }
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
				pComp[i].Rotation.rotateVect(compforce); //teraz w układzie drona
				forcetotal += compforce;
				momenttotal += pComp[i].Location.crossProduct(compforce);
//                if (!(cntr%20))
//                {
//                cout<<i<<" engine, compforce="<<compforce<<endl;
//                cout<<i<<" comp, forcetotal="<<forcetotal<<endl<<endl;
//                }
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

       //++cntr;
	}
};

#endif // UAVNODE_H
