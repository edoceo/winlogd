/*
 $Id$
 spec: Defines the winlogd Service

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

// note: Lots of improvements could be made
// note: Localisation would be nice

// Service Class
public __gc class winlogd : public ServiceBase
{
private:

  static int facility=0;
  // First one is to offset so Jan==1 and Dec==12, this should be localised
  static String* months __gc[] = { "NUL","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
  static String* facilities __gc[] = { "local0","local1","local2","local3","local4","local5","local6","local7" };
    
  Diagnostics::EventLog* mlogs[];  // Logs to monitor
  UdpClient* syslog_server;

public:
  winlogd(void)
  {
    this->AutoLog = false;
    this->CanHandlePowerEvent = true;
    this->CanPauseAndContinue = false;
    this->CanShutdown = true;
    this->ServiceName = "winlogd";
  }

  void OnStart(String* args __gc[])
  {
    // Attach Listener to each EventLog
    mlogs = EventLog::GetEventLogs();
    Collections::IEnumerator* e = mlogs->GetEnumerator();
    while (e->MoveNext())
    {
      Diagnostics::EventLog* el = __try_cast<Diagnostics::EventLog*>(e->Current);
      el->EnableRaisingEvents = true;
      el->EntryWritten+= new EntryWrittenEventHandler(this, &winlogd::EventHook);
    }

    // Read our operation parameters
    RegistryKey* k = Registry::LocalMachine->OpenSubKey(WINLOGD_PARAM_KEY);
    this->syslog_server = new UdpClient(
      k->GetValue("Server",S"syslog")->ToString()->ToLower() ,
      Convert::ToInt32(k->GetValue("Port",S"514")->ToString())
    );

    // Facility
    int i=0;
    this->facility = 16; // Default to local0
    String* f = Convert::ToString(k->GetValue("Facility"));
    for(i=0;i<facilities->Length;i++)
    {
      if (String::Compare(f , this->facilities[i], true)==0)
      {
        facility = i+16;
        break;
      }
    }
    // String array of sources to ignore
    //System::Object* o = k->GetValue("Ignore");
    //if ( o->GetType()->IsArray ) sources[] = __try_cast<String*[]>(o);
    k->Close();
  }

  // func: OnPowerEvent(PowerBroadcastStatus ps)
  // note: Currently this does not do anything 
  bool OnPowerEvent(PowerBroadcastStatus ps)
  {
    //Byte pkt[] = System::Text::Encoding::ASCII->GetBytes( "Power Event!" );
    //send_packet(pkt);
    return true;
  }

  void OnStop(void)
  {
    // Close all the EventLog objects
    Collections::IEnumerator* e = mlogs->GetEnumerator();
    while (e->MoveNext())
    {
      Diagnostics::EventLog* el = __try_cast<Diagnostics::EventLog*>(e->Current);
      el->Close();
    }
  }

  void OnShutdown()
  {
    //Text::StringBuilder* msg = new Text::StringBuilder(1024,1024);
    //msg->Append(String::Concat("<", Convert::ToString( (facility*8)+6), ">"));
  }

  // func: EventHook(Object* o, EntryWrittenEventArgs* e)
  // spec: This is the meat of this application
  void EventHook(Object* o, EntryWrittenEventArgs* e)
  {
    Text::StringBuilder* msg = new Text::StringBuilder(1024,1024);

    // Windows Only Has Error, Warning and Notice, FailureAudit and SuccessAudit
    int priority = 0;
    if (e->Entry->EntryType == EventLogEntryType::Information) priority = WINLGOD_LOG_INFO;
    else if (e->Entry->EntryType == EventLogEntryType::Warning) priority =  WINLGOD_LOG_WARNING;
    else if (e->Entry->EntryType == EventLogEntryType::Error) priority = WINLGOD_LOG_ERROR;
    else if (e->Entry->EntryType == EventLogEntryType::SuccessAudit) priority = WINLGOD_LOG_CRIT; // LOG_CRIT
    else if (e->Entry->EntryType == EventLogEntryType::FailureAudit) priority = WINLGOD_LOG_CRIT; // LOG_CRIT
    
    // PRI
    msg->Append(String::Concat("<", Convert::ToString( (facility*8)+priority ), ">"));
    
    // HEADER::TIMESTAMP
    msg->Append( String::Concat(this->months[e->Entry->TimeGenerated.Month]," ") );
    msg->Append( String::Format( "{0,2}", e->Entry->TimeGenerated.Day.ToString()) );
    msg->Append(S" ");
    msg->Append( String::Concat(e->Entry->TimeGenerated.Hour.ToString("00"),":") );
    msg->Append( String::Concat(e->Entry->TimeGenerated.Minute.ToString("00"),":") );
    msg->Append( String::Concat(e->Entry->TimeGenerated.Second.ToString("00")," ") );
    
    // HEADER::HOSTNAME
    msg->Append( String::Concat( e->Entry->MachineName->ToLower()," ") );
    
    // MSG::TAG
    // todo: update this to replace ' ' in the Source with '_'
    msg->Append( String::Concat(e->Entry->Source,"[", e->Entry->EventID.ToString(), "]: " ));
    
    // MSG::CONTENT
    // Have to clean out \r and \t, syslog will replace \n with " "
    String* clean = e->Entry->Message->Replace("\r","");
    msg->Append( clean->Replace("\t","") );

    // Send To Server
    Byte pkt[] = System::Text::Encoding::ASCII->GetBytes(msg->ToString());
    this->syslog_server->Send(pkt,pkt->Length);
    delete pkt;

    // note: this is useful for debugging
    //StreamWriter* sw = File::AppendText("c:\\winlogd.log");
    //sw->WriteLine(msg->ToString());
    //sw->Close();
  }
};
