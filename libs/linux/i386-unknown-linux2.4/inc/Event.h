/*
 * See the dyninst/COPYRIGHT file for copyright information.
 * 
 * We provide the Paradyn Tools (below described as "Paradyn")
 * on an AS IS basis, and do not warrant its validity or performance.
 * We reserve the right to update, modify, or discontinue this
 * software at any time.  We shall have no obligation to supply such
 * updates or modifications or any other form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#if !defined(EVENT_H_)
#define EVENT_H_

#include <string>
#include <set>
#include "dyntypes.h"
#include "EventType.h"
#include "PCProcess.h"
#include "util.h"

class HandlerPool;
class HandleCallbacks;

namespace Dyninst {
namespace ProcControlAPI {

class PC_EXPORT ArchEvent
{
private:
   std::string name;
public:
   ArchEvent(std::string name_ = std::string(""));
   virtual ~ArchEvent();
   std::string getName(); 
};

class Handler;
class EventTerminate;
class EventExit;
class EventCrash;
class EventForceTerminate;
class EventExec;
class EventBreakpoint;
class EventStop;
class EventNewThread;
class EventNewUserThread;
class EventNewLWP;
class EventThreadDestroy;
class EventUserThreadDestroy;
class EventLWPDestroy;
class EventFork;
class EventSignal;
class EventBootstrap;
class EventRPC;
class EventSingleStep;
class EventBreakpointClear;
class EventBreakpointRestore;
class EventLibrary;
class EventRPCLaunch;
class EventAsync;
class EventChangePCStop;
class EventPreBootstrap;
class EventDetach;
class EventIntBootstrap;
class EventNop;
class EventThreadDB;
class EventWinStopThreadDestroy;

class PC_EXPORT Event : public boost::enable_shared_from_this<Event>
{
   friend void boost::checked_delete<Event>(Event *);
   friend void boost::checked_delete<const Event>(const Event *);
   friend class ::HandlerPool;
   friend class ::int_process;
   friend class ::HandleCallbacks;
 public:
   typedef boost::shared_ptr<Event> ptr;
   typedef boost::shared_ptr<const Event> const_ptr;
   typedef boost::weak_ptr<Event> weak_ptr;

   Event(EventType etype_, Thread::ptr thread_ = Thread::ptr());
   virtual ~Event();

   typedef enum {
      unset,
      async,
      sync_thread,
      sync_process
   } SyncType;

   EventType getEventType() const;
   Thread::const_ptr getThread() const;
   Process::const_ptr getProcess() const;
   SyncType getSyncType() const;
   std::string name() const;

   void setThread(Thread::const_ptr t);
   void setProcess(Process::const_ptr p);
   void setSyncType(SyncType t);
   void setSuppressCB(bool b);

   virtual bool suppressCB() const;
   virtual bool triggersCB() const;
   virtual bool canFastHandle() const;
   virtual bool userEvent() const;
   virtual void setUserEvent(bool b);
   virtual bool procStopper() const;

   Event::weak_ptr subservientTo() const;
   void addSubservientEvent(Event::ptr ev);

   boost::shared_ptr<EventTerminate> getEventTerminate();
   boost::shared_ptr<const EventTerminate> getEventTerminate() const;

   boost::shared_ptr<EventExit> getEventExit();
   boost::shared_ptr<const EventExit> getEventExit() const;

   boost::shared_ptr<EventCrash> getEventCrash();
   boost::shared_ptr<const EventCrash> getEventCrash() const;

   boost::shared_ptr<EventForceTerminate> getEventForceTerminate();
   boost::shared_ptr<const EventForceTerminate> getEventForceTerminate() const;

   boost::shared_ptr<EventExec> getEventExec();
   boost::shared_ptr<const EventExec> getEventExec() const;

   boost::shared_ptr<EventStop> getEventStop();
   boost::shared_ptr<const EventStop> getEventStop() const;

   boost::shared_ptr<EventBreakpoint> getEventBreakpoint();
   boost::shared_ptr<const EventBreakpoint> getEventBreakpoint() const;

   boost::shared_ptr<EventNewThread> getEventNewThread();
   boost::shared_ptr<const EventNewThread> getEventNewThread() const;

   boost::shared_ptr<EventNewUserThread> getEventNewUserThread();
   boost::shared_ptr<const EventNewUserThread> getEventNewUserThread() const;

   boost::shared_ptr<EventNewLWP> getEventNewLWP();
   boost::shared_ptr<const EventNewLWP> getEventNewLWP() const;

   boost::shared_ptr<EventThreadDestroy> getEventThreadDestroy();
   boost::shared_ptr<const EventThreadDestroy> getEventThreadDestroy() const;

   boost::shared_ptr<EventUserThreadDestroy> getEventUserThreadDestroy();
   boost::shared_ptr<const EventUserThreadDestroy> getEventUserThreadDestroy() const;

   boost::shared_ptr<EventLWPDestroy> getEventLWPDestroy();
   boost::shared_ptr<const EventLWPDestroy> getEventLWPDestroy() const;

   boost::shared_ptr<EventFork> getEventFork();
   boost::shared_ptr<const EventFork> getEventFork() const;

   boost::shared_ptr<EventSignal> getEventSignal();
   boost::shared_ptr<const EventSignal> getEventSignal() const;

   boost::shared_ptr<EventBootstrap> getEventBootstrap();
   boost::shared_ptr<const EventBootstrap> getEventBootstrap() const;

   boost::shared_ptr<EventPreBootstrap> getEventPreBootstrap();
   boost::shared_ptr<const EventPreBootstrap> getEventPreBootstrap() const;

   boost::shared_ptr<EventRPC> getEventRPC();
   boost::shared_ptr<const EventRPC> getEventRPC() const;

   boost::shared_ptr<EventRPCLaunch> getEventRPCLaunch();
   boost::shared_ptr<const EventRPCLaunch> getEventRPCLaunch() const;

   boost::shared_ptr<EventSingleStep> getEventSingleStep();
   boost::shared_ptr<const EventSingleStep> getEventSingleStep() const;

   boost::shared_ptr<EventBreakpointClear> getEventBreakpointClear();
   boost::shared_ptr<const EventBreakpointClear> getEventBreakpointClear() const;

   boost::shared_ptr<EventBreakpointRestore> getEventBreakpointRestore();
   boost::shared_ptr<const EventBreakpointRestore> getEventBreakpointRestore() const;

   boost::shared_ptr<EventLibrary> getEventLibrary();
   boost::shared_ptr<const EventLibrary> getEventLibrary() const;

   boost::shared_ptr<EventAsync> getEventAsync();
   boost::shared_ptr<const EventAsync> getEventAsync() const;

   boost::shared_ptr<EventChangePCStop> getEventChangePCStop();
   boost::shared_ptr<const EventChangePCStop> getEventChangePCStop() const;

   boost::shared_ptr<EventDetach> getEventDetach();
   boost::shared_ptr<const EventDetach> getEventDetach() const;

   boost::shared_ptr<EventIntBootstrap> getEventIntBootstrap();
   boost::shared_ptr<const EventIntBootstrap> getEventIntBootstrap() const;

   boost::shared_ptr<EventNop> getEventNop();
   boost::shared_ptr<const EventNop> getEventNop() const;

   boost::shared_ptr<EventThreadDB> getEventThreadDB();
   boost::shared_ptr<const EventThreadDB> getEventThreadDB() const;

   boost::shared_ptr<EventWinStopThreadDestroy> getEventWinStopThreadDestroy();
   boost::shared_ptr<const EventWinStopThreadDestroy> getEventWinStopThreadDestroy() const;

   //Not meant for public consumption
   void setLastError(err_t ec, const char *es);
 protected:
   EventType etype;
   Thread::const_ptr thread;
   Process::const_ptr proc;
   SyncType stype;
   std::vector<Event::ptr> subservient_events;
   Event::weak_ptr master_event;
   std::set<Handler *> handled_by;
   bool suppress_cb;
   bool user_event;
   bool handling_started;
   bool noted_event;
};

template<typename OS>
OS& operator<<(OS& str, Event& e)
{
	str << e.getEventType().name() << " event is ";
	switch(e.getSyncType())
	{
	case Event::async:
		str << "asynchronous ";
		break;
	case Event::sync_thread:
		str << "thread synchronous ";
		break;
	case Event::sync_process:
		str << "process synchronous ";
		break;
	default:
		str << "<UNKNOWN SYNC TYPE> ";
		break;
	}
	str << "on " << (e.getProcess() ? e.getProcess()->getPid() : -1);
	str << "/" << ((e.getThread()) ?  e.getThread()->getLWP() : (Dyninst::LWP) -1);
	str << "\n";
	return str;
}

class PC_EXPORT EventTerminate : public Event
{
   friend void boost::checked_delete<EventTerminate>(EventTerminate *);
   friend void boost::checked_delete<const EventTerminate>(const EventTerminate *);
 public:
   typedef boost::shared_ptr<EventTerminate> ptr;
   typedef boost::shared_ptr<const EventTerminate> const_ptr;
   EventTerminate(EventType type_);
   virtual ~EventTerminate();
};

class PC_EXPORT EventExit : public EventTerminate
{
   friend void boost::checked_delete<EventExit>(EventExit *);
   friend void boost::checked_delete<const EventExit>(const EventExit *);
 private:
   int exitcode;
 public:
   typedef boost::shared_ptr<EventExit> ptr;
   typedef boost::shared_ptr<const EventExit> const_ptr;
   int getExitCode() const;
   EventExit(EventType::Time eventtime, int exitcode_);
   virtual ~EventExit();
};

class PC_EXPORT EventCrash : public EventTerminate
{
   friend void boost::checked_delete<EventCrash>(EventCrash *);
   friend void boost::checked_delete<const EventCrash>(const EventCrash *);
 private:
   int termsig;
 public:
   typedef boost::shared_ptr<EventCrash> ptr;
   typedef boost::shared_ptr<const EventCrash> const_ptr;
   int getTermSignal() const;
   EventCrash(int termsig);
   virtual ~EventCrash();
};

class PC_EXPORT EventForceTerminate : public EventTerminate
{
   friend void boost::checked_delete<EventForceTerminate>(EventForceTerminate *);
   friend void boost::checked_delete<const EventForceTerminate>(const EventForceTerminate *);
 private:
   int termsig;
 public:
   typedef boost::shared_ptr<EventForceTerminate> ptr;
   typedef boost::shared_ptr<const EventForceTerminate> const_ptr;
   int getTermSignal() const;
   EventForceTerminate(int termsig);
   virtual ~EventForceTerminate();
};

class PC_EXPORT EventExec : public Event
{
   friend void boost::checked_delete<EventExec>(EventExec *);
   friend void boost::checked_delete<const EventExec>(const EventExec *);
 private:
   std::string execpath;
 public:
   typedef boost::shared_ptr<EventExec> ptr;
   typedef boost::shared_ptr<const EventExec> const_ptr;
   EventExec(EventType::Time etime_, std::string path = std::string(""));
   virtual ~EventExec();

   std::string getExecPath() const;
   void setExecPath(std::string path_);
};

class PC_EXPORT EventStop : public Event
{
   friend void boost::checked_delete<EventStop>(EventStop *);
   friend void boost::checked_delete<const EventStop>(const EventStop *);
 public:
   typedef boost::shared_ptr<EventStop> ptr;
   typedef boost::shared_ptr<const EventStop> const_ptr;
   EventStop();
   virtual ~EventStop();
};

class PC_EXPORT EventNewThread : public Event
{
   friend void boost::checked_delete<EventNewThread>(EventNewThread *);
   friend void boost::checked_delete<const EventNewThread>(const EventNewThread *);
 public:
   typedef boost::shared_ptr<EventNewThread> ptr;
   typedef boost::shared_ptr<const EventNewThread> const_ptr;
   EventNewThread(EventType et);
   virtual ~EventNewThread();

   virtual Dyninst::LWP getLWP() const = 0;
   virtual Thread::const_ptr getNewThread() const = 0;
};

class int_eventNewUserThread;
class PC_EXPORT EventNewUserThread : public EventNewThread
{
   friend void boost::checked_delete<EventNewUserThread>(EventNewUserThread *);
   friend void boost::checked_delete<const EventNewUserThread>(const EventNewUserThread *);
  private:
   int_eventNewUserThread *iev;
  public:
   typedef boost::shared_ptr<EventNewUserThread> ptr;
   typedef boost::shared_ptr<const EventNewUserThread> const_ptr;   

   EventNewUserThread();
   virtual ~EventNewUserThread();
   int_eventNewUserThread *getInternalEvent() const;

   virtual Dyninst::LWP getLWP() const;
   virtual Thread::const_ptr getNewThread() const;
};

class PC_EXPORT EventNewLWP : public EventNewThread
{
   friend void boost::checked_delete<EventNewLWP>(EventNewLWP *);
   friend void boost::checked_delete<const EventNewLWP>(const EventNewLWP *);
  private:
   Dyninst::LWP lwp;
  public:
   typedef boost::shared_ptr<EventNewLWP> ptr;
   typedef boost::shared_ptr<const EventNewLWP> const_ptr;   
   EventNewLWP(Dyninst::LWP lwp_);
   virtual ~EventNewLWP();

   virtual Dyninst::LWP getLWP() const;
   virtual Thread::const_ptr getNewThread() const;
};

class PC_EXPORT EventThreadDestroy : public Event
{
   friend void boost::checked_delete<EventThreadDestroy>(EventThreadDestroy *);
   friend void boost::checked_delete<const EventThreadDestroy>(const EventThreadDestroy *);
 public:
   typedef boost::shared_ptr<EventThreadDestroy> ptr;
   typedef boost::shared_ptr<const EventThreadDestroy> const_ptr;
   EventThreadDestroy(EventType et);
   virtual ~EventThreadDestroy() = 0;
};

class PC_EXPORT EventUserThreadDestroy : public EventThreadDestroy
{
   friend void boost::checked_delete<EventUserThreadDestroy>(EventUserThreadDestroy *);
   friend void boost::checked_delete<const EventUserThreadDestroy>(const EventUserThreadDestroy *);
 public:
   typedef boost::shared_ptr<EventUserThreadDestroy> ptr;
   typedef boost::shared_ptr<const EventUserThreadDestroy> const_ptr;
   EventUserThreadDestroy(EventType::Time time_);
   virtual ~EventUserThreadDestroy();
};

class PC_EXPORT EventLWPDestroy : public EventThreadDestroy
{
   friend void boost::checked_delete<EventLWPDestroy>(EventLWPDestroy *);
   friend void boost::checked_delete<const EventLWPDestroy>(const EventLWPDestroy *);
 public:
   typedef boost::shared_ptr<EventLWPDestroy> ptr;
   typedef boost::shared_ptr<const EventLWPDestroy> const_ptr;
   EventLWPDestroy(EventType::Time time_);
   virtual ~EventLWPDestroy();
};

class PC_EXPORT EventFork : public Event
{
   friend void boost::checked_delete<EventFork>(EventFork *);
   friend void boost::checked_delete<const EventFork>(const EventFork *);
  private:
   Dyninst::PID pid;
  public:
   typedef boost::shared_ptr<EventFork> ptr;
   typedef boost::shared_ptr<const EventFork> const_ptr;
   EventFork(EventType::Time time_, Dyninst::PID pid_);
   virtual ~EventFork();
   Dyninst::PID getPID() const;
   Process::const_ptr getChildProcess() const;
};

class PC_EXPORT EventSignal : public Event
{
   friend void boost::checked_delete<EventSignal>(EventSignal *);
   friend void boost::checked_delete<const EventSignal>(const EventSignal *);
 private:
   int sig;
 public:
   typedef boost::shared_ptr<EventSignal> ptr;
   typedef boost::shared_ptr<const EventSignal> const_ptr;
   EventSignal(int sig);
   virtual ~EventSignal();

   int getSignal() const;
   void setThreadSignal(int newSignal) const;
   void clearThreadSignal() const;
};

class PC_EXPORT EventBootstrap : public Event
{
   friend void boost::checked_delete<EventBootstrap>(EventBootstrap *);
   friend void boost::checked_delete<const EventBootstrap>(const EventBootstrap *);
 public:
   typedef boost::shared_ptr<EventBootstrap> ptr;
   typedef boost::shared_ptr<const EventBootstrap> const_ptr;
   EventBootstrap();
   virtual ~EventBootstrap();
};

class PC_EXPORT EventPreBootstrap : public Event
{
   friend void boost::checked_delete<EventPreBootstrap>(EventPreBootstrap *);
   friend void boost::checked_delete<const EventPreBootstrap>(const EventPreBootstrap *);
 public:
   typedef boost::shared_ptr<EventPreBootstrap> ptr;
   typedef boost::shared_ptr<const EventPreBootstrap> const_ptr;
   EventPreBootstrap();
   virtual ~EventPreBootstrap();
};


class int_eventRPC;
class PC_EXPORT EventRPC : public Event
{
   friend void boost::checked_delete<EventRPC>(EventRPC *);
   friend void boost::checked_delete<const EventRPC>(const EventRPC *);
 private:
   int_eventRPC *int_rpc;
   rpc_wrapper *wrapper;
 public:
   virtual bool suppressCB() const;
   rpc_wrapper *getllRPC();
   typedef boost::shared_ptr<EventRPC> ptr;
   typedef boost::shared_ptr<const EventRPC> const_ptr;
   EventRPC(rpc_wrapper *wrapper_);
   virtual ~EventRPC();

   IRPC::const_ptr getIRPC() const;
   int_eventRPC *getInternal() const;
};

class PC_EXPORT EventRPCLaunch : public Event
{
   friend void boost::checked_delete<EventRPCLaunch>(EventRPCLaunch *);
   friend void boost::checked_delete<const EventRPCLaunch>(const EventRPCLaunch *);
 public:
   typedef boost::shared_ptr<EventRPCLaunch> ptr;
   typedef boost::shared_ptr<const EventRPCLaunch> const_ptr;
   virtual bool procStopper() const;
   EventRPCLaunch();
   virtual ~EventRPCLaunch();
};

class PC_EXPORT EventSingleStep : public Event
{
   friend void boost::checked_delete<EventSingleStep>(EventSingleStep *);
   friend void boost::checked_delete<const EventSingleStep>(const EventSingleStep *);
 public:
   typedef boost::shared_ptr<EventSingleStep> ptr;
   typedef boost::shared_ptr<const EventSingleStep> const_ptr;
   EventSingleStep();
   virtual ~EventSingleStep();
};

class int_eventBreakpoint;
class PC_EXPORT EventBreakpoint : public Event
{
   friend void boost::checked_delete<EventBreakpoint>(EventBreakpoint *);
   friend void boost::checked_delete<const EventBreakpoint>(const EventBreakpoint *);
 private:
   int_eventBreakpoint *int_bp;
 public:
   typedef boost::shared_ptr<EventBreakpoint> ptr;
   typedef boost::shared_ptr<const EventBreakpoint> const_ptr;
   int_eventBreakpoint *getInternal() const;

   EventBreakpoint(int_eventBreakpoint *ibp);
   virtual ~EventBreakpoint();

   Dyninst::Address getAddress() const;
   void getBreakpoints(std::vector<Breakpoint::const_ptr> &bps) const;
   void getBreakpoints(std::vector<Breakpoint::ptr> &bps);
   virtual bool suppressCB() const;
   virtual bool procStopper() const;
};


class int_eventBreakpointClear;
class PC_EXPORT EventBreakpointClear : public Event
{
   friend void boost::checked_delete<EventBreakpointClear>(EventBreakpointClear *);
   friend void boost::checked_delete<const EventBreakpointClear>(const EventBreakpointClear *);
  private:
   int_eventBreakpointClear *int_bpc;
  public:
   typedef boost::shared_ptr<EventBreakpointClear> ptr;
   typedef boost::shared_ptr<const EventBreakpointClear> const_ptr;
   EventBreakpointClear();
   virtual ~EventBreakpointClear();
   
   int_eventBreakpointClear *getInternal() const;
   virtual bool procStopper() const;
};

class int_eventBreakpointRestore;
class EventBreakpointRestore : public Event
{
   friend void boost::checked_delete<EventBreakpointRestore>(EventBreakpointRestore *);
   friend void boost::checked_delete<const EventBreakpointRestore>(const EventBreakpointRestore *);
  private:
   int_eventBreakpointRestore *int_bpr;
  public:
   typedef boost::shared_ptr<EventBreakpointRestore> ptr;
   typedef boost::shared_ptr<const EventBreakpointRestore> const_ptr;

   EventBreakpointRestore(int_eventBreakpointRestore *iebpr);
   virtual ~EventBreakpointRestore();

   int_eventBreakpointRestore *getInternal() const;
};

class PC_EXPORT EventLibrary : public Event
{
   friend void boost::checked_delete<EventLibrary>(EventLibrary *);
   friend void boost::checked_delete<const EventLibrary>(const EventLibrary *);
 private:
   std::set<Library::ptr> added_libs;
   std::set<Library::ptr> rmd_libs;
 public:
   typedef boost::shared_ptr<EventLibrary> ptr;
   typedef boost::shared_ptr<const EventLibrary> const_ptr;
   EventLibrary();
   EventLibrary(const std::set<Library::ptr> &added_libs_,
                const std::set<Library::ptr> &rmd_libs_);
   virtual ~EventLibrary();

   void setLibs(const std::set<Library::ptr> &added_libs_,
                const std::set<Library::ptr> &rmd_libs_);
   const std::set<Library::ptr> &libsAdded() const;
   const std::set<Library::ptr> &libsRemoved() const;
};

class int_eventAsync;
class PC_EXPORT EventAsync : public Event
{
   friend void boost::checked_delete<EventAsync>(EventAsync *);
   friend void boost::checked_delete<const EventAsync>(const EventAsync *);
   
  private:
   int_eventAsync *internal;
  public:
   typedef boost::shared_ptr<EventAsync> ptr;
   typedef boost::shared_ptr<const EventAsync> const_ptr;

   EventAsync(int_eventAsync *ievent);
   virtual ~EventAsync();
   int_eventAsync *getInternal() const;
};

class PC_EXPORT EventChangePCStop : public Event
{
   friend void boost::checked_delete<EventChangePCStop>(EventChangePCStop *);
   friend void boost::checked_delete<const EventChangePCStop>(const EventChangePCStop *);
 public:
   typedef boost::shared_ptr<EventChangePCStop> ptr;
   typedef boost::shared_ptr<const EventChangePCStop> const_ptr;
   EventChangePCStop();
   virtual ~EventChangePCStop();
};

class int_eventDetach;
class PC_EXPORT EventDetach : public Event
{
   friend void boost::checked_delete<EventDetach>(EventDetach *);
   friend void boost::checked_delete<const EventDetach>(const EventDetach *);
   int_eventDetach *int_detach;
 public:
   typedef boost::shared_ptr<EventDetach> ptr;
   typedef boost::shared_ptr<const EventDetach> const_ptr;

   EventDetach();
   virtual ~EventDetach();
   int_eventDetach *getInternal() const;
   virtual bool procStopper() const;
};

class PC_EXPORT EventIntBootstrap : public Event
{
   friend void boost::checked_delete<EventIntBootstrap>(EventIntBootstrap *);
   friend void boost::checked_delete<const EventIntBootstrap>(const EventIntBootstrap *);
   
   void *data;
 public:
   typedef boost::shared_ptr<EventIntBootstrap> ptr;
   typedef boost::shared_ptr<const EventIntBootstrap> const_ptr;
   EventIntBootstrap(void *d = NULL);
   virtual ~EventIntBootstrap();

   void *getData() const;
   void setData(void *v);
};

class PC_EXPORT EventNop : public Event
{
   friend void boost::checked_delete<EventNop>(EventNop *);
   friend void boost::checked_delete<const EventNop>(const EventNop *);
 public:
   typedef boost::shared_ptr<EventNop> ptr;
   typedef boost::shared_ptr<const EventNop> const_ptr;
   EventNop();
   virtual ~EventNop();
};

class int_eventThreadDB;
class PC_EXPORT EventThreadDB : public Event
{
   friend void boost::checked_delete<EventThreadDB>(EventThreadDB *);
   friend void boost::checked_delete<const EventThreadDB>(const EventThreadDB *);
   int_eventThreadDB *int_etdb;
  public:
   typedef boost::shared_ptr<EventThreadDB> ptr;
   typedef boost::shared_ptr<const EventThreadDB> const_ptr;
   int_eventThreadDB *getInternal() const;

   EventThreadDB();
   virtual ~EventThreadDB();

   virtual bool triggersCB() const;
};

class PC_EXPORT EventWinStopThreadDestroy : public EventThreadDestroy
{
   friend void boost::checked_delete<EventWinStopThreadDestroy>(EventWinStopThreadDestroy *);
   friend void boost::checked_delete<const EventWinStopThreadDestroy>(const EventWinStopThreadDestroy *);
 public:
   typedef boost::shared_ptr<EventWinStopThreadDestroy> ptr;
   typedef boost::shared_ptr<const EventWinStopThreadDestroy> const_ptr;
   EventWinStopThreadDestroy(EventType::Time time_);
   virtual ~EventWinStopThreadDestroy();
};

}
}
#endif
