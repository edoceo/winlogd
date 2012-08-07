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

using namespace System;
using namespace System::Configuration::Install;
using namespace System::Diagnostics;
using namespace System::Net::Sockets;
using namespace System::ServiceProcess;
using namespace Microsoft::Win32;
//using namespace System::IO;

#define WINLOGD_APP_NAME "Edoceo, Inc. winlogd v1.5\n  http://www.edoceo.com/creo/winlogd\n"
#define WINLOGD_SERVICE_KEY "SYSTEM\\CurrentControlSet\\Services\\winlogd"
#define WINLOGD_PARAM_KEY "SYSTEM\\CurrentControlSet\\Services\\winlogd\\Parameters"

#define WINLGOD_LOG_CRIT 2
#define WINLGOD_LOG_ERROR 3
#define WINLGOD_LOG_WARNING 4
//#define WINLGOD_LOG_NOTICE 5
#define WINLGOD_LOG_INFO 6

#include "winlogdService.h"

// Installer Class
public ref class winlogi : public Installer
{
public:
  winlogi(void)
  {
    ServiceProcessInstaller^ spi = gcnew ServiceProcessInstaller();
    spi->Account = ServiceAccount::LocalSystem;

    ServiceInstaller^ si = gcnew ServiceInstaller();
    si->ServiceName = "winlogd";
    si->StartType = ServiceStartMode::Automatic;

    Installers->Add(spi);
    Installers->Add(si);
  }
};

//install_uninstall - Will add or remove winlogd based on bool uninstall
int install_uninstall(bool uninstall)
{
  Collections::ArrayList^ cmdline = gcnew Collections::ArrayList();
  cmdline->Add(String::Format("/assemblypath={0}",System::Reflection::Assembly::GetExecutingAssembly()->Location));
  cmdline->Add("/logToConsole=false");
  cmdline->Add("/showCallStack");

  InstallContext^ ctx = gcnew InstallContext("winlogi.log", safe_cast<array<String^>^>(cmdline->ToArray(String::typeid)));

  TransactedInstaller^ ti = gcnew TransactedInstaller();
  ti->Installers->Add(gcnew winlogi());
  ti->Context = ctx;
  //try
  //{
    if (uninstall == false)
    {
      try
      {
        ti->Install(gcnew Collections::Hashtable());
      }
      catch (Exception^ e)
      {
        Console::WriteLine(e->InnerException->Message);
        return(1);
      }

  RegistryKey^ k = Registry::LocalMachine->OpenSubKey(WINLOGD_SERVICE_KEY,true);
  k->SetValue("Description","Sends Windows Event Log data to a syslog server");
  // Operation Parameters
  k->CreateSubKey("Parameters");
  k->Close();

  // Do Parameters
  String^ s;
  int i;
  // todo: check if it exists first then only change if it doesn't
  k = Registry::LocalMachine->OpenSubKey(WINLOGD_PARAM_KEY,true);
  // Flush
  //i = Convert::ToInt32( k->GetValue("Flush","0")->ToString() );
  //if (i==0) k->SetValue("Flush", 6000);
  // Monitor
  // k->SetValue("Monitor", 6000);
  i = Convert::ToInt32( k->GetValue("Port","0")->ToString() );
  if (i==0) k->SetValue("Port", 514);
  // Server
  s = k->GetValue("Server","default")->ToString();
  if (String::Compare(s,"default")==0) k->SetValue("Server", "syslog");
  s = k->GetValue("Facility","default")->ToString();
  if (String::Compare(s,"default")==0) k->SetValue("Facility", "local4");
  k->Close();

      Console::WriteLine("Installation successful, say `net start winlogd`");
    }
    else
    {
      try
      {
        ti->Uninstall(nullptr);
      }
      catch (Exception^ e)
      {
        //e->InnerException->Message
        Console::WriteLine(e->InnerException->Message);
        return(2);
      }
    }
        
  //}
  //catch (Exception^ e)
  //{
  //  Console::WriteLine( e->StackTrace );
  //  return 1;
  //}
  return 0;
}

