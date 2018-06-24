#ifndef CAMERA_H
#define CAMERA_H
#include <irrlicht.h>
#include "settingsmgr.h"
#include "dictionary.h"
#include "uavnode.h"
#include <iostream>  //tymczasowo
#include "settingsmgr.h"  //tymczasowo
using namespace std;  //tymczasowo
enum class CAMERASTATE{FP,TP,STAND};
using namespace irr;

class Camera
{
private:
    scene::ICameraSceneNode* pIrrCamera=nullptr;
    IrrlichtDevice* pDevice=nullptr;
    video::IVideoDriver* pDriver=nullptr;
    SettingsMgr* pSett=nullptr;
    Dictionary* pDict=nullptr;
    UavNode* pSelectedUav=nullptr;
    CAMERASTATE eSTATE=CAMERASTATE::STAND;
    float fAngleStep=0.17453;  //10 stopni w radianach
    float fDist=20;  //początkowo kamera będzie 20m od wybranego drona
    float fBearing=0;  //początkowy kierunek kamery, radiany
    float fElevation=0.17453;  //początkowa elewacja kamery(10stopni), radiany
    unsigned uRefreshParamRate;
    core::dimension2d<u32> Screensize{};
    void UpdateFrustum()
    {
        FarRightUp=pIrrCamera->getViewFrustum()->getFarRightUp();
        core::matrix4 M = pIrrCamera->getViewMatrix();
        M.rotateVect(FarRightUp);
    }

public:
    core::vector3df FarRightUp;
    //scene::SViewFrustum Frustum;
    Camera(IrrlichtDevice* device,SettingsMgr* sett,Dictionary* dict,unsigned refreshparamrate=10)
        :pDevice(device),pSett(sett),pDict(dict),uRefreshParamRate(refreshparamrate)
    {
        pDriver=pDevice->getVideoDriver();
        pIrrCamera=pDevice->getSceneManager()->addCameraSceneNode();  //utworzenie kamery Irrlichtowej
        //trzeba pobrac ustawienie kroku kamery
        fAngleStep=pSett->getVar<int>("cameraanglestep")*PI/180;
        FarRightUp=pIrrCamera->getViewFrustum()->getFarRightUp();
        //getIrrCamera()->setNearValue(0.5f);
    }
    ~Camera()
    {
        if (pIrrCamera)
        {
            pIrrCamera->remove();
        }
    }
    scene::ICameraSceneNode* getIrrCamera()
    {
        return pIrrCamera;
    }
    void Attach(UavNode* node,CAMERASTATE state)
    {
        if (node==nullptr)
        {
            pSelectedUav=node;  //czyli zero
            eSTATE=CAMERASTATE::STAND;  //w takim razie kamera nieruchoma
            return;
        }
        else  //w parametrze mamy adres niezerowy -
        {
            pSelectedUav=node;  //ustawiamy wskaźnik na nowego rodzica
            if (state==CAMERASTATE::FP)
            {
                eSTATE=CAMERASTATE::FP;
                fBearing=0;  //na początku kamera zawsze patrzy wprzód
                fElevation=0;
            }
            else
            {
                eSTATE=CAMERASTATE::TP;
                fElevation=0.17453;  //10 stopni w dół
                //fBearing=pSelectedUav->getRotation().Y*PI/180;
                core::vector3df uavdir(0,0,1);  //wektor pokazujący północ w układzie globalnym
                node->getAbsoluteTransformation().rotateVect(uavdir);  //teraz pokazuje kierunek przed nosem maszyny
                fBearing=atan2(uavdir.X,uavdir.Z);
                pIrrCamera->setUpVector(core::vector3df(0,1,0));  //w tym widoku zawsze niebo u góry, ziemia na dole
            }
        }
    }
    UavNode* GetSelectedUav()
    {
            return pSelectedUav;
    }
    CAMERASTATE GetState()
    {
        return eSTATE;
    }

