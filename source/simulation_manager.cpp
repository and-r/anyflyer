#include "simulation_manager.h"
SimMgr* SimMgr::pSimMgr = nullptr;//inicjalizacja zerem zmiennej statycznej

SimMgr::~SimMgr()
{
    delete pSett; //usuwamy settingsmgr
    delete pDict; //usuwamy dictionary
    delete pUavArr;  //usuwamy obiektotablicę dronów
    if (pCamera)
    {
        delete pCamera;  //usuwamy kamerę w sytuacji wyjątku
    }
    if (pSimMenuList)
    {
        delete pSimMenuList;  //usuwamy menu w sytuacji wyjątku
    }
    if (pPrompt)
    {
        delete pPrompt;  //usuwamy obiekt podpowiedzi w sytuacji wyjatku
    }
    if (pInfotext)
    {
        delete pInfotext;  //usuwamy obiekt tekstu informacyjnego w sytuacji wyjatku
    }
    delete pReceiver; //usuwamy SimEventReceiver;
    pDevice->drop(); //usuwamy IrrlichtDevice
    cout<<"Application exits"<<endl;
}
void SimMgr::Begin()
{
    pSett=new SettingsMgr;
    pDict=new Dictionary;
    if (pSett->ReadSettings(SETTINGSFILENAME))
    {

    }

    string lfil=LANGFILENAME;
    lfil=pSett->getVar<string>("langfile");
        //cout<<"parameter langfile not found in settings.txt, assumed: "<<lfil<<endl;
    if (pDict->ReadTexts(lfil))
    {
        SimExceptContainer capsule;
        capsule.sText="file:"+lfil+" not found";
        throw capsule;//plik jezykowy nie znaleziony,rzucamy wyjatek
    }
    //tworzymy SimEventReceiver
    pReceiver = new SimEventReceiver;
    //pobieramy dane do utworzenia irrlichtdevice, jesli nie uda sie z pliku, beda domyslne
    u32 x=1024;
    u32 y=768;
    int drv=5;
    x=pSett->getVar<int>("xresolution");
        //cout<<pDict->get(0)<<x<<endl;
    y=pSett->getVar<int>("yresolution");
        //cout<<pDict->get(1)<<y<<endl;
    drv=pSett->getVar<int>("drivertype");
        //cout<<pDict->get(2)<<endl;
    pDevice=createDevice(static_cast<video::E_DRIVER_TYPE>(drv),core::dimension2d<u32>(x,y),16,false,true,false,pReceiver);//probujemy utworzyc IrrlichtDevice
    if (pDevice==nullptr)
    {
        SimExceptContainer capsule;
        capsule.sText="could not create IrrlichtDevice";
        throw capsule;
    }
	pDevice->setResizable(true);
    //jesli juz mamy device, przypisujemy wskazniki simulation_managera, dotyczace obiektow IrrlichDevice
    pScene=pDevice->getSceneManager();
	pCollMan = pScene->getSceneCollisionManager();
    pUavArr=new UavArray(pDevice,pSett,pDict);
    pDriver=pDevice->getVideoDriver();
}
void SimMgr::BeginScene()
{
    //dodajemy wskazniki do czcionek
    pFont0=pDevice->getGUIEnvironment()->getBuiltInFont();
    pFont1=pDevice->getGUIEnvironment()->getFont(string(pSett->getVar<string>("font1")).data());
    if (!pFont1)//jesli nie znaleziono danej czcionki
    {
        pFont1=pDevice->getGUIEnvironment()->getBuiltInFont();//uzyj czcionki wbudowanej
    }

    //dodajemy teren - pustynię
    string mapname(pSett->getVar<string>("mapname"));
    string terrainmeshpath("terrain/"+mapname+".3ds");
    pTerrain = pScene->addOctreeSceneNode(pScene->getMesh(terrainmeshpath.data()));//dodaje mapę jako node do sceny
    if (!pTerrain)
    {
        SimExceptContainer capsule;
        throw capsule;
    }
    pTerrain->setMaterialFlag(video::EMF_LIGHTING, true);//dynamic light
    //na teren nakladamy teksture
    string terraintexturepath("terrain/"+mapname+".jpg");
    //pTerrain->setMaterialTexture(0,pDriver->getTexture(terraintexturepath.data()));
    for (int i=0;i<pTerrain->getMaterialCount();++i)
    {
        pTerrain->getMaterial(i).setTexture(0,pDriver->getTexture(terraintexturepath.data()));
        pTerrain->getMaterial(i).AmbientColor.setAlpha(255);
        pTerrain->getMaterial(i).AmbientColor.setRed(100);
        pTerrain->getMaterial(i).AmbientColor.setGreen(100);
        pTerrain->getMaterial(i).AmbientColor.setBlue(100);
        pTerrain->getMaterial(i).SpecularColor.setAlpha(255);
        pTerrain->getMaterial(i).SpecularColor.setRed(0);
        pTerrain->getMaterial(i).SpecularColor.setGreen(0);
        pTerrain->getMaterial(i).SpecularColor.setBlue(0);
    }
    //ustawiamy swiatlo ambient
    float col=pSett->getVar<float>("ambientlightlevel");
    pScene->setAmbientLight(video::SColorf(col,col,col,col));
    //ustawiamy kolor cienia
    pScene->setShadowColor(video::SColor(pSett->getVar<unsigned>("shadowintensity"),0,0,0));
	//sky color
	core::vector3df rgb = pSett->getVar<core::vector3df>("skycolor");
	SkyColor = video::SColor(255, u32(rgb.X), u32(rgb.Y), u32(rgb.Z));
    //ustawiamy skale symboli HUD
    uHudScale=pSett->getVar<unsigned>("hudscale");
    for (int i=0;i<7;++i)  //mnożymy współrzędne punktów znaku "waterline" przez skale HUD
    {
       Wsign[i]=core::vector2di(iWsign[i*2]*uHudScale,iWsign[i*2+1]*uHudScale);
    }
    //jak czesto odswiezac parametry
    uParamRefresh = pSett->getVar<unsigned>("paramrefresh");
    //ustawiamy kolor symboli HUD
    HudColor.set(255,pSett->getVar<u32>("hudcolorred"),pSett->getVar<u32>("hudcolorgreen"),pSett->getVar<u32>("hudcolorblue"));
    //jakich jednostek używać do wyświetlania parametrów
    bImperialUnits=pSett->getVar<bool>("imperialunits");
    iMouseSpeed=pSett->getVar<int>("mousespeed");
    iMouseReturnSpeed=pSett->getVar<int>("mousereturnspeed");
    iTrimStep = pSett->getVar<int>("trimstep");
	iFrameDelay = pSett->getVar<int>("minframedelay");
    //dodajemy swiatlo kierunkowe
    core::vector3df lightpos(pSett->getVar<float>("light_x"),pSett->getVar<float>("light_y"),pSett->getVar<float>("light_z"));
    col=pSett->getVar<float>("lightlevel");
    pLight=pScene->addLightSceneNode(0,lightpos,video::SColorf(col,col,col),pSett->getVar<float>("lightradius"));
    if (!pLight)
    {
        SimExceptContainer capsule;
        capsule.sText=pDict->get(3);
        throw capsule;
    }

    video::ITexture*  lightbillboardtexture=pDriver->getTexture("terrain/sun.bmp");
    if (lightbillboardtexture)
    {
        float lpicsize=pSett->getVar<float>("lightpicturesize");
        scene::IBillboardSceneNode* lightbillboard=pScene->addBillboardSceneNode(static_cast<scene::ISceneNode*>(pLight),core::dimension2df(lpicsize,lpicsize));
        lightbillboard->setMaterialFlag(video::EMF_LIGHTING, false);
        lightbillboard->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
        lightbillboard->setMaterialTexture(0,lightbillboardtexture);
    }
    //dodajemy triangle selector
    scene::ITriangleSelector* selector = pScene->createOctreeTriangleSelector(pTerrain->getMesh(),pTerrain,128);
    pTerrain->setTriangleSelector(selector);
    selector->drop();
    //ustawiamy przyspieszenie ziemskie i parametry atmosfery
        UavNode::fGaccel = pSett->getVar<float>("gaccel");
        UavNode::fRozero = pSett->getVar<float>("rozero")*0.5;  //gęstość w programie będzie podzielona na dwa! Zeby pominąć dzielenie w wyliczeniu sił
        UavNode::fTzero = pSett->getVar<float>("tzero");
        UavNode::fNi = pSett->getVar<float>("ni");

		//cout << "przed" << endl;
		//int a;
		//cin >> a;

	//values and coefficient related to landing gear (wheels)
		UavNode::fRollcoefrough = pSett->getVar<float>("rollcoefrough");
		UavNode::fRollcoefconcrete = pSett->getVar<float>("rollcoefconcrete");
		UavNode::fGripcoefrough = pSett->getVar<float>("gripcoefrough");
		UavNode::fGripcoefconcrete = pSett->getVar<float>("gripcoefconcrete");
		UavNode::fGripspeed = pSett->getVar<float>("gripspeed");
		

    //ustawiamy wskaźniki do funkcji całkowania dla UavNode
    switch (pSett->getVar<int>("vintegralfunc"))
    {
        case 0:
            UavNode::setpVfunc(UavNode::IntegralEulerVector);
            break;
        case 1:
            UavNode::setpVfunc(UavNode::IntegralTrapezeVector);
    }
    switch (pSett->getVar<int>("qintegralfunc"))
    {
        case 0:
            UavNode::setpQfunc(UavNode::IntegralEulerQuaternion);
            break;
        case 1:
            UavNode::setpQfunc(UavNode::IntegralTrapezeQuaternion);
    }
}
void SimMgr::Run()
{
    while(1)
    {
        //ładowanie planu lotu(tworzenie tablicy dronów, ustawianie ich pozycji)
        switch (Menu())
        {
        case MENU_ANSW::EXITAPP:
            return;
            break;
        case MENU_ANSW::FLY:
            try
            {
                LoadPlan(0);  //plan lotu zero(domyślny):utworzenie jednego drona kierowanego przez usera, ustawienia domyślne
                //oprócz tego przygotowanie obiektów na 1 symulację: kamery i obiektów GUI
                Fly();  //metoda zawierająca główną pętlę symulacji, noexcept!
                ResetPlan();  //usunięcie dronów i obiektów tworzonych na czas 1 symulacji i 1 planu
            }
            catch (SimExceptContainer capsule)
            {
                cout<<capsule.sText<<endl;
            }
            break;


        }
    }
}

