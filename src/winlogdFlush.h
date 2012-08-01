
/*
winlogdFlush.h - busby@edoceo.cm
Defines the Class for the thread that would flush the EventLogs
*/

// Flush Thread Class
public __gc class winlogf
{
private:
  Diagnostics::EventLog* flogs[];
  System::Collections::Hashtable* ht;
  
public:
  winlogf()
  {
    flogs = EventLog::GetEventLogs();
    Collections::IEnumerator* e = flogs->GetEnumerator();
    e->Reset();
    while (e->MoveNext())
    {
      Diagnostics::EventLog* el = __try_cast<Diagnostics::EventLog*>(e->Current);
      ht->Add ( el->Log, __box(el->Entries->Count) );
    }
  }
  void FlushRun()
  {
    // TODO: Make an array of items that will store the count of items in the log
    //  Perhaps the other thread winlogdService should keep track of how many items are in the log
    Collections::IEnumerator* e = flogs->GetEnumerator();
    e->Reset();
    while (e->MoveNext())
    {
      Diagnostics::EventLog* el = __try_cast<Diagnostics::EventLog*>(e->Current);
      //if (el->Entries->Count > 

    }
  }
};