    core::matrix4 Update()
    {
        static int counter{};
        static bool countfrustum = false;
        if (countfrustum)
        {
            UpdateFrustum();
            countfrustum = false;
        }
        if (!(counter%uRefreshParamRate))
        {
            Screensize = pDriver->getScreenSize();
            //cout<<"screensize.x="<<Screensize.Width<<" screensize.y="<<Screensize.Height<<endl;
            static core::dimension2d<u32> oldscreensize{};

            if (Screensize != oldscreensize)  //trzeba zaktualizować parametry kamery
            {
                pIrrCamera->setAspectRatio(float(Screensize.Width)/Screensize.Height);

                oldscreensize = Screensize;
                countfrustum = true;
            }

        }
        ++counter;
        switch (eSTATE)
        {
        case CAMERASTATE::TP:
        {
            pSelectedUav->updateAbsolutePosition();  //najpierw trzeba zaktualizować położenie rodzica
            //trzeba zmienić położenie kamery
            core::vector3df relativepos(0,0,-fDist);
            core::matrix4 M;
            M.setRotationRadians(core::vector3df(fElevation,fBearing,0));
            M.rotateVect(relativepos);
            pIrrCamera->setPosition(pSelectedUav->getPosition()+relativepos);
            //trzeba zmienić target dla kamery
            pIrrCamera->setTarget(pSelectedUav->getPosition()+core::vector3df(0,2,0));  //dwa metry ponad obiektem-rodzicem
            return M;
        }
        break;
        case CAMERASTATE::FP:
        {
            pSelectedUav->updateAbsolutePosition();  //najpierw trzeba zaktualizować położenie rodzica
            core::vector3df campos=pSelectedUav->getFPPCamPos();
            core::matrix4 M=pSelectedUav->getAbsoluteTransformation();  //obrót drona
            M.rotateVect(campos);
            campos+=pSelectedUav->getPosition();
            pIrrCamera->setPosition(campos);  //ustawiamy kamerę w miejscu rodzica

            //teraz trzeba znalezc polozenie target 
            core::matrix4 camrotation;
            camrotation.setRotationRadians(core::vector3df(fElevation,fBearing,0));  //obrót kamery
            M*=camrotation;  //dodajemy do siebie obroty: drona i kamery
            core::vector3df relativepos(0,0,10);
            M.rotateVect(relativepos);
            pIrrCamera->setTarget(campos+relativepos);
            //na koniec obliczamy i ustawiamy UpVector kamery
            core::vector3df upvec(0,1,0);
            M.rotateVect(upvec);
            pIrrCamera->setUpVector(upvec);
            return M;
        }
        break;
        case CAMERASTATE::STAND:
        {
            return core::IdentityMatrix;
        }
            break;
        }
        return core::IdentityMatrix;
    }
    void TurnRight()
    {
        float newvalue = fBearing+fAngleStep;
        fBearing=(newvalue>2*PI ? newvalue-2*PI : newvalue);  //jeśli wychodzi ponad 360 stopni, odejmij je
    }
    void TurnLeft()
    {
        float newvalue = fBearing-fAngleStep;
        fBearing=(newvalue<0 ? newvalue+2*PI : newvalue);  //jeśli wychodzi ponad 360 stopni, odejmij je
    }
    void TurnUp()
    {
       fElevation -= fAngleStep;
       fElevation = (fElevation<-PI*0.49 ? -PI*0.49 : fElevation);
    }
    void TurnDown()
    {


       fElevation += fAngleStep;
       fElevation = (fElevation>PI*0.49 ? PI*0.49 : fElevation);
    }


//    void Reset()
//    {
//        cout<<"jestesmy w funkcji reset"<<endl;
//        SetSelectedUav(nullptr);  //odłączamy kamere od rodzica
//        fDist=20;
//        fBearing=0;
//        fElevation=0;
//        eSTATE=CAMERASTATE::TPS;
//    }
};

#endif // CAMERA_H