MENU_ANSW SimMgr::Menu()
{
    MenuItem functionanswer{};  //tu znajdzie sie odpowiedz funkcji
    //najpierw tworzymy kamere
    core::aabbox3d<f32> terrainbox=pTerrain->getBoundingBox();
    core::vector3df camerapos=terrainbox.getCenter()+core::vector3df(0,(terrainbox.getExtent().getLength()/2),(-terrainbox.getExtent().getLength()/4));
    scene::ICameraSceneNode* camera = pScene->addCameraSceneNode(0,camerapos,terrainbox.getCenter());//trzeba na koniec zniszczyć
    pScene->setActiveCamera(camera);
    camera->setFarValue(terrainbox.getExtent().getLength());
    core::vector3df center=terrainbox.getCenter()+core::vector3df(0,(terrainbox.getExtent().getLength()/4),0);
    //scene::ICameraSceneNode* camera = pScene->addCameraSceneNodeFPS();
    scene::ISceneNodeAnimator* anim = pScene->createFlyCircleAnimator(center,(terrainbox.getExtent().getLength()/4),0.0001f,core::vector3df(0,1,0));//tworzymy animatora
    //scene::ISceneNodeAnimator* anim = pScene->createFlyCircleAnimator(center,5000.0f,0.001f,core::vector3df(1,0,0));//tworzymy animatora
	//let's set window caption: app name and version
#ifndef _DEBUG
	wstring caption = L"AnyFlyer ";
	caption += wstring(cVersion);
	caption += L" - main menu";
	pDevice->setWindowCaption(caption.c_str());
#endif
	//----------------------------------------------
    if (anim)
    {
        camera->addAnimator(anim);
        anim->drop();
    }

    //tworzymy liste menu
    vector<MenuItem> lista;

    MenuItem item{};

    item.sDescription = pDict->get(5).c_str();  //"wyjscie"
    item.eANSWER = MENU_ANSW::EXITAPP;
    lista.push_back(item);

    item.sDescription = pDict->get(6).c_str();  //"zaplanuj lot"
    item.eANSWER = MENU_ANSW::FLIGHTPLAN;
    lista.push_back(item);

    item.sDescription = pDict->get(7).c_str();  //"lec"
    item.eANSWER = MENU_ANSW::FLY;
    lista.push_back(item);

    MenuList* p_mainmenu=new MenuList(pFont1,lista,2);
    lista.clear();
    //-------------------


    //tworzymy prompt
    MenuItem item2{};

    item2.sDescription = pDict->get(24).c_str();  //"wybierz:"
    item2.eANSWER=MENU_ANSW::NOTHING;
    //item2.iParameter = 1;
    lista.push_back(item2);

    item2.sDescription = getFlightPlan();
    item2.eANSWER=MENU_ANSW::NOTHING;
    lista.push_back(item2);

	item2.sDescription = pDict->get(35).c_str();  //"scene preparation"
	item2.eANSWER = MENU_ANSW::NOTHING;
	lista.push_back(item2);

    Prompt* p_prompt=new Prompt(pFont1,lista,0,video::SColor(255,0,0,128));
    lista.clear();
    //---------------

    //tworzymy podmenu "zaplanuj lot"
    MenuItem item3{};

    item3.sDescription = pDict->get(25).c_str();  //"dodaj samolot"
    item3.eANSWER=MENU_ANSW::ADDUAV;
    lista.push_back(item3);

    item3.sDescription = pDict->get(26).c_str();  //"usun samolot"
    item3.eANSWER=MENU_ANSW::REMOVEUAV;
    lista.push_back(item3);

    item3.sDescription = pDict->get(27).c_str();  //"ustaw kurs"
    item3.eANSWER=MENU_ANSW::SETCOURSE;
    lista.push_back(item3);

    item3.sDescription = pDict->get(28).c_str();  //"ustaw pulap"
    item3.eANSWER=MENU_ANSW::SETALTITUDE;
    lista.push_back(item3);

    MenuList* p_flightplan=new MenuList(pFont1,lista);
    p_mainmenu->SetSubmenu(MENU_ANSW::FLIGHTPLAN,p_flightplan);  //podpinamy do glownego

    lista.clear();
    //------------------------------

    //tworzymy podmenu "Dodaj Samolot"
    int typecounter=0;
    MenuItem item4{};

    try
    {
        while (1)
        {
            SettingsMgr sett;
            string uavdatapath("aircraft/u"+to_string(typecounter)+".dat");
            if (sett.ReadSettings(uavdatapath.data()))  //nie znaleziono, wychodzimy z petli
            {
                break;
            }


            item4.eANSWER=MENU_ANSW::UAVTYPE;
            item4.iParameter = typecounter;
            item4.sDescription = sett.getVar<string>("type").c_str();
            lista.push_back(item4);
            ++typecounter;
        }
    }
    catch (SimExceptContainer capsule)
    {
        if (typecounter>0)
            {
            //nic nie rób - po prostu nie ma kolejnego pliku, albo nazwa samolotu w nim jest nieczytelna
            }
        else
            {throw;}  //nie ma żadnego pliku samolotu, trzeba opuścić program
    }
    MenuList* p_uavlist = new MenuList(pFont1,lista);
    p_flightplan->SetSubmenu(MENU_ANSW::ADDUAV,p_uavlist);
    p_uavlist = nullptr;  //mozna wyzerowac


    if ((vUavList.size() == 0)&&(lista.size()>0))  //domyslnie jeden samolot bedzie w liscie na poczatku, o ile chociaz 1 typ jest w katalogu
    {
       vUavList.push_back(0);  //dodajemy pojedynczy samolot typu 0
    }
    lista.clear();
    p_prompt->Modify(MenuItem(getFlightPlan()),1);  //dodany samolot - modyfikujemy opis planu lotu
    //--------------------------------
    //tworzymy podmenu "Ustaw Kurs"
    MenuItem item5{};
    item5.eANSWER = MENU_ANSW::COURSE;
    item5.sDescription=L"N";
    item5.iParameter = static_cast<int>(COURSE::N);
    lista.push_back(item5);

    item5.sDescription=L"NE";
    item5.iParameter = static_cast<int>(COURSE::NE);
    lista.push_back(item5);

    item5.sDescription=L"E";
    item5.iParameter = static_cast<int>(COURSE::E);
    lista.push_back(item5);

    item5.sDescription=L"SE";
    item5.iParameter = static_cast<int>(COURSE::SE);
    lista.push_back(item5);

    item5.sDescription=L"S";
    item5.iParameter = static_cast<int>(COURSE::S);
    lista.push_back(item5);

    item5.sDescription=L"SW";
    item5.iParameter = static_cast<int>(COURSE::SW);
    lista.push_back(item5);

    item5.sDescription=L"W";
    item5.iParameter = static_cast<int>(COURSE::W);
    lista.push_back(item5);

    item5.sDescription=L"NW";
    item5.iParameter = static_cast<int>(COURSE::NW);
    lista.push_back(item5);

    MenuList* p_coursemenu = new MenuList(pFont1,lista);
    p_flightplan->SetSubmenu(MENU_ANSW::SETCOURSE,p_coursemenu);
    p_coursemenu=nullptr;
    lista.clear();
    //-----------------------------

    //tworzymy podmenu "Ustaw Pulap"
    MenuItem item6(pDict->get(32).c_str(),MENU_ANSW::INCREASEALT);  //"Podnies"
    lista.push_back(item6);

    item6=MenuItem(pDict->get(33).c_str(),MENU_ANSW::DECREASEALT);  //"Obniz"
    lista.push_back(item6);
    MenuList* p_altmenu = new MenuList(pFont1,lista);
    p_flightplan->SetSubmenu(MENU_ANSW::SETALTITUDE,p_altmenu);
    p_altmenu=nullptr;
    lista.clear();
    //------------------------------
    while(pDevice->run())  //petla renderingu
    {
        MenuItem answer{};  //tu beda wczytywane odpowiedzi
        answer.eANSWER = MENU_ANSW::NOTHING;  //bedzie ustawione w razie braku odpowiedzi

        if(pReceiver->IsKeyPressed(irr::KEY_DOWN) || pReceiver->IsKeyPressed(irr::KEY_NEXT))
        {
            p_mainmenu->SwitchLower();
        }
        if(pReceiver->IsKeyPressed(irr::KEY_UP) || pReceiver->IsKeyPressed(irr::KEY_PRIOR))
        {
            p_mainmenu->SwitchUpper();
        }
        if (pReceiver->IsKeyPressed(irr::KEY_LEFT))
        {
            p_mainmenu->SwitchLeft();
        }
        if (pReceiver->IsKeyPressed(irr::KEY_RIGHT))
        {
            p_mainmenu->SwitchRight();
        }
        if(pReceiver->IsKeyPressed(irr::KEY_RETURN))
        {
            answer= p_mainmenu->ReadCurrent();
        }
        if (pReceiver->IsMouseLeftPressed())  //.. na klikniecie myszka
        {
            core::dimension2di clickedpos=pDevice->getCursorControl()->getPosition();
            clickedpos-=MenuPos;
            p_mainmenu->MouseClick(clickedpos,answer);
        }

        //analizujemy odpowiedz p_mainmenu
        switch (answer.eANSWER)
        {
        case MENU_ANSW::UAVTYPE:
            vUavList.push_back(answer.iParameter);
            p_prompt->Modify(MenuItem(getFlightPlan()),1);
            break;
        case MENU_ANSW::REMOVEUAV:
            vUavList.pop_back();
            p_prompt->Modify(MenuItem(getFlightPlan()),1);
            break;
        case MENU_ANSW::COURSE:
            eCOURSE=static_cast<COURSE>(answer.iParameter);
            p_prompt->Modify(MenuItem(getFlightPlan()),1);
            break;
        case MENU_ANSW::INCREASEALT:
            if (iAltitude < 1000)
            {
                iAltitude+=100;
            }
            else
            {
                iAltitude+=500;
            }
            p_prompt->Modify(MenuItem(getFlightPlan()),1);
            break;
        case MENU_ANSW::DECREASEALT:
            if (iAltitude <=1000)
            {
                iAltitude-=100;
            }
            else
            {
                iAltitude-=500;
            }
            p_prompt->Modify(MenuItem(getFlightPlan()),1);
            break;
        }
        //--------------------------------

        //aktualizujemy prompt
        if (p_mainmenu->getCurrentIndex() == 1)  //pozycja "Zaplanuj lot"
        {
            p_prompt->Switch(1);  //wyswietli dane o planie lotu
        }
        else
        {
            p_prompt->Switch(0);  //wyswietli "Wybierz:"x
        }
        //--------------------


		//dwa przypadki powodujace wyjscie z funkcji Menu, czesc dalsza bedzie po wyswietleniu zmian (w prompcie)
		if (answer.eANSWER == MENU_ANSW::EXITAPP)
		{
			functionanswer = answer;
		}
		if (answer.eANSWER == MENU_ANSW::FLY)
		{
			p_prompt->Switch(2);
			functionanswer = answer;
		}
		//-----------------------------------------------

        pDriver->beginScene(true, true, SkyColor);
        pScene->drawAll();
        p_mainmenu->Draw(MenuPos);
        p_prompt->Draw(PromptPos);
        pDriver->endScene();

		if (answer.eANSWER == MENU_ANSW::FLY || answer.eANSWER == MENU_ANSW::EXITAPP)
		{
			break;  //wyjscie z petli a pozniej z funkcji Menu
		}
    }
    camera->remove();
    delete p_mainmenu;
    delete p_prompt;
    return functionanswer.eANSWER;
}
core::stringw SimMgr::getFlightPlan()
{
    string text=pDict->get(29);  //"Wybranych Samolotow:"
    text+=to_string(vUavList.size());
    text+=pDict->get(30);  //"kurs:"
    switch (eCOURSE)
    {
    case COURSE::N:
        text+='N';
        break;
    case COURSE::NE:
        text+="NE";
        break;
    case COURSE::E:
        text+='E';
        break;
    case COURSE::SE:
        text+="SE";
        break;
    case COURSE::S:
        text+='S';
        break;
    case COURSE::SW:
        text+="SW";
        break;
    case COURSE::W:
        text+='W';
        break;
    case COURSE::NW:
        text+="NW";
        break;
    }
    text+=pDict->get(31);  //"pulap"
    text+=to_string(static_cast<int>(iAltitude));
    if (bImperialUnits == true)
    {
        text+=" ft";
    }
    else
    {
        text+= " m";
    }
    return text.c_str();
}

