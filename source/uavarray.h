#ifndef UAVARRAY_H
#define UAVARRAY_H
#include <irrlicht.h>
#include <string>
#include "uavnode.h"
#include "settingsmgr.h"
#include "dictionary.h"
#include <iostream>  //tymczasowo
using namespace std;  //tymczasowo
using namespace irr;
const int CHARTPOINTNUMBER{36};
enum class COURSE{N=0,NE=1,E=2,SE=3,S=4,SW=5,W=6,NW=7};

class UavArray
{
private:
    IrrlichtDevice* pDevice;
    Dictionary* pDict;
    SettingsMgr* pSett;
    int iNum=0;
    UavNode** pArray=nullptr;  //tablica wskaźników do dronów
public:
	vector<scene::IMeshSceneNode*> vTerrain;
	int iSoftTerrainNum = 0;
    UavArray(IrrlichtDevice* device,SettingsMgr* sett,Dictionary* dict):pDevice(device),pSett(sett),pDict(dict)  //konstuktor parametryczny - jedyny
    {

	}
    ~UavArray()
    {
        DeleteAll();
    }

    UavNode* Add(string type,COURSE course = COURSE::N,float alt = 1000.0f);   //dodaje pojedynczy samolot do tablicy
    void LoadData(UavNode*);  //called by Add(..) method
	void AssignTexture(string, scene::IMeshSceneNode*);
    void DeleteAll()
    {
        if (iNum <= 0)
        {
            return;  // nie ma samolotów, nie ma tablicy, nic nie rób
        }
        for(int i=0;i<iNum;++i)
        {
            pArray[i]->removeAll();  //najpierw usuwamy IMeshSceneNode, czyli obiekt-dziecko
            pArray[i]->remove();  //teraz wlaściwy obiekt
            pArray[i]->drop();
        }
        delete [] pArray;
        pArray=nullptr;
        iNum=0;
    }

    int getNum()const
    {
        return iNum;  //zwraca aktualną ilość samolotów
    }

    UavNode* getUav(int index)
    {
        if (index >= 0 && index < iNum)
        {
            return pArray[index];
        }
        else
        {
            return nullptr;
        }
    }
    UavNode** getArray()
    {
        return pArray;
    }
};

#endif // UAVARRAY_H