int list_services()
{
  array<Diagnostics::EventLog^>^ logs = EventLog::GetEventLogs();
  Collections::IEnumerator^ e = logs->GetEnumerator();
  while (e->MoveNext())
  {
    Diagnostics::EventLog^ el = safe_cast<Diagnostics::EventLog^>(e->Current);
    Console::Write(String::Concat(el->Log,":\n  "));
    // Open Logs Registry Key to read sources
    RegistryKey^ k = Registry::LocalMachine->OpenSubKey(String::Concat("SYSTEM\\CurrentControlSet\\Services\\EventLog\\",el->Log));
    System::Object^ o = k->GetValue("Sources");
    if ( o->GetType()->IsArray )
    {
      array<String^>^ sources = safe_cast<array<String^>^>(o);
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
  RegistryKey^ k = Registry::LocalMachine->OpenSubKey(WINLOGD_PARAM_KEY);
  Console::WriteLine( String::Concat("Server:   ", k!=nullptr? k->GetValue("Server","syslog")->ToString() : "N/A" ) );
  Console::WriteLine( String::Concat("Port:     ", k!=nullptr? k->GetValue("Port","514")->ToString() : "N/A" ) );
  Console::WriteLine( String::Concat("Facility: ", k!=nullptr? k->GetValue("Facility","local1")->ToString() : "N/A" ) );
  Console::WriteLine( String::Concat("Monitor:  ", k!=nullptr? k->GetValue("Monitor","6000")->ToString() : "N/A" ) );
  Console::WriteLine( String::Concat("Flush:    ", k!=nullptr? k->GetValue("Flush","6000")->ToString() : "N/A" ) );
  if (k != nullptr)
    k->Close();
}

// func: test_winlogd - Write three messages to the log, then flush
int test_winlogd()
{
  String^ src = "winlogd";

  if ( !EventLog::SourceExists(src) ) EventLog::CreateEventSource(src, "Application"); 
  EventLog^ el = gcnew EventLog("Application", ".", src);

  el->WriteEntry( "Application Log Informational Message", EventLogEntryType::Information, 1, 1);
  el->WriteEntry( "Application Log Warning Message", EventLogEntryType::Warning, 2, 1 );
  el->WriteEntry( "Application Log Error Message", EventLogEntryType::Error, 3, 1 );
  el->WriteEntry( "Application Log Success Audit", EventLogEntryType::SuccessAudit, 4, 1 );
  el->WriteEntry( "Application Log Failure Audit", EventLogEntryType::FailureAudit, 5, 1 );

  return 0;
}

int main(array<System::String ^> ^args)
{
  try
  {
    if (args->Length >= 1)
    {
      String^ cmd = gcnew String(args[0]);
      if ( (cmd->Equals("-i")) || (cmd->Equals("-u")) ) { return install_uninstall(cmd->Equals("-u")); }
      else if ( (cmd->Equals("-d")) || (cmd->Equals("--dump")) ) { return reg_dump_parameters(); }
      else if ( (cmd->Equals("-h")) || (cmd->Equals("--help")) )
      {
        Console::Write(WINLOGD_APP_NAME);
        Console::Write("Options:\n");
        Console::Write("  --dump\tShow the registry parameters\n");
        Console::Write("  --help\tThis help\n");
        Console::Write("  --list\tList the Event Sources, facilities and levels\n");
        Console::Write("  --test\tRun some internal tests\n");
        Console::Write("  -i\t\tInstall\n  -u\t\tUninstall\n");
      }
      else if ( (cmd->Equals("-l")) || (cmd->Equals("--list")) ) { return list_services(); }
      else if ( (cmd->Equals("-t")) || (cmd->Equals("--test")) ) { return test_winlogd(); }
      else if ( (cmd->Equals("-V")) || (cmd->Equals("--version")) )
      {
        Console::WriteLine(WINLOGD_APP_NAME);
        Console::WriteLine("Released under the terms of the GPL");
        Console::WriteLine("see http://www.edoceo.com/ for more information");
      }
      return(0);
    }
    ServiceBase::Run(gcnew winlogd());
  }
  catch(Exception^ ex) 
  { 
    Console::WriteLine("An error occurred:"); 
    Console::WriteLine(ex->ToString()); 
  }
  return 0;
}
