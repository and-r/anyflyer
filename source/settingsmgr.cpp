#include "simulation_manager.h"
SettingsMgr::SettingsMgr()
{

}
int SettingsMgr::ReadSettings(const char* filename)
{
    ifstream ifstrum(filename);
    string text;
    if (!ifstrum)
    {
        SimExceptContainer capsule;
        capsule.sText=string("file:")+filename+" not found";
        throw capsule;//plik z ustawieniami nie znaleziony,rzucamy wyjatek
        //cout<<"file: "<<filename<<" cannot be found"<<endl;
        return 1;  //kod błędu
    }
    else
    {
        sFileName = filename;  //zapisujemy żeby w przypadku błędu (brak zmiennej) móc podać plik
        while (1)
        {
            ifstrum>>text;
            if (ifstrum)
            {
                if (text[0]=='(' || text[0]=='{' || text[0]=='[')//jesli strumien cos odczytal i nie jest to komentarz w pliku
                {
                    vData.push_back(text);
                    //cout<<"text="<<text<<endl;
                }
            }
            else
            {
                break;
            }
        }
    }
    ifstrum.close();
    return 0;  //kod prawidłowego wyjścia
}

