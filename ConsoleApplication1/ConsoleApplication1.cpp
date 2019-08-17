// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
// https://devblogs.microsoft.com/oldnewthing/20171002-00/?p=97116
//  https://docs.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-ivirtualdesktopmanager-getwindowdesktopid

#include "pch.h"
#include "json.hpp"
#include <windows.h>
#include <string>
#include <shlobj.h>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>    
using namespace std;
using json = nlohmann::json;


string HWNDToString(HWND input)
{
	string output = "";
	size_t sizeTBuffer = GetWindowTextLength(input) + 1;

	if (sizeTBuffer > 0)
	{
		output.resize(sizeTBuffer);
		sizeTBuffer = GetWindowTextA(input, &output[0], sizeTBuffer);
		output.resize(sizeTBuffer);
	}

	return output;
}
string GetWindowTitle(HWND hwnd)
{	
	return HWNDToString(hwnd);
}
string GetActiveWindowTitle()
{
	HWND hwnd = GetForegroundWindow(); // get handle of currently active window	
	return GetWindowTitle(hwnd);
}
GUID GetWindowDesktop(IVirtualDesktopManager* g_pvdm, HWND hwnd)
{
	GUID ret;
	g_pvdm->GetWindowDesktopId(hwnd,
		&ret);
	return ret;
}
GUID GetActiveWindowDesktop(IVirtualDesktopManager* g_pvdm)
{
	HWND hwnd = GetForegroundWindow(); // get handle of currently active window
	return GetWindowDesktop( g_pvdm, hwnd);
}
string GuidToString(GUID guid)
{
	char guid_cstr[39];
	snprintf(guid_cstr, sizeof(guid_cstr),
		"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return string(guid_cstr);
}
BOOL CALLBACK getWindow(HWND hwnd, LPARAM lParam) 
{
	vector<HWND> &handles = *reinterpret_cast<vector<HWND>*>(lParam); 
	handles.push_back(hwnd);
	return TRUE;
}
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	localtime_s(&tstruct, &now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}
int main()
{
	map<string, vector<string>> data;
	vector<HWND> handles;
	string desktop;
	string title;
	int interval_seconds = 1;
	IVirtualDesktopManager* g_pvdm;


	// create an empty structure (null)
	json toWriteJson;


	if (SUCCEEDED(CoInitialize(NULL))) {/* In case we use COM */

	  // This can fail if the system does not support virtual desktops.
		CoCreateInstance(CLSID_VirtualDesktopManager,
			nullptr, CLSCTX_ALL, IID_PPV_ARGS(&g_pvdm));

		fstream myFile("output.txt", fstream::out | fstream::app);

		std::cout << "Hello World!\n";

		for (int i = 0; i < 100; i++) {			
			Sleep(interval_seconds*1000);
			data.clear();
			handles.clear();			
			BOOL enumeratingWindowsSucceeded = EnumWindows((WNDENUMPROC)getWindow, reinterpret_cast<LPARAM>(&handles));
			// cout << handles.size() << ',' << enumeratingWindowsSucceeded << endl;
			for (auto it = handles.begin(); it != handles.end(); ++it)
			{
				
				desktop = GuidToString(GetWindowDesktop(g_pvdm, *it));
				title = GetWindowTitle(*it);
				//if (data.find(desktop) == data.end())
				//{
				//	// not found
				//	data[desktop].push_back(new vector<string>());
				//}
				data[desktop].push_back(title);
			}
			data.erase("{00000000-0000-0000-0000-000000000000}");
			map<string, vector<string>>::iterator it;
			cout << currentDateTime() << endl;
			toWriteJson.empty();
			for (it = data.begin(); it != data.end(); it++)
			{
				desktop = it->first;
				toWriteJson[desktop] = it->second;
			}
			
			toWriteJson["time"] = currentDateTime();
			toWriteJson["active_desktop"] = GuidToString(GetActiveWindowDesktop(g_pvdm));
			toWriteJson["active_window"] = GetActiveWindowTitle();

			myFile << toWriteJson.dump(-1, ' ', false, json::error_handler_t::ignore) << ',' << endl;
			myFile.flush();
		}
		if (g_pvdm) g_pvdm->Release();

		CoUninitialize();
	}

    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
