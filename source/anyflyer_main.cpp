#include<iostream>
#include "simulation_manager.h"

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
void closeall(int status)
{
    delete SimMgr::pSimMgr;
    SimMgr::pSimMgr=nullptr;
    if (status==0)
        {
            cout<<"EXIT_SUCCESS"<<endl;
#ifdef _IRR_WINDOWS_
			//system("pause");
#endif
            exit(EXIT_SUCCESS);
        }
    else
        {
            cout<<"EXIT_FAILURE"<<endl;
#ifdef _IRR_WINDOWS_
			system("pause");
#endif
            exit(EXIT_FAILURE);
        }
}
int main()
{
    if (SimMgr::pSimMgr==nullptr)
    {
        try
        {
            SimMgr::pSimMgr=new SimMgr;
            SimMgr::pSimMgr->Begin();
            SimMgr::pSimMgr->BeginScene();
        //---------------------tresc programu---------------------------
            SimMgr::pSimMgr->Run();
        //--------------------------------------------------------------
        }
        catch (SimExceptContainer capsule)
        {
            cout<<"Exception thrown:"<<capsule.sText<<endl;
            closeall(1);
        }

        closeall(0);//normalne wyjscie
    }
    return 0;
}

