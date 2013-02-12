#ifndef _LUA_DEBUGGER_H
#define _LUA_DEBUGGER_H

#include <lua.hpp>
#include <list>

class LuaDebugger
{
public:
	typedef enum { DM_NORM , DM_STEP , DM_FUNC } Mode;
protected:
	lua_State* m_pGlobalState;
	lua_State* m_pDebuggeeState;
	std::list<int> m_listBreakPoints;
	Mode m_emMode;
public:
	static LuaDebugger* m_spActiveDebugger;
public:
	LuaDebugger();
	~LuaDebugger();

	void LoadLuaFile(const char* name);
	void Run();
	void Pause();
	void Stop();
	void AddBreakPoint(int lineNum);
	void RemoveBreakPoint(int lineNum);
	bool FindBreakPoint(int lineNum);
	void WatchVarValue();
	void SetMode(const Mode mode);
	Mode GetMode() { return m_emMode; }

	static void Hook(lua_State* L , lua_Debug* ar);
};

#endif