core::stringw SimMgr::getFlapsAndGearText(UavNode* uav)
{
	string outtext;
	int flapstate;
	int gearstate;
	uav->getFlapsAndGear(flapstate, gearstate);
	//cout << "flaps: " << flapstate << "landing gear: " << gearstate << endl;
	if ((flapstate != 0) && (flapstate != 1))
	{
		outtext = pDict->get(37);
	}
	switch (flapstate)
	{
	case -1:
		outtext += pDict->get(38);   //"shifting.."
		break;
	case 2:
		outtext += pDict->get(39);	//"take off"
		break;
	case 3:
		outtext += pDict->get(40);   //"intermediate"
		break;
	case 4:
		outtext += pDict->get(41);  //"landing"
		break;
	}
	if (gearstate == -1)
	{
		if (outtext.empty()==false){ outtext += ", "; }
		outtext += pDict->get(42);  //"Undercarriage:"
		outtext += pDict->get(38);  //"shifting"
	}
	if (gearstate == 1)
	{
		if (outtext.empty() == false) { outtext += ", "; }
		outtext += pDict->get(42);  //"Undercarriage:"
		outtext += pDict->get(43);  //"down"
	}
	return outtext.c_str();
}

void SimMgr::LoadPlan(int choice=0)
{
    for (auto k:vUavList)  //dodajemy maszyny odpowiedniego typu, zapisanego w liscie vUavlist
    {
        if (bImperialUnits)
        {
            pUavArr->Add(k,eCOURSE,static_cast<float>(iAltitude)*0.3048f);
        }
        else
        {
            pUavArr->Add(k,eCOURSE,static_cast<float>(iAltitude));
        }

    }
    //bControlled = false;//tymczasowo!!!

    vector<MenuItem> lista;

    //tworzymy menu poziom pierwszy
    MenuItem item{};

    item.sDescription=pDict->get(13).c_str();  //"wyjdz do glownego menu"
    item.eANSWER=MENU_ANSW::MAINMENU;
    lista.push_back(item);

    item.sDescription=pDict->get(14).c_str();  //"wybierz samolot"
    item.eANSWER=MENU_ANSW::CHOOSEUAV;
    lista.push_back(item);

    item.sDescription=pDict->get(16).c_str();  //"opis klawiatury i sterowania"
    item.eANSWER=MENU_ANSW::KEYDESCRIPTION;
    lista.push_back(item);

    pSimMenuList=new MenuList(pFont1,lista);
    //-----------------------------
    lista.clear();  //opróżniamy listę

    //tworzymy liste KeyDescription - jednoelementową
    MenuItem item2;
    item2.sDescription=pDict->get(19).c_str();
    lista.push_back(item2);
    MenuList* pmenu = new MenuList(pFont1,lista,-2);//-2 czyli nie można operować przyciskami na liście
    //pmenu->bVisible=true;
    if (!pSimMenuList->SetSubmenu(MENU_ANSW::KEYDESCRIPTION,pmenu))
    {
        delete pmenu;// nie udalo sie podczepic do menu wyzszego poziomu, niszczymy to menu
    }
    //-----------------------------
    lista.clear(); //opróżniamy liste

    //tworzymy liste menu dronów
    for (int i=0;i<pUavArr->getNum();++i)
    {
        MenuItem item3;
        item3.sDescription=L"#";
        item3.sDescription+=to_wstring(i).c_str();
        item3.sDescription+=L" (";
        item3.sDescription+=pUavArr->getUav(i)->getType();
        item3.sDescription+=L")";
        item3.iParameter=i;
        item3.eANSWER=MENU_ANSW::CHOOSEUAV;
        lista.push_back(item3);
    }
    pmenu = new MenuList(pFont1,lista);
    pmenu->Switch(lista.size()-1);  //ustawiamy na ostatni element bo zaraz wsiadziemy do ostatniego samolotu
    if (!pSimMenuList->SetSubmenu(MENU_ANSW::CHOOSEUAV,pmenu))
    {
        delete pmenu;// nie udalo sie podczepic do menu wyzszego poziomu, niszczymy to menu
    }
    //----------------------------
    lista.clear(); //opróżniamy listę

    //tworzymy menu prompt
    MenuItem item4{};
    item4.sDescription = pDict->get(22).c_str(); //"MENU(ESC)"
    item4.eANSWER =MENU_ANSW::NOTHING;
    item4.iParameter = 1;
    lista.push_back(item4);

    item4.sDescription = pDict->get(23).c_str();  //"Uzyj klawiszy strzalek i enter"
    item4.eANSWER = MENU_ANSW::NOTHING;
    item4.iParameter = 2;
    lista.push_back(item4);

    pPrompt = new Prompt(pFont1,lista,0,video::SColor(255,0,0,128));
    lista.clear();
    pPrompt->bVisible=true;
    pPrompt->Switch(0);
    //--------------------

    //tworzymy infotext
    MenuItem item5(pDict->get(34).c_str()); //row with "Pause..."
    lista.push_back(item5);
	lista.push_back(MenuItem());  //second, empty row
    pInfotext = new Prompt(pFont1,lista,0,video::SColor(255,0,0,128));
    //-----------------

    //scene::ICameraSceneNode* camera = pScene->addCameraSceneNodeFPS(0,100.0f,0.05f);//tymczasowa kamera, trzeba na koniec zniszczyć
    core::aabbox3d<f32> terrainbox = pTerrain->getBoundingBox();
    //dodajemy kamerę
    pCamera = new Camera(pDevice,pSett,pDict,uParamRefresh);
    pCamera->getIrrCamera()->setFarValue(2*terrainbox.getExtent().getLength());
    pCamera->getIrrCamera()->setNearValue(0.5f);
    pCamera->getIrrCamera()->setPosition(core::vector3df(0,50,-20));
    pScene->setActiveCamera(pCamera->getIrrCamera());
    //---------------
	//let's set window caption with app name and version
#ifndef _DEBUG
	wstring caption = L"AnyFlyer ";
	caption += wstring(cVersion);
	caption += L" - simulation";
	pDevice->setWindowCaption(caption.c_str());
#endif
	//--------------------------------------------------

    pCamera->Attach(pUavArr->getUav(pUavArr->getNum()-1),CAMERASTATE::FP);//wsiadamy do ostatniego samolotu


    bPause=true;     //rozpoczynamy od pauzy
    pInfotext->bVisible = true;


}
void SimMgr::ResetPlan()
{
    delete pCamera;
    pCamera=nullptr;
    delete pSimMenuList;
    pSimMenuList=nullptr;
    delete pPrompt;
    pPrompt = nullptr;
    delete pInfotext;
    pInfotext = nullptr;
    pUavArr->DeleteAll();  //kasujemy drony i ich tablicę
    pDevice->getCursorControl()->setVisible(true);
}

