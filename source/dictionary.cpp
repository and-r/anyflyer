#include "simulation_manager.h"

Dictionary::Dictionary()
{

}
int Dictionary::ReadTexts(string filename)
{
    ifstream ifstrum(filename);
    string text;
    char sign;
    if (!ifstrum)
    {
        cout<<"file: "<<filename<<" cannot be found"<<endl;
        return 1;
    }
    else
    {
        while(1)
        {
            do
            {
                if (!ifstrum.get(sign))
                {
                    ifstrum.close();
                    return 0;
                }
            }while(sign!='#');//petla zagniezdziona usuwa ze strumienia wszystkie znaki do napotkania znaku #, lub do końca pliku

            getline(ifstrum,text,'$');  //getline pobiera wszystkie znaki do napotkania $ (ktory wyrzuca)
            if (ifstrum && text.size()>0)
            {
                while (text.back()=='\n')
                {
                    text.pop_back();//jesli na koncu znajda sie znaki nowej linii, trzeba je usunąć
                }
               vDict.push_back(text);//wynikowy tekst wstawiamy do vectora tekstow
               //cout<<"vDict["<<vDict.size()-1<<"]="<<vDict.back()<<"%"<<endl;
            }
            if (!ifstrum)
            {
                ifstrum.close();
                return 0;//jesli koniec pliku - wyjdz
            }

        }
    }
    ifstrum.close();
    return 0;
}
