#include "uavarray.h"
#include "secontainer.h"

UavNode* UavArray::Add(int type=0, COURSE course, float alt)
{
    if (pDevice)
    {
        //próbujemy załadować mesh i teksturę z plików
        string dronemeshpath("aircraft/u"+to_string(type)+".3ds");  //budujemy nazwę pliku
        scene::IMesh* mesh=pDevice->getSceneManager()->getMesh(dronemeshpath.data());
        if(!mesh)
        {
            SimExceptContainer capsule;
            //nie ustawiamy żadnego tekstu, bo irrlicht rzuca swój wyjątek i pokazuje opis w konsoli
            throw capsule;  //nie znaleziono mesha -rzucamy wyjątek
        }
        string dronetexturepath("aircraft/u"+to_string(type)+".jpg");  //budujemy nazwę pliku
        video::ITexture* texture=pDevice->getVideoDriver()->getTexture(dronetexturepath.data());
        if (!texture)
        {
            SimExceptContainer capsule;
            //nie ustawiamy żadnego tekstu, bo irrlicht rzuca swój wyjątek i pokazuje opis w konsoli
            throw capsule;
        }
        //wszystko się załadowało

        if (iNum==0)   //możemy utworzyć pierwszego drona
        {
            pArray = new UavNode*[1];
        }
        else  //kolejnego
        {
            UavNode** newarray;  //nowy wskaźnik do wskaźników
            newarray = new UavNode* [iNum+1];  //nowa tablica wskaźników, przypisana tymczasowo do newarray
            for(int i=0;i<iNum;++i)
            {
                newarray[i]=pArray[i]; //kopiowanie wskaźników
            }
            delete pArray;  //usuwamy starą tablicę wskaźników, ale nie obiekty pod tymi wskaźnikami
            pArray=newarray;  //wskaźnik - zmienną obiektową, ustawiamy na nową tablicę
        }
        //teraz możemy utworzyć nowego drona na ostatniej pozycji tablicy
        pArray[iNum] = new UavNode(pDevice->getSceneManager()->getRootSceneNode(),pDevice->getSceneManager(),type,mesh);  //utworzenie pierwszego drona
        for (int i=0;i<pArray[iNum]->getMaterialCount();++i)
        {
                pArray[iNum]->getMeshChild()->getMaterial(i).setTexture(0,texture);
                //pArray[iNum]->getMaterial(i).setTexture(0,texture);//ustawia tą samą teksturę dla każdego mesh buffera
                //pArray[iNum]->getMaterial(i).ColorMaterial=video::ECM_NONE;
                pArray[iNum]->getMaterial(i).AmbientColor.setAlpha(255);
                pArray[iNum]->getMaterial(i).AmbientColor.setRed(255);
                pArray[iNum]->getMaterial(i).AmbientColor.setGreen(255);
                pArray[iNum]->getMaterial(i).AmbientColor.setBlue(255);
        }
        //ustawiamy położenie i kurs, ustawiamy maszyne w formacji z innymi
        core::vector3df position(float(iNum*50),float(alt),float(-iNum*50));
        core::vector3df speed(0,0,1);
        float angle = static_cast<float>(course)*45;  //zamiana enuma na kąt w stopniach
        core::matrix4 M;
        M.setRotationDegrees(core::vector3df(0,angle,0));
        M.rotateVect(position);
        M.rotateVect(speed);
        pArray[iNum]->setPosition(position);
        pArray[iNum]->Speed = speed;
        pArray[iNum]->setRotation(core::vector3df(0,angle,0));
        //--------------------------

        LoadData(pArray[iNum]);  //ładowanie danych i charakterystyk z pliku .dat

//        pArray[iNum]->setPosition(core::vector3df(iNum*5,1043+iNum*5,0));  //m
//        pArray[iNum]->Speed=core::vector3df(0,0,140);  //m/s
//        pArray[iNum]->setRotation(core::vector3df(0,0,0));  //w stopniach
//        pArray[iNum]->RotSpeed=core::vector3df(0,0,0);   //w radianach
        pArray[iNum]->updateAbsolutePosition();
        ++iNum;  //licznik dronów powiększa się o 1
        return pArray[iNum-1];  //metoda zwraca wskaźnik do właśnie utworzonego drona
    }
    return nullptr;
}
void UavArray::LoadData(UavNode* uav)
{
    SettingsMgr sett;
    string dronedatapath("aircraft/u"+to_string(uav->getID())+".dat");
    if (sett.ReadSettings(dronedatapath.data()))
    {

    }

    //przypisywanie właściwości całego drona
    uav->sType = sett.getVar<string>("type").c_str();

    uav->fMass=sett.getVar<float>("mass");
    uav->Cog=sett.getVar<core::vector3df>("cog");
    uav->MomInert=sett.getVar<core::vector3df>("mominert");
    uav->FPPCamPos=sett.getVar<core::vector3df>("fppcampos");
    float controlderiv=sett.getVar<float>("controlderiv");  //zmienna posrednia, zaraz to wykorzystamy do powierzchni sterowych
    float defaultspeed = sett.getVar<float>("defaultspeed"); //służy do modyfikacji wektora prędkości
    uav->Speed*=defaultspeed;

    //przypisywanie właściwości komponentu
    uav->iCompNum=sett.getVar<int>("compnum");
    uav->pComp = new Component[uav->iCompNum];  //utworzenie dynamicznej tablicy komponentów drona
    for(int i=0;i<uav->iCompNum;++i)
    {
        string varname(to_string(i)+"comptype");
        uav->pComp[i].eCOMPTYPE=static_cast<COMPONENTTYPE>(sett.getVar<int>(varname));
        //cout<<i<<", comptype:"<<static_cast<int>(uav->pComp[i].eCOMPTYPE)<<endl;
        varname=to_string(i)+"location";
        uav->pComp[i].Location=sett.getVar<core::vector3df>(varname);
        varname=to_string(i)+"rotation";
        core::vector3df rotation=sett.getVar<core::vector3df>(varname);
        uav->pComp[i].Rotation.setRotationDegrees(rotation);
        uav->pComp[i].Rotation.rotateVect(uav->pComp[i].Xvector,core::vector3df(1,0,0));
        uav->pComp[i].Rotation.rotateVect(uav->pComp[i].Yvector,core::vector3df(0,1,0));
        uav->pComp[i].Rotation.rotateVect(uav->pComp[i].Zvector,core::vector3df(0,0,1));
        switch (uav->pComp[i].eCOMPTYPE)
        {
            case COMPONENTTYPE::FUSELAGE:
            {
                varname=to_string(i)+"length";
                float length = sett.getVar<float>(varname);  //dlugosc kadluba, zmienna posrednia, zaraz to wykorzystamy
                uav->pComp[i].FuselageP.fLength025 = 0.25*length;
                varname = to_string(i)+"diameter";
                float diameter = sett.getVar<float>(varname);  //srednica kadluba, zmienna posrednia
                float s_wet;
                if (length/diameter >= 4.5)
                {
                    s_wet = PI*length*diameter*pow((1-2/(length/diameter)),0.66667)*(1+1/pow(length/diameter,2));  //Torenbeek 1988, dla kadlubów ze wstawką walcową i length/diameter>=4.5
                }
                else
                {
                    s_wet = PI*diameter*diameter*(length/diameter);  //uproszczone pole powierzchni elipsoidy obrotowej
                }
                uav->pComp[i].FuselageP.fCstream = 0.037*s_wet*pow(length/UavNode::fNi,-0.2);
                varname = to_string(i)+"cdcross";
                float cdcross = sett.getVar<float>(varname);
                uav->pComp[i].FuselageP.fCcyl = 0.5*cdcross*s_wet/PI;
                break;
            }
            case COMPONENTTYPE::AIRFOIL:
            {
                varname=to_string(i)+"size";
                float size=sett.getVar<float>(varname);  //powierzchnia płata, zmienna posrednia, zaraz to wykorzystamy
                varname=to_string(i)+"controlsize";
                uav->pComp[i].FoilP.fControlPower=sett.getVar<float>(varname)*size*controlderiv;
                varname=to_string(i)+"controlangleup";
                uav->pComp[i].FoilP.fControlAngleUp=sett.getVar<float>(varname)*PI/180;
                varname=to_string(i)+"controlangledown";
                uav->pComp[i].FoilP.fControlAngleDown=sett.getVar<float>(varname)*PI/180;
                varname=to_string(i)+"controlsensit";
                uav->pComp[i].FoilP.ControlSensit=sett.getVar<core::vector3df>(varname);

                varname=to_string(i)+"chartid";
                int chartid=sett.getVar<int>(varname);
                varname=to_string(i)+"ar";
                float ar=sett.getVar<float>(varname);
                //ładowanie wykresu nr chartid
                core::matrix4 M;
                for(int j=0;j<CHARTPOINTNUMBER;++j)
                {
                    varname=to_string(chartid)+'c'+to_string(j);
                    core::vector3df angledata=sett.getVar<core::vector3df>(varname);  //wektor we współrzędnych przepływu
                    angledata.Y*=(ar/(ar+2));  //uwzględnienie wydłużenia płata na siłe nośną
//                    core::vector3df rotdeg(-float(j*10),0,0);  //kąt obracający, macierz jest lewoskretna
//                    M.setInverseRotationDegrees(rotdeg);  //macierz obracająca
//                    M.rotateVect(angledata);  //obrót do współrzędnych płata
//                    cout<<"rotdeg,lift,drag: "<<rotdeg<<' '<<angledata.Y<<' '<<angledata.Z<<endl;
                    angledata*=size;  //pomnożenie przez powierzchnie płata

                    uav->pComp[i].FoilP.Chart[j]=angledata;
                }
                break;
            }
        case COMPONENTTYPE::ENGINE:
            {
                varname=to_string(i)+"maxthrust";
                float maxthrust = sett.getVar<float>(varname);
                uav->pComp[i].EngineP.fMaxThrust=maxthrust/(1.225/2);  //po pomnożeniu przez ro w funkcji Reaction, wartość będzie ok
            }
        }



    }
}