inline bool SimMgr::Input()//obsługa wejścia klawiatury, true - wyjście z pętli symulacji
{
    core::dimension2du screenhalf=pDriver->getScreenSize()/2;  //współrzedne srodka ekranu - przyda sie do 2 rzeczy
    //przełączenie pauza / nie pauza
    if(pReceiver->IsKeyPressed(irr::KEY_SPACE)||pReceiver->IsKeyPressed((irr::KEY_KEY_P)))
    {
        if (bPause)
        {
            bPause=false;
            pDevice->getCursorControl()->setVisible(false);
            pDevice->getCursorControl()->setPosition(s32(screenhalf.Width),s32(screenhalf.Height));
        }
        else
        {
            bPause=true;
            pDevice->getCursorControl()->setVisible(true);
        }
    }
    if (pSimMenuList->bVisible)  //kiedy menu jest widoczne
    {
        if (pReceiver->IsKeyPressed(irr::KEY_ESCAPE))
        {
            pSimMenuList->Hide();
            pPrompt->Switch(0);
        }
        if(pReceiver->IsKeyPressed(irr::KEY_PRIOR)||pReceiver->IsKeyPressed(irr::KEY_UP))
        {
            pSimMenuList->SwitchUpper();
        }
        if(pReceiver->IsKeyPressed(irr::KEY_NEXT)||pReceiver->IsKeyPressed(irr::KEY_DOWN))
        {
            pSimMenuList->SwitchLower();
        }
        if(pReceiver->IsKeyPressed(irr::KEY_RIGHT))
        {
            pSimMenuList->SwitchRight();
        }
        if(pReceiver->IsKeyPressed(irr::KEY_LEFT))
        {
            pSimMenuList->SwitchLeft();
            if (!pSimMenuList->bVisible)
            {
                pPrompt->Switch(0);
            }
        }
        //badamy odpowiedz menu
        MenuItem answer{};
        answer.eANSWER=MENU_ANSW::NOTHING;  //bedzie ustawione w razie braku odpowiedzi
        if (pReceiver->IsMouseLeftPressed())  //.. na klikniecie myszka
        {
            core::dimension2di clickedpos=pDevice->getCursorControl()->getPosition();
            clickedpos-=MenuPos;
            pSimMenuList->MouseClick(clickedpos,answer);
        }

        if(pReceiver->IsKeyPressed(irr::KEY_RETURN))  //.. albo na klawisz enter
        {
           answer=pSimMenuList->ReadCurrent();
        }
        //teraz podejmujemy akcję zależnie od odpowiedzi menu
        switch (answer.eANSWER)
        {
        case MENU_ANSW::MAINMENU:
            //bPause=false;
            //pSimMenuList->Hide();
            return true;
            break;
        case MENU_ANSW::CHOOSEUAV:
            pCamera->Attach(pUavArr->getArray()[answer.iParameter],pCamera->GetState());
            pSimMenuList->Hide();
            pPrompt->Switch(0);         
            break;
        }

    }
    else  //kiedy menu nie jest widoczne
    {
        MenuItem answer{};
        if (pReceiver->IsMouseLeftPressed())  //klikniecie na prompt spowoduje otwarcie menu
        {
            core::dimension2di clickedpos=pDevice->getCursorControl()->getPosition();
            clickedpos-=PromptPos;
            pPrompt->MouseClick(clickedpos,answer);
        }
        if ((pReceiver->IsKeyPressed(irr::KEY_ESCAPE)) || (answer.iParameter > 0))  //..tak samo jak przycisniecie ESC
        {
            pSimMenuList->Show();  //otwieramy menu
            pPrompt->Switch(1);
        }
        if (pReceiver->IsKeyPressed(irr::KEY_LEFT))
        {
            pCamera->TurnLeft();
        }
        if (pReceiver->IsKeyPressed(irr::KEY_RIGHT))
        {
            pCamera->TurnRight();
        }
        if (pReceiver->IsKeyPressed(irr::KEY_UP))
        {
            pCamera->TurnUp();
        }
        if (pReceiver->IsKeyPressed(irr::KEY_DOWN))
        {
            pCamera->TurnDown();
        }
    }
    if (UavNode* uav=pCamera->GetSelectedUav())  //jeśli jest śledzony jakiś uav
    {
        if (pReceiver->IsKeyPressed(irr::KEY_TAB))  //przełączanie trybu kamery
        {
          switch (pCamera->GetState())
          {
          case CAMERASTATE::FP:
            pCamera->Attach(uav,CAMERASTATE::TP);
            break;
          case CAMERASTATE::TP:
            pCamera->Attach(uav,CAMERASTATE::STAND);
            break;
          default:
            pCamera->Attach(uav,CAMERASTATE::FP);
          }

          //if (pCamera->GetState()==CAMERASTATE::FP)
          //{pCamera->Attach(uav,CAMERASTATE::TP);}
          //else
          //{pCamera->Attach(uav,CAMERASTATE::FP);}
        }
        if (pReceiver->IsKeyPressed(irr::KEY_KEY_H)) //włączanie/wyłączanie HUD
        {
            bHudOn=(bHudOn? false:true);
        }
//        if (pReceiver->MouseState.LeftButtonDown)
//        {
//            cout<<"klinieto przycisk myszy"<<endl;
//        }

        if (bPause==false && bControlled == true)  //sterowanie dronem za pomocą myszy i klawiatury
        {
            float wheelvalue=0;
            if (pReceiver->IsWheelMoved(wheelvalue))  //poruszono kółko myszy, zmiana mocy silnika
            {
               if (wheelvalue > 0)
               {
                   if (pReceiver->IsKeyDown(irr::KEY_LCONTROL))  //trym pochylenia w górę
                   {
					   uav->AddPower();
                   }
                   else
                   {
					   uav->AddPitchTrim(-iTrimStep);
                       
                   }
               }
               else
               {
                   if (pReceiver->IsKeyDown(irr::KEY_LCONTROL))  //trym pochylenia w dół
                   {
					   uav->DecreasePower();
                   }
                   else
                   {
					   uav->AddPitchTrim(iTrimStep);
                   }
               }
            }

			if (pReceiver->IsKeyPressed(irr::KEY_KEY_S))
			{
				uav->AddPower();  //drugi sposób zmiany mocy silnika
			}
			if (pReceiver->IsKeyPressed(irr::KEY_KEY_A))
			{
				uav->DecreasePower();  //drugi sposób zmiany mocy silnika
			}
			if (pReceiver->IsKeyPressed(irr::KEY_KEY_D))
			{
				uav->DecreaseFlaps();
			}
			if (pReceiver->IsKeyPressed(irr::KEY_KEY_F))
			{
				uav->IncreaseFlaps();
			}
			if (pReceiver->IsKeyPressed(irr::KEY_KEY_G))
			{
				uav->ChangeGear();
			}
//            static unsigned cntr=0;
            core::position2di currmousepos=pDevice->getCursorControl()->getPosition();
            core::position2di deltamousepos=currmousepos-core::dimension2di(screenhalf);

            // obliczanie parametru sterującego Roll
            fRollInput+=deltamousepos.X*int(uFrameDeltaTime)*iMouseSpeed/5000.0f;  //ruch odchylający drążek od środka
            fRollInput-=fRollInput*int(uFrameDeltaTime)*iMouseReturnSpeed/1000.0f;  //ruch przywracający drążek do środka

            //obliczanie parametru sterującego Pitch
            fPitchInput+=deltamousepos.Y*int(uFrameDeltaTime)*iMouseSpeed/5000.0f;  //ruch odchylający drążek od środka
            fPitchInput-=fPitchInput*int(uFrameDeltaTime)*iMouseReturnSpeed/1000.0f;  //ruch przywracający drążek do środka

//            if (fPitchInput<-1)
//            {
//                fPitchInput=-1;
//            }
//            else
//            {
//                if (fPitchInput>1)
//                {fPitchInput=1;}
//            }
//            if (deltamousepos.X!=0 || deltamousepos.Y!=0)
//            {
//                cout<<"input: X="<<fRollInput<<" Y="<<fPitchInput<<endl;
//            }
//            if (cntr%60 == 0)
//            {
//                cout<<"deltamouse: X="<<deltamousepos.X<<" Y="<<deltamousepos.Y<<endl;
//            }
//            ++cntr;
            pDevice->getCursorControl()->setPosition(s32(screenhalf.Width),s32(screenhalf.Height));

            //obliczanie parametru sterującego Yaw
            if (pReceiver->IsKeyDown(irr::KEY_KEY_Z))
            {
                fYawInput-=int(uFrameDeltaTime)*iMouseSpeed/1200.0f;
            }
            else
            {
                if (pReceiver->IsKeyDown(irr::KEY_KEY_X))
                {
                    fYawInput+=int(uFrameDeltaTime)*iMouseSpeed/1200.0f;
                }
            }
            fYawInput-=fYawInput*int(uFrameDeltaTime)*iMouseReturnSpeed/4000.0f;  //powrót do położenia 0

			//calculation of input signals for wheel brakes
			// substraction from brake signals
			if (fBrakeRightInput > 0)
			{
				fBrakeRightInput -= int(uFrameDeltaTime)*iMouseSpeed / 4000.0f;
			}
			if (fBrakeLeftInput > 0)
			{
				fBrakeLeftInput -= int(uFrameDeltaTime)*iMouseSpeed / 4000.0f;
			}
			//addition to brake signals
			if (pReceiver->IsKeyDown(irr::KEY_KEY_B))  //both brakes symmetrically
			{
				if (fBrakeRightInput > fBrakeLeftInput)
				{
					fBrakeLeftInput += int(uFrameDeltaTime)*iMouseSpeed / 1200.0f;
					fBrakeLeftInput = (fBrakeRightInput < fBrakeLeftInput ? fBrakeRightInput : fBrakeLeftInput);
				}
				else
				{
					if (fBrakeRightInput < fBrakeLeftInput)
					{
						fBrakeRightInput += int(uFrameDeltaTime)*iMouseSpeed / 1200.0f;
						fBrakeRightInput = (fBrakeRightInput > fBrakeLeftInput ? fBrakeLeftInput : fBrakeRightInput);
					}
					else
					{
						fBrakeRightInput += int(uFrameDeltaTime)*iMouseSpeed / 1200.0f;
						fBrakeLeftInput += int(uFrameDeltaTime)*iMouseSpeed / 1200.0f;
					}
				}
			}
			if (pReceiver->IsKeyDown(irr::KEY_KEY_V))  //only right brake
			{
				fBrakeRightInput += int(uFrameDeltaTime)*iMouseSpeed / 1200.0f;
			}
			if (pReceiver->IsKeyDown(irr::KEY_KEY_C))  //only left brake
			{
				fBrakeLeftInput += int(uFrameDeltaTime)*iMouseSpeed / 1200.0f;
			}
			//cut to 1 if any signal exceeds this value
			if (fBrakeRightInput > 1)
			{fBrakeRightInput = 1;}

			if (fBrakeLeftInput > 1)
			{fBrakeLeftInput = 1;}

            //now let's send all the signals to the aircraft
            uav->setControlSignals(core::vector3df((fPitchInput),fYawInput,fRollInput));
			uav->SetOtherSignals(fBrakeRightInput, fBrakeLeftInput);
        }
    }
    //ustawienie sygnałów sterujacych dla maszyn aktualnie nie sterowanych przez użytkownika (tylko trym)
    for (int i=0;i<pUavArr->getNum();++i)
    {
        UavNode* uav = pUavArr->getUav(i);
        if ((uav != pCamera->GetSelectedUav()) || (bControlled == false))  //nie śledzone, albo wszystkie jeśli żaden nie jest kontrolowany
        {
            uav->setControlSignals(core::vector3df(0,0,0));
        }
    }

return false;
}

