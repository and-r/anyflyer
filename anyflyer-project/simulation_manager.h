#ifndef SIMMGR_H
#define SIMMGR_H
#include <string>
#include <iostream>
#include <irrlicht.h>
#include <dirent.h>  //needed to list directory content
#include "settingsmgr.h"
#include "dictionary.h"
#include "simeventreceiver.h"
#include "menulist.h"
#include "uavarray.h"
#include "camera.h"
#ifdef _IRR_WINDOWS_
#include <Windows.h>  //potrzebne do funkcji Sleep
#endif
using namespace std;
using namespace irr;
const char SETTINGSFILENAME[]{"settings.txt"};
const char LANGFILENAME[]{"lang-pl.txt"};

// najwazniejsza klasa programu
class SimMgr
{
private:
    //wskazniki
    SettingsMgr* pSett=nullptr;  //wskaznik do obiektu z ustawieniami
    Dictionary* pDict=nullptr;  //wskaznik do obiektu z tekstami w calym programie
    SimEventReceiver* pReceiver=nullptr; //wskaznik do odbiornika zdarzeń (user input)
    MenuList* pSimMenuList=nullptr;
    Prompt* pPrompt=nullptr;
    Prompt* pInfotext=nullptr;
    //MenuList* pListKeyDescription=nullptr;
    UavArray* pUavArr=nullptr;
    Camera* pCamera=nullptr;

    //wskaźniki irrlichtowe
    IrrlichtDevice* pDevice=nullptr;  //wskaznik do podstawowego obiektu silnika irrlicht
    scene::ISceneManager* pScene=nullptr;//na koniec nie trzeba niszczyc
    video::IVideoDriver* pDriver=nullptr;//na koniec nie trzeba niszczyc
    scene::ISceneCollisionManager* pCollMan=nullptr;//na koniec nie trzeba niszczyc
    //scene::IMeshSceneNode* pTerrain=nullptr;//na koniec nie trzeba niszczyc
	vector<scene::IMeshSceneNode*> vTerrain;  //vector of terrain objects, soft at the beginning, then hard
    scene::ILightSceneNode* pLight=nullptr;//na koniec nie trzeba niszczyc
    gui::IGUIFont* pFont0=nullptr;//na koniec nie trzeba niszczyc
    gui::IGUIFont* pFont1=nullptr;//na koniec nie trzeba niszczyc

    //inne zmienne
	wchar_t* cVersion = L"0.3.0";
    bool bPause = false;  //tymczasowo
    bool bHudOn = true;
    bool bControlled = true;  //czy maszyna jest kontrolowana (ta która jest śledzona przez kamerę)
    core::matrix4 CamRotMatrix;
    video::SColor HudColor;
	video::SColor SkyColor;
	int iSoftTerrainNum = 0;
    unsigned uHudScale;
    unsigned uParamRefresh;
    unsigned uFrameDeltaTime;  //parametr obiektu - jest potrzebny w wielu miejscach
    float fRollInput=0;
    float fPitchInput=0;
    float fYawInput=0;
	float fBrakeRightInput = 0;
	float fBrakeLeftInput = 0;
    int iTrimStep = 1;
    int iMouseSpeed;
    int iMouseReturnSpeed;
    bool bImperialUnits;
	int iFrameDelay=10;
    //planowanie lotu
    //vector<int> vUavList;
	vector<string>vUavLst;
    COURSE eCOURSE = COURSE::N;
    int iAltitude = 1000;

    core::dimension2di MenuPos=core::dimension2di(30,60);
    core::dimension2di PromptPos = core::dimension2di(30,10);
    core::dimension2di InfotextPos = core::dimension2di(-400,10);//od prawej krawedzi ekranu!

    int iWsign[14]={-14,0,-6,0,-3,5,0,0,3,5,6,0,14,0};  //punkty znaku "waterline"
    core::vector2di Wsign[7];  //punkty znaku "waterline"
    //metody prywatne
    MENU_ANSW Menu();//kamera krazaca nad terenem, mozna wybierac opcje za pomoca GUI
	void Fly()noexcept;//zawiera pętlę symulacji - żeby przyspieszyć robimy noexcept
    bool Input();//obsluga klawiatury i myszy, true = wyjscie z petli symulacji
    void DrawGui();
    void DrawHud();
    void LoadPlan();
    void ResetPlan();
	void SetTerrain();  //loads meshes and textures for soft and hard terrain, sets sky color
    core::stringw getFlightPlan();
	core::stringw getFlapsAndGearText(UavNode* uav);
public:
static SimMgr* pSimMgr;
    ~SimMgr();//destruktor
    //konstruktor jest domyślny
    void Begin();//tu powstaja obiekty skladowe, do utworzenia IrrlichtDevice
    void BeginScene();//obiekty składowe sceny
    void Run();//metoda cyklicznie wywolujace Menu, Plan i Fly

};

#endif // SIMMGR_H
