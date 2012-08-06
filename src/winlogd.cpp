/*// winlogd.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "winlogd.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINLOGD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINLOGD));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINLOGD));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINLOGD);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

*/



/*
 $Id$

winlogd - Send Windows Event Logs to a syslog server
Copyright (C) 2004 Edoceo, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

// note: see http://www.ietf.org/rfc/rfc3164.txt 
#include "StdAfx.h"

#using <mscorlib.dll>

#using <System.dll>
#using <System.Configuration.Install.dll>
#using <System.ServiceProcess.dll>

using namespace System;
using namespace System::Configuration::Install;
using namespace System::Diagnostics;
using namespace System::Net::Sockets;
using namespace System::ServiceProcess;
using namespace Microsoft::Win32;

#define WINLOGD_APP_NAME "Edoceo, Inc. winlogd v1.5\n  http://www.edoceo.com/creo/winlogd\n"
#define WINLOGD_SERVICE_KEY "SYSTEM\\CurrentControlSet\\Services\\winlogd"
#define WINLOGD_PARAM_KEY "SYSTEM\\CurrentControlSet\\Services\\winlogd\\Parameters"

#define WINLGOD_LOG_CRIT 2
#define WINLGOD_LOG_ERROR 3
#define WINLGOD_LOG_WARNING 4
#define WINLGOD_LOG_NOTICE 5
#define WINLGOD_LOG_INFO 6

#include "winlogdService.h"


// Installer Class
public __gc class winlogi : public Installer
{
public:
  winlogi(void)
  {
    ServiceProcessInstaller* spi = new ServiceProcessInstaller();
    spi->Account = ServiceAccount::LocalSystem;

    ServiceInstaller* si = new ServiceInstaller();
    si->ServiceName = S"winlogd";
    si->StartType = ServiceStartMode::Automatic;

    Installers->Add(spi);
    Installers->Add(si);
  }
};

//install_uninstall - Will add or remove winlogd based on bool uninstall
int install_uninstall(bool uninstall)
{
  Collections::ArrayList* cmdline = new Collections::ArrayList();
  cmdline->Add(String::Format("/assemblypath={0}",System::Reflection::Assembly::GetExecutingAssembly()->Location));
  cmdline->Add(S"/logToConsole=false");
  cmdline->Add(S"/showCallStack");

  InstallContext* ctx = new InstallContext(S"winlogi.log",__try_cast<String* []>( cmdline->ToArray(__typeof(String))));

  TransactedInstaller* ti = new TransactedInstaller();
  ti->Installers->Add(new winlogi());
  ti->Context = ctx;
  //try
  //{
    if (uninstall == false)
    {
      try
      {
        ti->Install(new Collections::Hashtable());
      }
      catch (Exception* e)
      {
        Console::WriteLine(e->InnerException->Message);
        return(1);
      }

  RegistryKey* k = Registry::LocalMachine->OpenSubKey(WINLOGD_SERVICE_KEY,true);
  k->SetValue("Description",S"Sends Windows Event Log data to a syslog server");
  // Operation Parameters
  k->CreateSubKey(S"Parameters");
  k->Close();

  // Do Parameters
  String* s;
  int i;
  // todo: check if it exists first then only change if it doesn't
  k = Registry::LocalMachine->OpenSubKey(WINLOGD_PARAM_KEY,true);
  // Flush
  //i = Convert::ToInt32( k->GetValue("Flush",S"0")->ToString() );
  //if (i==0) k->SetValue("Flush", __box(6000));
  // Monitor
  // k->SetValue("Monitor", __box(6000));
  i = Convert::ToInt32( k->GetValue("Port",S"0")->ToString() );
  if (i==0) k->SetValue("Port", __box(514));
  // Server
  s = k->GetValue("Server",S"default")->ToString();
  if (String::Compare(s,S"default")==0) k->SetValue("Server", S"syslog");
  s = k->GetValue("Facility",S"default")->ToString();
  if (String::Compare(s,S"default")==0) k->SetValue("Facility", S"local4");
  k->Close();

      Console::WriteLine("Installation successful, say `net start winlogd`");
    }
    else
    {
      try
      {
        ti->Uninstall(0);
      }
      catch (Exception* e)
      {
        //e->InnerException->Message
        Console::WriteLine(e->InnerException->Message);
        return(2);
      }
    }
        
  //}
  //catch (Exception* e)
  //{
  //  Console::WriteLine( e->StackTrace );
  //  return 1;
  //}
  return 0;
}

int list_services()
{
  Diagnostics::EventLog* logs[] = EventLog::GetEventLogs();
  Collections::IEnumerator* e = logs->GetEnumerator();
  while (e->MoveNext())
  {
    Diagnostics::EventLog* el = __try_cast<Diagnostics::EventLog*>(e->Current);
    Console::Write(String::Concat(el->Log,":\n  "));
    // Open Logs Registry Key to read sources
    RegistryKey* k = Registry::LocalMachine->OpenSubKey(String::Concat("SYSTEM\\CurrentControlSet\\Services\\EventLog\\",el->Log));
    System::Object* o = k->GetValue("Sources");
    if ( o->GetType()->IsArray )
    {
      String* sources[] = __try_cast<String*[]>(o);
      // NOTE: Version 1.2 added sorting to the output array
      Array::Sort(sources);
      for (int i=0;i<sources->Length;i++)
      {
        Console::Write(sources[i]);
        if (i+1<sources->Length) Console::Write(",");
      }
      Console::Write("\n");
    }
    k->Close();
  }
  return 0;
}

// func: reg_dump_parameters() - dumps the registry parameters to the console
void reg_dump_parameters()
{
  // Registry Operation Parameters
  RegistryKey* k = Registry::LocalMachine->OpenSubKey(WINLOGD_PARAM_KEY);
  Console::WriteLine( String::Concat("Server:   ", k->GetValue("Server",S"syslog")->ToString() ) );
  Console::WriteLine( String::Concat("Port:     ", k->GetValue("Port",S"514")->ToString() ) );
  Console::WriteLine( String::Concat("Facility: ", k->GetValue("Facility",S"local1")->ToString() ) );
  Console::WriteLine( String::Concat("Monitor:  ", k->GetValue("Monitor",S"6000")->ToString() ) );
  Console::WriteLine( String::Concat("Flush:    ", k->GetValue("Flush",S"6000")->ToString() ) );
  k->Close();
}

// func: test_winlogd - Write three messages to the log, then flush
int test_winlogd()
{
  String* src = "winlogd";

  if ( !EventLog::SourceExists(src) ) EventLog::CreateEventSource(src, "Application"); 
  EventLog* el = new EventLog("Application", ".", src);

  el->WriteEntry( S"Application Log Informational Message", EventLogEntryType::Information, 1, 1);
  el->WriteEntry( S"Application Log Warning Message", EventLogEntryType::Warning, 2, 1 );
  el->WriteEntry( S"Application Log Error Message", EventLogEntryType::Error, 3, 1 );
  el->WriteEntry( S"Application Log Success Audit", EventLogEntryType::SuccessAudit, 4, 1 );
  el->WriteEntry( S"Application Log Failure Audit", EventLogEntryType::FailureAudit, 5, 1 );

  return 0;
}

// main
int main(int argc, char* argv[])
{
  if (argc >= 2)
  {
    String* cmd = new String(argv[1]);
    if ( (cmd->Equals(S"-i")) || (cmd->Equals(S"-u")) ) { return install_uninstall(cmd->Equals(S"-u")); }
    else if ( (cmd->Equals(S"-d")) || (cmd->Equals(S"--dump")) ) { return reg_dump_parameters(); }
    else if ( (cmd->Equals(S"-h")) || (cmd->Equals(S"--help")) )
    {
      Console::Write(WINLOGD_APP_NAME);
      Console::Write("Options:\n");
      Console::Write("  --dump\tShow the registry parameters\n");
      Console::Write("  --help\tThis help\n");
      Console::Write("  --list\tList the Event Sources, facilities and levels\n");
      Console::Write("  --test\tRun some internal tests\n");
      Console::Write("  -i\tInstall\n\t-u\tUninstall\n");
    }
    else if ( (cmd->Equals(S"-l")) || (cmd->Equals(S"--list")) ) { return list_services(); }
    else if ( (cmd->Equals(S"-t")) || (cmd->Equals(S"--test")) ) { return test_winlogd(); }
    else if ( (cmd->Equals(S"-V")) || (cmd->Equals(S"--version")) )
    {
      Console::WriteLine(WINLOGD_APP_NAME);
      Console::WriteLine("Released under the terms of the GPL");
      Console::WriteLine("see http://www.edoceo.com/ for more information");
    }
    return(0);
  }
  ServiceBase::Run(new winlogd());
  return 0;
}