inline void SimMgr::DrawGui()
{
	static int counter = 0;
    core::dimension2d<u32> screensize=pDriver->getScreenSize();
	pInfotext->bVisible = false;
	static core::stringw flapsandgeartext=L"";
    if (pCamera->GetSelectedUav())
    {
        if (bHudOn && pCamera->GetState()!=CAMERASTATE::STAND)
        {
            DrawHud();
        }
        if (bControlled)  //wyświetl położenie drążka
        {
           core::vector2di stickpixel;
           stickpixel.X=screensize.Width-50;
           stickpixel.Y=screensize.Height-60;
           core::recti stickrect(stickpixel.X-40,stickpixel.Y-40,stickpixel.X+40,stickpixel.Y+40);
           pDriver->draw2DRectangleOutline(stickrect,HudColor);
           core::vector2di stickcurrent=stickpixel;
           stickcurrent.X+=static_cast<int>(40.0f*pCamera->GetSelectedUav()->getControlSignals().Z);
           stickcurrent.Y+=static_cast<int>(40.0f*pCamera->GetSelectedUav()->getControlSignals().X);
           pDriver->draw2DLine(core::vector2di(stickcurrent.X-5,stickcurrent.Y),core::vector2di(stickcurrent.X+5,stickcurrent.Y),HudColor);
           pDriver->draw2DLine(core::vector2di(stickcurrent.X,stickcurrent.Y-5),core::vector2di(stickcurrent.X,stickcurrent.Y+5),HudColor);
           //jak również położenie orczyka
           int ruddercurrent=stickpixel.X+static_cast<int>(40.0f*pCamera->GetSelectedUav()->getControlSignals().Y);
           pDriver->draw2DLine(core::vector2di(ruddercurrent,stickpixel.Y+40),core::vector2di(ruddercurrent,stickpixel.Y+50),HudColor);
           //a teraz pokaż moc na przepustnicy
           static string powertitle = pDict->get(21);  //sięga do słownika tylko na początku
           core::stringw powertext=powertitle.c_str();
           powertext+=to_wstring(pCamera->GetSelectedUav()->getPower()).c_str();
           powertext+=L"%";
           core::rect<s32> powerrect(stickpixel.X-40,stickpixel.Y-70,stickpixel.X+40,stickpixel.Y-50);
           pFont1->draw(powertext,powerrect,HudColor,true,true);
		   //show force applied to brakes - only if there is any
		   if (fBrakeRightInput > 0 || fBrakeLeftInput > 0)
		   {
			   core::stringw braketext = pDict->get(36).c_str();
			   core::rect<s32> brakerect(stickpixel.X - 120, stickpixel.Y - 70, stickpixel.X - 40, stickpixel.Y - 50);
			   pFont1->draw(braketext, brakerect, HudColor, true, true);
			   float brakeright, brakeleft;
			   pCamera->GetSelectedUav()->GetOtherSignals(brakeright, brakeleft);
			   int ibrakeright = int(brakeright * 80);
			   int ibrakeleft = int(brakeleft * 80);
			   pDriver->draw2DLine(core::vector2di(stickpixel.X - 120, stickpixel.Y + 40 - ibrakeleft),
				   core::vector2di(stickpixel.X - 80, stickpixel.Y + 40 - ibrakeleft),HudColor);
			   pDriver->draw2DLine(core::vector2di(stickpixel.X - 80, stickpixel.Y + 40 - ibrakeright),
				   core::vector2di(stickpixel.X - 40, stickpixel.Y + 40 - ibrakeright),HudColor);
		   }
		   //gathering information about flaps and landing gear
		   if (counter%uParamRefresh == 0)
		   {
			   flapsandgeartext = getFlapsAndGearText(pCamera->GetSelectedUav());
			   if (flapsandgeartext.size() > 0)
			   {
				    MenuItem item(flapsandgeartext);
					pInfotext->Modify(item, 1);
					pInfotext->Switch(1);
			   }
		   }
        }
    }
    if (pSimMenuList->bVisible)
    {
        pSimMenuList->Draw(MenuPos);
    }
	if (flapsandgeartext.size()>0)
	{
		pInfotext->bVisible = true;
	}
    if (pPrompt->bVisible)
    {
        pPrompt->Draw(PromptPos);
    }
	if (bPause == true)
	{
		pInfotext->Switch(0);  //switch to text about pause
		pInfotext->bVisible = true;
	}
    if (pInfotext->bVisible)
    {
        pInfotext->Draw(core::dimension2di(screensize.Width+InfotextPos.Width,InfotextPos.Height));
    }
	++counter;
}
inline void SimMgr::DrawHud()
{
    //static int licznik=0;
    core::dimension2d<u32> screensize=pDriver->getScreenSize();
    core::vector2di pixel;
    static core::vector2di velpixel;
    static core::vector2di altpixel;
    static core::vector2di headpixel;
    static unsigned counter = 0;
    static core::stringw airspeedstring;
    static core::stringw altitudestring;
    static core::stringw headingstring;

    //znak "waterline" i przy okazji heading
    core::vector3df nose(0,0,1);
    pCamera->GetSelectedUav()->getAbsoluteTransformation().rotateVect(nose);
    core::vector3df pitchyaw=nose.getHorizontalAngle();
    CamRotMatrix.inverseRotateVect(nose);
    nose.normalize();

    if (nose.Z > 0.0873)  //zeby nie rysował 180 stopni od rzeczywistego położenia
    {
        pixel.X= static_cast<int>((screensize.Width/2)+((nose.X/nose.Z)/(pCamera->FarRightUp.X/pCamera->FarRightUp.Z))*(screensize.Width/2));
        pixel.Y= static_cast<int>((screensize.Height/2)-((nose.Y/nose.Z)/(pCamera->FarRightUp.Y/pCamera->FarRightUp.Z))*(screensize.Height/2));
        if ((pixel.X>0 && pixel.Y>0) && (pixel.X<screensize.Width && pixel.Y<screensize.Height))
        {
            for (int i=0;i<6;++i)
            {
                pDriver->draw2DLine(pixel+Wsign[i],pixel+Wsign[i+1],HudColor);
            }
        }
    }
    //kółko wektora prędkości
    core::vector3df speed=pCamera->GetSelectedUav()->Speed;  //pobieramy wektor prędkości z uava
    CamRotMatrix.inverseRotateVect(speed);
    if (speed.Z>0)  //zeby nie rysował 180 stopni od rzeczywistego położenia
    {
        pixel.X=static_cast<int>((screensize.Width/2)+((speed.X/speed.Z)/(pCamera->FarRightUp.X/pCamera->FarRightUp.Z))*(screensize.Width/2));
        pixel.Y=static_cast<int>((screensize.Height/2)-((speed.Y/speed.Z)/(pCamera->FarRightUp.Y/pCamera->FarRightUp.Z))*(screensize.Height/2));
        if ((pixel.X>0 && pixel.Y>0) && (pixel.X<screensize.Width && pixel.Y<screensize.Height))
        {
            pDriver->draw2DPolygon(pixel,5*uHudScale,HudColor,16);  //rysowanie kółka
        }
    }

    if (counter%uParamRefresh == 0)
    {
        static core::dimension2d<u32> oldscreensize{};
        if (screensize!=oldscreensize)  //jesli tak, to trzeba updatować parametry HUD
        {
            velpixel.X=screensize.Width/2 -(uHudScale*100);
            velpixel.Y=screensize.Height/2;
            altpixel.X=screensize.Width/2 +(uHudScale*100);
            altpixel.Y=screensize.Height/2;
            headpixel.X=screensize.Width/2;
            headpixel.Y=screensize.Height/2-(uHudScale*100);
            oldscreensize=screensize;
        }

        if (bImperialUnits)
        {
           airspeedstring=to_wstring(static_cast<int>(pCamera->GetSelectedUav()->Speed.getLength()*1.9438f)).c_str();  //liczba całkowita, kts
           airspeedstring+=" kts";
           altitudestring=to_wstring(static_cast<int>(pCamera->GetSelectedUav()->getPosition().Y*3.2808f)).c_str();  //stopy
           altitudestring+=" ft";
        }
        else
        {
           airspeedstring=to_wstring(static_cast<int>(pCamera->GetSelectedUav()->Speed.getLength()*3.6)).c_str();  //liczba całkowita, km/h
           airspeedstring+=" km/h";
           altitudestring=to_wstring(static_cast<int>(pCamera->GetSelectedUav()->getPosition().Y)).c_str();  //metry
           altitudestring+=" m";
        }
        //teraz heading
        headingstring=L"H ";
        headingstring+=to_wstring(static_cast<int>(pitchyaw.Y)).c_str();
    }
    ++counter;
    //indicated airspeed
    core::rect<s32> velrect(velpixel.X-40,velpixel.Y-12,velpixel.X+40,velpixel.Y+12);
    pDriver->draw2DRectangleOutline(velrect,HudColor);
    pFont1->draw(airspeedstring,velrect,HudColor,true,true);
    //barometric altitude
    core::recti altrect(altpixel.X-40,altpixel.Y-12,altpixel.X+40,altpixel.Y+12);
    pDriver->draw2DRectangleOutline(altrect,HudColor);
    pFont1->draw(altitudestring,altrect,HudColor,true,true);
    //heading
    core::recti headrect(headpixel.X-30,headpixel.Y-12,headpixel.X+30,headpixel.Y+12);
    pDriver->draw2DRectangleOutline(headrect,HudColor);
    pFont1->draw(headingstring,headrect,HudColor,true,true);
}

