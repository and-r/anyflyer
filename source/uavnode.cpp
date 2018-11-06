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



//core::vector3df UavNode::Move(unsigned deltatime, core::vector3df wind)
//{
////    static int cntr=0;//tymczasowo!!!
//
//    float deltaseconds=deltatime/1000.0f;  //czas w sekundach
//
//    //najpierw wartości liniowe
//    core::vector3df oldspeed=Speed;  //zapisanie poprzedniej wartości
//    core::vector3df forcetotal{};  //wektor główny siły, wyzerowany
//    core::vector3df momenttotal{};  //moment główny, wyzerowany
//
//    Reaction(forcetotal,momenttotal,wind);  //otrzymujemy wektor główny siły i mom. główny w układzie drona
////    cout<<"forcetotal,drone cs:"<<forcetotal<<endl;
////    core::matrix4 M=getAbsoluteTransformation();
////    M.rotateVect(forcetotal);
//    AbsoluteTransformation.rotateVect(forcetotal);  //teraz forcetotal jest w układzie globalnym
//    core::vector3df acceltotal=(forcetotal/fMass)+core::vector3df(0,-fGaccel,0);  //suma przyśpieszeń działająca na dron
//    //core::vector3df acceltotal=(core::vector3df(0,0,0)/fMass)+core::vector3df(0,-fGaccel,0);  //suma przyśpieszeń działająca na dron
//
//    Speed+=acceltotal*(deltaseconds);  //scałkowane przyśpieszenie = przyrost prędkości
//
//    //całkowanie prędkości liniowej = położenie
//    setPosition(getPosition()+(*pVfunc)(oldspeed,Speed,deltaseconds));
//
//    //później wartości kątowe
//    core::vector3df oldrotspeed=RotSpeed;  //zapisanie poprzedniej wartości
//
//    //tutaj trzeba obliczyć przyrost prędkości kątowej wynikający z działania  momentu głównego i dodać go do RotSpeed
//    //core::vector3df pewienwektor=(momenttotal/MomInert)*deltaseconds*180/PI;
//    //cout<<"przyrost deg/s="<<pewienwektor<<endl<<endl;
//    RotSpeed+=(momenttotal/MomInert)*deltaseconds;
//    if (RotSpeed.X*oldrotspeed.X<0)
//    {
//        RotSpeed.X=0;
//    }
//    if (RotSpeed.Y*oldrotspeed.Y<0)
//    {
//        RotSpeed.Y=0;
//    }
//    if (RotSpeed.Z*oldrotspeed.Z<0)
//    {
//        RotSpeed.Z=0;
//    }
//
//    //całkowanie prędkości obrotowej = orientacja
//    core::quaternion rottm1(getAbsoluteTransformation());  //najpierw quaternion ze starej orientacji
//    core::quaternion rott0 = (*pQfunc)(oldrotspeed,RotSpeed,deltaseconds)*rottm1;  //całkowanie właściwe
//    core::vector3df rotation;
//    rott0.toEuler(rotation);  //wyciągniecie kątów Eulera z quaterniona, w radianach
//    rotation*=180/PI;  //przeliczenie na stopnie
//    //setRotation(rott0.getMatrix().getRotationDegrees());
//    setRotation(rotation);
////    if (!(cntr%20))
////    {
////        core::vector3df rotdiag=rott0.getMatrix().getRotationDegrees();
////        cout<<"getrotationdegrees="<<rotdiag<<endl;
////    }
////    ++cntr;
//    //setPosition(getPosition()+Speed*deltaseconds);  całkowanie t0
//    return getPosition();
//}
//
//
//
//void UavNode::Reaction(core::vector3df& forcetotal,core::vector3df& momenttotal,const core::vector3df& wind)
//{
//    static int cntr;  //tylko do sprawdzania!!!
//    int alt=int(getPosition().Y);
//    static int lastalt = alt;
//    static float ro = fRozero*pow((fTzero-0.0065f*alt)/fTzero,4.2561f);  //obliczenie gęstości powietrza
//    if (abs(alt-lastalt)>100)  //obliczamy ponownie gestosc, jesli jest odpowiednia roznica wysokosci do poprz. obliczenia
//    {
//        ro = fRozero*pow((fTzero-0.0065f*alt)/fTzero,4.2561f);
//        lastalt=alt;
//    }
//    core::vector3df uavairspeed = Speed-wind;
//    //AbsoluteTransformation.rotateVect(uavairspeed);
//    AbsoluteTransformation.inverseRotateVect(uavairspeed);  //obracamy prędkość do układu drona
////    cout<<"uavairspeed, uav cs= "<<uavairspeed<<endl;
////    core::vector3df innywektor=RotSpeed*180/PI;  //stopnie na sekundę
////    cout<<endl<<"RotSpeed, deg/s = "<<innywektor<<endl;
//    //core::vector3df compairspeed[iCompNum];
//    for (int i=0;i<iCompNum;++i)
//    {
//        switch (pComp[i].eCOMPTYPE)
//        {
//        case COMPONENTTYPE::FUSELAGE:
//            {
//                core::vector3df compairspeed=uavairspeed;
//                pComp[i].Rotation.inverseRotateVect(compairspeed);  //obrót predkosci do ukladu komponentu
//                core::vector3df yforward(compairspeed.X,-compairspeed.Z,-compairspeed.Y);  //wektor compairspeed obrocony osia Y do przodu
//                float fusaoa=90-yforward.getHorizontalAngle().X;  //kąt odchylenia wektora prędkości od osi Z kadłuba w stopniach
//
//                fusaoa*=PI/180;  //teraz w radianach
//                float speedmodule=compairspeed.getLength();
//                float fusdrag = pComp[i].FuselageP.fCstream*pow(speedmodule,1.8)*cos(fusaoa)*cos(fusaoa) +
//                        pComp[i].FuselageP.fCcyl*speedmodule*speedmodule*sin(fusaoa)*sin(fusaoa);
//                core::vector3df compforce=-compairspeed.normalize();  //z minusem bo wektor siły działa w dokładnie odwrotną stronę niż prędkość
//                compforce*=fusdrag;  //wektor siły gotowy - w układzie komponentu
//                pComp[i].Rotation.rotateVect(compforce);  //obrót do układu drona
//                forcetotal+=compforce;  //dodajemy siłe do wektora głównego siły
//
//                //teraz trzeba obliczyć punkt przyłożenia siły
//                core::vector3df dragpoint(0,0,pComp[i].FuselageP.fLength025*cos(fusaoa));  //w układzie komponentu (kadłuba)
//                pComp[i].Rotation.rotateVect(dragpoint);  //teraz w układzie drona
//                dragpoint+=pComp[i].Location;  //dodajemy odsuniecie kadluba od punktu 0,0,0 drona
//                momenttotal+=dragpoint.crossProduct(compforce);  //dodajemy moment od siły do momentu głównego
////                if (!(cntr%20))
////                {
////                    cout<<endl<<i<<" comp, fuselage dragpoint="<<dragpoint<<endl;
////                    cout<<i<<" comp, fuselage compforce="<<compforce<<endl;
////                    cout<<"angleofattack:"<<fusaoa*180/PI<<endl;
////                }
//                break;
//            }
//        case COMPONENTTYPE::AIRFOIL:
//
//            {
//                core::vector3df compairspeed=(RotSpeed.crossProduct(pComp[i].Location))+uavairspeed;  //obliczenie predkosci w komponentach - ukl drona
//                float speed_y= pComp[i].Yvector.dotProduct(compairspeed);  //składowa prędkości na kierunek Y komponentu
//                float speed_z= pComp[i].Zvector.dotProduct(compairspeed);  //składowa prędkości na kierunek Z komponentu
//                float angleofattack=atan2(-speed_y,speed_z)*18/PI;
//                //pComp[i].Rotation.inverseRotateVect(compairspeed);  //obrót predkosci do ukladu komponentu
//                //float angleofattack=atan2(-compairspeed.Y,compairspeed.Z)*18/PI;  //kat natarcia w [deg/10], minus bo ujemna prędkość Y to dodatni kąt natarcia
//                if (angleofattack < 0)
//                {
//                    angleofattack+=36; //przejscie na tylko dodatnie katy
//                }
//                if (angleofattack==36) {angleofattack=0;}  //nie może być kąta 36
//
//                //interpolacja wektora z charta
//                float d=angleofattack-static_cast<float>(int(angleofattack));  //część ułamkowa kąta natarcia
//                int thisindex=static_cast<int>(angleofattack);
//                int nextindex=thisindex+1;
//                if (nextindex>=36)
//                {
//                    nextindex=0;
//                }
//                core::vector3df nextvector = pComp[i].FoilP.Chart[nextindex];
//                core::vector3df chartforce= nextvector.getInterpolated(pComp[i].FoilP.Chart[thisindex],d);  //tutaj narazie tylko Cl,Cd, w układzie przepływu
//                float vsquarey = speed_y * speed_y;
//                float vsquarez = speed_z * speed_z;
////                float vsquarey=compairspeed.Y*compairspeed.Y;  //predkosc Y do kwadratu
////                float vsquarez=compairspeed.Z*compairspeed.Z; //predkosc Z do kwadratu
//
////                if (!(cntr%20)&&(i==2||i==3))
////                {
////                  cout<<i<<" comp, thisindex="<<thisindex<<" nextindex="<<nextindex<<endl;
////                  cout<<i<<" comp, compairspeed="<<compairspeed<<endl;
////                  cout<<i<<" comp, angleofattack="<<angleofattack<<endl;
////                  cout<<i<<" thisvector="<<pComp[i].FoilP.Chart[thisindex]<<endl;
////                  cout<<i<<" nextvector="<<pComp[i].FoilP.Chart[nextindex]<<endl;
////                }
//                chartforce *= (vsquarey+vsquarez);  //mnożymy półprodukt przez prędkość do kwadratu
//                core::vector3df dragforce = compairspeed;
//                dragforce.normalize();
//                dragforce *= chartforce.Z;  //opór w układzie aircraft
//                core::vector3df liftforce = compairspeed.crossProduct(pComp[i].Xvector);  //siła nośna jest prostopadła do prędkości i osi X komponentu
//                liftforce.normalize();
//                liftforce*=chartforce.Y;  //siła nośna w układzie aircraft
//
//                core::vector3df compforce = dragforce + liftforce;  //całkowita siła od komponentu w układzie aircraft
//                //teraz obliczamy siły od powierzchni sterowych
//                if (pComp[i].FoilP.fAileronPower > 0)  //jeżeli płat ma lotkę
//                {
//                    float ailsignal=(pComp[i].Location.X < 0 ? 1 : -1);
//                    //core::vector3df ailforce(0,cos(pComp[i].FoilP.fAileronAngle*ControlSignals.Z),sin(pComp[i].FoilP.fAileronAngle*ControlSignals.Z));
//                    //składowa siły lotki na kierunek Z komponentu:
//                    core::vector3df aildrag = pComp[i].Zvector * (-sin(pComp[i].FoilP.fAileronAngle*ControlSignals.Z)*ControlSignals.Z);
//                    //składowa siły lotki na kierunek Y komponentu:
//                    core::vector3df aillift = pComp[i].Yvector * (cos(pComp[i].FoilP.fAileronAngle*ControlSignals.Z)*ailsignal*ControlSignals.Z);
//                    core::vector3df ailforce = aildrag + aillift;
//                    ailforce *= vsquarez * pComp[i].FoilP.fAileronPower;
//                    compforce += ailforce;
//                    //ailforce.Y*=ailsignal*ControlSignals.Z;
//                    //ailforce.Z*=-ControlSignals.Z;
//                    //ailforce*=vsquarez*pComp[i].FoilP.fAileronPower;
////                    if (!(cntr%20) && (i==2 || i==3))
////                    {
////                        cout<<i<<" comp, ailforce="<<ailforce<<endl;
//
////                    }
//                    //compforce+=ailforce;
//                }
//                if (pComp[i].FoilP.fElevatorPower > 0)  //jeżeli płat ma ster wysokości
//                {
//                    float elevsignal=(pComp[i].Location.Z < 0 ? -1 : 1);
//                    core::vector3df elevdrag = pComp[i].Zvector * (-sin(pComp[i].FoilP.fElevatorAngle*ControlSignals.X)*ControlSignals.X);
//                    core::vector3df elevlift = pComp[i].Yvector * (cos(pComp[i].FoilP.fElevatorAngle*ControlSignals.X)*elevsignal*ControlSignals.X);
//
//                    core::vector3df elevforce = elevlift + elevdrag;
//                    elevforce *= vsquarez * pComp[i].FoilP.fElevatorPower;
//                    compforce += elevforce;
//
////                    elevforce.Y*=elevsignal*ControlSignals.X;
////                    elevforce.Z*=-ControlSignals.X;
////                    elevforce*=vsquarez*pComp[i].FoilP.fElevatorPower;
//    //                if (!(cntr%20) && (i==2 || i==3))
//    //                {
//    //                    cout<<i<<" comp, elevatorforce="<<elevforce<<endl;
//    //                }
//                    //compforce+=elevforce;
//                }
//                if (pComp[i].FoilP.fRudderPower > 0)  //jeżeli płat ma ster kierunku
//                {
//                    float rudsignal=(pComp[i].Location.Z < 0 ? 1 : -1);
//                    core::vector3df ruddrag = pComp[i].Zvector * (-sin(pComp[i].FoilP.fRudderAngle*ControlSignals.Y)*ControlSignals.Y);
//                    core::vector3df rudlift = pComp[i].Yvector * (cos(pComp[i].FoilP.fRudderAngle*ControlSignals.Y)*rudsignal*ControlSignals.Y);
//
//                    core::vector3df rudforce = rudlift + ruddrag;
//                    rudforce *= vsquarez * pComp[i].FoilP.fRudderPower;
//                    compforce += rudforce;
//
////                    core::vector3df rudforce(0,cos(pComp[i].FoilP.fRudderAngle*ControlSignals.Y),sin(pComp[i].FoilP.fRudderAngle*ControlSignals.Y));
////                    rudforce.Y*=rudsignal*ControlSignals.Y;
////                    rudforce.Z*=-ControlSignals.Y;
////                    rudforce*=vsquarez*pComp[i].FoilP.fRudderPower;
////                    if (!(cntr%20) && (i==2 || i==3))
////                    {
////                        cout<<i<<" comp, rudderrforce="<<rudforce<<endl;
////                    }
//                    //compforce+=rudforce;
//                }
//                //pComp[i].Rotation.rotateVect(compforce);  //obrót siły do układu drona
//
//                forcetotal+=compforce;  //sumujemy
//                momenttotal+=pComp[i].Location.crossProduct(compforce);  //dodajemy moment składowy do momentu głównego
//
////                if (!(cntr%20))
////                {
////                    //if (i==2||i==3)
////                    {
////                        //cout<<"angleofattack,deg="<<angleofattack*10<<endl;
////                        cout<<i<<" comp, compforce="<<' '<<compforce<<endl;
////                    }
////                }
//
//
//                break;
//            }
//        case COMPONENTTYPE::ENGINE:
//            {
//                core::vector3df compforce(0,0,iPower/100.0f*pComp[i].EngineP.fMaxThrust);  //siła ciągu w układzie komponentu
//                pComp[i].Rotation.rotateVect(compforce); //teraz w układzie drona
//                forcetotal+=compforce;
//                momenttotal+=pComp[i].Location.crossProduct(compforce);
////                if (!(cntr%20))
////                {
////                        cout<<i<<" engine, compforce="<<' '<<compforce<<endl;
////                }
//            }
//        }
//    }
//    forcetotal*=ro;  //tak naprawdę pomnożenie przez ro/2
//    momenttotal*=ro;  //analogicznie
//
////    if (!(cntr%20))
////    {
////          cout<<endl;
////        cout<<endl<<"forcetotal="<<forcetotal<<endl;
////       cout<<"momenttotal="<<momenttotal<<endl;
////       core::vector3df jakiswektor=AbsoluteTransformation.getRotationDegrees();
////        cout<<"transformation,deg= "<<jakiswektor<<endl<<endl;
////    }
//    ++cntr;
//}



void UavNode::Crash(core::vector3df collpoint)
{
    Speed=core::vector3df{};
    setPosition(collpoint);
    eSTATE=FLIGHTSTATE::CRASHED;
}
