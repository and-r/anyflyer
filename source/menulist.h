#ifndef MENULIST_H
#define MENULIST_H
#include "prompt.h"


class MenuList:public Prompt
{
private:
    int iMaxWidth{};  //szerokość najszerszego textlabel
    video::SColor colOn;
    int iVertClearance = 10;
    int iHorClearance = 10;
public:
    bool bSubmenu = false;  //czy jest widoczne i aktywne submenu dla pozycji na liście wskazywanej przez iCurrent?
    MenuList(gui::IGUIFont* font,vector<MenuItem> list,int defau=0,
             video::SColor fontcolor=video::SColor(255,255,255,255),video::SColor coloron=video::SColor(255,255,0,0))
        :Prompt(font,list,defau,fontcolor),colOn(coloron)
    {
       //trzeba określić szerokość najszerszego prostokąta
       if (vList.size()>0)
       {
           for (int i=0;i<vList.size();++i)
           {
               if(vList[i].TextLabelRect.getWidth()>iMaxWidth)
               {
                   iMaxWidth=vList[i].TextLabelRect.getWidth();
               }

           }
       }
    }

    ~MenuList()  //destruktor - niszczy podmenu z niższego poziomu
    {
        for (auto k:vList)
        {
            if(k.pSubmenu)
            {
                delete k.pSubmenu;
            }
        }
    }

