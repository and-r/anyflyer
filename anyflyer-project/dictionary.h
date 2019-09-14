#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

class Dictionary
{
private:
    vector<string> vDict;
public:
    Dictionary();
    int ReadTexts(string filename);//metoda wczytująca wszystkie teksty z pliku od pamięci(deklaracja)

    string get(int i)//metoda zwracająca string (implementacja)
    {
        if (i>=0 && i<vDict.size())
        {
            return vDict[i];
        }
        else
        {
            return "#out of text database index#";
        }
    }
};

#endif // DICTIONARY_H
