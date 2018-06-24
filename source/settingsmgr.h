#ifndef SETTINGSMGR_H
#define SETTINGSMGR_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <irrlicht.h>
#include "secontainer.h"
using namespace std;
using namespace irr;

//najpierw funkcje przeciążające operatory << i >> dla klasy irr::core::vector3d
inline ostream& operator<<(ostream& strwyj, core::vector3df& w)  //inline żeby nie złamać "one definition rule"
{
    strwyj<<w.X<<','<<w.Y<<','<<w.Z;
    return strwyj;
}

inline istream& operator >> (istream& strwej, core::vector3df& w)  //inline żeby nie złamać "one definition rule"
{
    string temp;
    strwej>>temp;
    for (int i=0;i<2;++i)  //dwa separatory
    {
        size_t pos=temp.find(',');
        if (pos<string::npos)
        {
            temp.replace(pos,1,1,' ');
        }
        else
        {
            strwej.setstate(ios::failbit);
            return strwej;
        }
    }

    istringstream isst(temp);
    isst>>w.X;
    if (isst)
    {
        isst>>w.Y;
        if (isst)
        {
            isst>>w.Z;
            if (isst)
            {
                return strwej;
            }
        }
    }
    //jeśli tu sie znajdzie punkt wykonania - cos poszlo nie tak
    strwej.setstate(ios::failbit);
    return strwej;

}


class SettingsMgr
{
private:
    vector<string> vData;
    string sFileName;
public:
    SettingsMgr();
    int ReadSettings(const char* filename);

    template <typename T>
    T getVar (string varname)
    {
        T param;
        //cout<<"liczba elementow:"<<vData.size()<<endl;
        for (int i =0; i<vData.size();++i)
        {

            string varnameinvector(vData[i].data(),1,vData[i].length()-2);
            if (vData[i][0]=='(' && varnameinvector==varname)
            {
                //cout<<"znaleziono "<<varnameinvector<<endl;
                if ((i<vData.size()-1)&&(vData[i+1][0]=='{'))//mozemy przypuszczac, ze w nastepnym elemencie vData znajduje sie szukana wartosc
                {
                    string varinvector(vData[i+1].data(),1,vData[i+1].length()-2);
                    istringstream isstrum(varinvector);
                    //T=
                    isstrum>>param;
                    if (isstrum)
                    {
                        return param;
                    }
                }
            }
        }
        //nie udalo sie znalezc lub zwrocic zmiennej, rzucamy wyjatkiem
        SimExceptContainer capsule;
        capsule.sText="variable:"+varname+" not found in file:"+sFileName;
        throw capsule;
    }
};

#endif // SETTINGSMGR_H