    void Draw(core::dimension2d<s32> pos=core::dimension2d<s32>(0,0)) override
    {
        if (bVisible=true  && vList.size()>0)  //jesli menu nie jest widoczne, komenda Draw zmienia flage widocznosci na true
        {
            s32 fontheight=pFont->getDimension(L"A").Height;
            //vList[1]=(L"eewwrr");
            for(int i=0;i<vList.size();++i)
            {

                //core::rect<u32> a(pos,pFont->getDimension(vList[i].data()));
                //u32 width = pFont->getDimension(vList[i].data()).Width;
                core::rect<s32> textlabel(pos.Width,pos.Height,pos.Width+vList[i].TextLabelRect.getWidth(),
                                          pos.Height+vList[i].TextLabelRect.getHeight());
                if (i==iCurrent)
                {
                    pFont->draw(vList[i].sDescription ,textlabel,colOn);
                }
                else
                {
                    pFont->draw(vList[i].sDescription,textlabel,FontCol);
                }
                //teraz wyświetlamy submenu
                    if(vList[i].pSubmenu  && vList[i].pSubmenu->bVisible)  //o ile jest podczepione i jest widoczne
                    {
                        vList[i].pSubmenu->Draw(core::dimension2d<s32>(pos.Width+iMaxWidth+iHorClearance,pos.Height));
                    }
                //-------------------------
                pos.Height+=textlabel.getHeight()+iVertClearance;
            }
        }
    }
    void Hide() override
    {
        //najpierw chowamy wszystkie podmenu
        for (auto& k:vList)
        {
            if (k.pSubmenu && k.pSubmenu->bVisible)  //jeśli istnieje submenu dla elementu i jest widoczne
            {
                k.pSubmenu->Hide();
            }
        }
        bVisible=false;  //teraz właściwe menu
    }
    void Show() override
    {
        bVisible=true;
    }
    void SwitchLower()
    {
        if (bVisible && vList.size()>0 &&iCurrent>=0)  //jesli menu jest widoczne, nie jest puste i jest wybrane któreś pole
        {
            if (vList[iCurrent].pSubmenu  && vList[iCurrent].pSubmenu->bVisible)
            {
                vList[iCurrent].pSubmenu->SwitchLower();
            }
            else
            {
                iCurrent=(iCurrent==vList.size()-1? 0 : iCurrent+1);
            }
        }
    }
    void SwitchUpper()
    {
        if (bVisible && vList.size()>0 &&iCurrent>=0)  //jesli menu jest widoczne, nie jest puste i jest wybrane któreś pole
        {
            if (vList[iCurrent].pSubmenu  && vList[iCurrent].pSubmenu->bVisible)
            {
                vList[iCurrent].pSubmenu->SwitchUpper();
            }
            else
            {
                iCurrent=(iCurrent<=0 ? vList.size()-1 : iCurrent-1);
            }
        }
    }
    void SwitchRight()
    {
        if(iCurrent>=0 && vList[iCurrent].pSubmenu)  //jeśli to nie jest "martwa"lista, i jeśli istnieje dla tego elementu zdefiniowane submenu
        {
            if(vList[iCurrent].pSubmenu->bVisible==false)  //i jeśli jeszcze nie jest widoczne
            {
               vList[iCurrent].pSubmenu->Show();  //to staje się widoczne
               //cout<<"powinno sie pojawic submenu, iCurrent="<<iCurrent<<endl;
            }
            else
            {
                vList[iCurrent].pSubmenu->SwitchRight();  //a jeśli już było widoczne, to przekazujemy mu dalej sygnał SwitchRight
            }
        }
    }
    void SwitchLeft()
    {
        if (bVisible && vList.size()>0)
        {
            if(iCurrent>=0 && vList[iCurrent].pSubmenu && vList[iCurrent].pSubmenu->bVisible)
            {
                vList[iCurrent].pSubmenu->SwitchLeft();  //przekazujemy dalej sygnał
            }
            else
            {
                Hide();
            }
        }
    }
    bool MouseClick(core::dimension2di pos,MenuItem& item) override  //pos jest we współrzędnych menu, samo menu nie wie gdzie jest na ekranie!
    {
        if (iCurrent>=0)
        {
            core::dimension2di checkedpos=pos;

            //najpierw sprawdzamy czy myszka trafila w element bierzacego menu
            for (int i=0;i<vList.size();++i)
            {
                if (vList[i].TextLabelRect.isPointInside(checkedpos))
                {
                    for (int j=0;j<vList.size();++j)  //trafiła, zamykamy ewentualnie otwarte submenu innych elementów
                    {
                        if ((i!=j)&&(vList[j].pSubmenu)&&(vList[j].pSubmenu->bVisible))
                        {
                            vList[j].pSubmenu->Hide();
                        }
                    }
                    iCurrent = i;
                    item = ReadCurrent();
                    return true;
                }
                checkedpos.Height-=vList[i].TextLabelRect.getHeight()+iVertClearance;
            }
            //nie trafiła w bierzace menu, to moze w podmenu?
            checkedpos=pos;
            checkedpos.Width-=iMaxWidth+iHorClearance;
            for (int i=0;i<vList.size();++i)
            {
                if ((vList[i].pSubmenu) && (vList[i].pSubmenu->bVisible))
                {
                    if (vList[i].pSubmenu->MouseClick(checkedpos,item))  //przekazujemy klikniecie dalej
                    {
                        return true;
                    }

                }
                checkedpos.Height-=vList[i].TextLabelRect.getHeight()+iVertClearance;
            }
        }
        return false;
    }

    MenuItem ReadCurrent() override
    {
        MenuItem item{};
        item.eANSWER=MENU_ANSW::NOTHING;
        if (iCurrent>=0)
        {
            if (vList[iCurrent].pSubmenu)
            {
                if(vList[iCurrent].pSubmenu->bVisible)
                {
                    return vList[iCurrent].pSubmenu->ReadCurrent();  //przesyłamy zapytanie do submenu
                }
                else
                {
                    vList[iCurrent].pSubmenu->Show();  //otwieramy submenu - dzialanie jak przy SwitchRight()
                    return item;  //odpowiedź pusta
                }
            }
            else
            {
                return vList[iCurrent];  //nie ma submenu, zwracamy odpowiedź elementu z listy
            }
        }
        return item;  //odpowiedź pusta
    }


    bool SetSubmenu(MENU_ANSW ask,MenuList* pointer)
    {

         if (pointer)
         {
             for (auto& k:vList)
             {
                if (ask==k.eANSWER)
                {
                    k.pSubmenu=pointer;
                    return true;  //sukces
                }
             }
         }
        return false;  //niepowodzenie
    }
};

#endif // MENULIST_H