void SimMgr::Fly()noexcept  //contains simulation loop - to speed up, it is noexcept
{

    UavNode** uavarray = pUavArr->getArray();
    u32 tzero;
    u32 tminus1=pDevice->getTimer()->getTime();
    u32 tstart=tminus1;
    while(pDevice->run())
    {
        //at first we collect current time and calculate difference to the previous frame
        tzero=pDevice->getTimer()->getTime();
        if (tzero - tminus1 < iFrameDelay)  //difference is lower than acceptable, need to wait
		{
#ifdef _IRR_WINDOWS_
            Sleep(iFrameDelay - (tzero - tminus1));  //WINDOWS, delay function
#endif
#ifndef _IRR_WINDOWS_
            timespec req{},rem{};
            req.tv_nsec=(iFrameDelay-(tzero-tminus1))*1000000;
            nanosleep(&req,&rem);  //Linux, delay function
#endif
			tzero = pDevice->getTimer()->getTime();  //again we take current time
		}
        uFrameDeltaTime=(tzero-tminus1 > 0 ? tzero-tminus1 : 1);  //framedeltatime should be at least 1 ms
		if (uFrameDeltaTime > 300) //protection against too long lags, induced (the most often) by some external processes
		{
			uFrameDeltaTime = 300;
		}
        tminus1=tzero;
        //-------------------------------------------------------------------------

        if (Input())//wyłapywanie wejścia w pauzę(escape) i obsługi kamery
        {
            break;//odebrano sygnał wyjścia z symulacji
        }
        if (!bPause)
        {
            //przemieszczanie obiektów
            for(int i = 0;i<pUavArr->getNum();++i)
            {
                if (uavarray[i]->eSTATE!=FLIGHTSTATE::CRASHED)//przemieszczamy tylko "sprawne" drony
                {
                    uavarray[i]->updateAbsolutePosition();  //być moze mozna to usunąć
                    core::vector3df prevpos(uavarray[i]->getPosition());
					float deltaseconds = uFrameDeltaTime / 1000.0f;  //czas w sekundach
                    core::vector3df currpos(uavarray[i]->Move(deltaseconds,core::vector3df(0,0,0)));
                    //sprawdzamy zderzenie
                    core::line3df way(prevpos,currpos);
                    core::vector3df collisionpoint;
                    core::triangle3df triangle;
                    if (pCollMan->getSceneNodeAndCollisionPointFromRay(way,collisionpoint,triangle)!=nullptr)
                    {
                        //doszlo do zderzenia
                        uavarray[i]->Crash(collisionpoint);
                        cout<<"czas zderzenia:"<<(tzero-tstart)/1000.0f<<endl;
                    }
                }
            }
//            core::vector3df pos=pUavArr->getUav(0)->getPosition();
//            elevation=pos.Y;
//            if (elevation<=0 && done==false)
//            {
//                cout<<"czas uderzenia:"<<(tzero-tstart)/1000.0f<<endl;
//                done=true;
//            }
        }
        CamRotMatrix = pCamera->Update();  //aktualizacja położenia i skierowania kamery
        pDriver->beginScene(true, true, SkyColor);
        pScene->drawAll();
        DrawGui();
        pDriver->endScene();
#ifdef _DEBUG
        int fps = pDriver->getFPS();
		wstring caption = to_wstring(uFrameDeltaTime);
		pDevice->setWindowCaption(caption.c_str());
#endif
        
    }
}
