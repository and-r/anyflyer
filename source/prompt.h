#ifndef MENU_H
#define MENU_H
#include <irrlicht.h>
#include <vector>
#include <string>
#include <iostream>  //tymczasowo
using namespace std;  //tymczasowo??
using namespace irr;

//enum class MENU_ANSW{EXIT=0,FLY=1,PLAN=2};
enum class MENU_ANSW{EXITAPP,FLY,PLAN,MAINMENU,CHOOSEUAV,FLIGHTPLAN,KEYDESCRIPTION,ADDUAV,REMOVEUAV,UAVTYPE,
                     SETCOURSE,SETALTITUDE,COURSE,INCREASEALT,DECREASEALT,NOTHING=999};
class MenuList;   //deklaracja zapowiadająca
struct MenuItem
{
    core::stringw sDescription;
    MENU_ANSW eANSWER{};
    int iParameter{};
    core::rect<s32> TextLabelRect{};//rozmiar i położenie prostokąta potrzebnego do wypisania tekstu
    MenuList* pSubmenu=nullptr;
public:
    MenuItem(core::stringw description=L"",MENU_ANSW answer=MENU_ANSW::NOTHING,int parameter=0):sDescription(description),
        eANSWER(answer),iParameter(parameter)
    {}
};

class Prompt
{
protected:
    gui::IGUIFont* pFont;
    vector<MenuItem> vList;
    int iCurrent{};
    video::SColor FontCol;
    void SetTextLabelRect(int index)
    {
       vList[index].TextLabelRect=core::rect<s32>(0,0,pFont->getDimension(vList[index].sDescription.c_str()).Width,
                                            pFont->getDimension(vList[index].sDescription.c_str()).Height);
    }
public:
    bool bVisible = false;
    Prompt(gui::IGUIFont* font,vector<MenuItem> list,int defau=0,
             video::SColor fontcolor=video::SColor(255,255,255,255))
            :pFont(font),vList(list),iCurrent(defau),FontCol(fontcolor)
    {
        //trzeba określić jaki prostokąt zajmuje każdy element tablicy
        if (vList.size()>0)
        {
            for (int i=0;i<vList.size();++i)
            {
                SetTextLabelRect(i);
            }
        }
    }
    virtual ~Prompt()
    {}
    virtual void Draw(core::dimension2d<s32> pos=core::dimension2d<s32>(0,0))
    {
        if (bVisible=true  && vList.size()>0)  //jesli menu nie jest widoczne, komenda Draw zmienia flage widocznosci na true
        {

            core::rect<s32> textlabel(pos.Width,pos.Height,pos.Width+vList[iCurrent].TextLabelRect.getWidth(),
                                          pos.Height+vList[iCurrent].TextLabelRect.getHeight());

            pFont->draw(vList[iCurrent].sDescription ,textlabel,FontCol);
        }
    }
    virtual void Hide()
    {
        bVisible=false;
    }
    virtual void Show()
    {
        bVisible=true;
    }
    virtual void Switch(int index)
    {
        if (vList.size()>0 &&iCurrent>=0)
        {
            if (index >= 0 && index < vList.size())
            {
                iCurrent = index;
            }
        }
    }
    virtual bool MouseClick(core::dimension2di pos,MenuItem& item)
    {
        if (iCurrent >= 0)
        {
            if (vList[iCurrent].TextLabelRect.isPointInside(pos))
            {
                item = ReadCurrent();
                return true;
            }
        }
        return false;
    }
    virtual MenuItem ReadCurrent()
    {
        MenuItem item{};
        item.eANSWER=MENU_ANSW::NOTHING;
        if (iCurrent>=0)
        {
           return vList[iCurrent];
        }
        return item;  //odpowiedź pusta
    }
    virtual void Modify(const MenuItem& item,int index=0)
    {
        if (index>=0 && index < vList.size())
        {
            vList[index] = item;
        }
    }
    virtual int getCurrentIndex()
    {
        return iCurrent;
    }
};

#endif // MENU_H
