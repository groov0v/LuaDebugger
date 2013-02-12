#include "LuaDebugger.h"
#include <cstdio>

LuaDebugger* LuaDebugger::m_spActiveDebugger = NULL;

LuaDebugger::LuaDebugger()
{
	m_pGlobalState = luaL_newstate();
	luaL_openlibs(m_pGlobalState);
	m_pDebuggeeState = lua_newthread(m_pGlobalState);
}

LuaDebugger::~LuaDebugger()
{
	lua_close(m_pGlobalState);
	if(this == m_spActiveDebugger)
		m_spActiveDebugger = NULL;
}

void LuaDebugger::LoadLuaFile(const char* name)
{
	if(luaL_loadfile(m_pDebuggeeState , name))
    {
		fprintf(stderr , "can not load script(%s)\n" , lua_tostring(m_pDebuggeeState , -1));
		lua_pop(m_pDebuggeeState , 1);
    }
}

void LuaDebugger::Hook(lua_State* L , lua_Debug* ar)
{
    if(!m_spActiveDebugger)
		fprintf(stderr , "no active debugger!\n");
	else
	{
		switch(ar->event)
		{
		case LUA_HOOKLINE:
			{
				if(m_spActiveDebugger->GetMode() == DM_STEP)
				{
					m_spActiveDebugger->Pause();
					m_spActiveDebugger->WatchVarValue();
					m_spActiveDebugger->SetMode(DM_NORM);
				}
				else if(m_spActiveDebugger->FindBreakPoint(ar->currentline))
				{
					printf("break on line %d\n" , ar->currentline);
					m_spActiveDebugger->Pause();
					m_spActiveDebugger->WatchVarValue();
				}
			}
			break;
		case LUA_HOOKCALL:
			{
				if(m_spActiveDebugger->GetMode() == DM_FUNC)
				{
					m_spActiveDebugger->Pause();
					m_spActiveDebugger->WatchVarValue();
					m_spActiveDebugger->SetMode(DM_NORM);
				}
			}
			break;
		}
	}
}

void LuaDebugger::Run()
{
    int err = lua_resume(m_pDebuggeeState , NULL , 0);
    if(err == LUA_ERRRUN)
        fprintf(stderr , "no code to run!\n");
    else if(err != LUA_OK && err != LUA_YIELD)
        fprintf(stderr , "error to run (%d)\n" , err);
}

void LuaDebugger::Pause()
{
	lua_yield(m_pDebuggeeState , 0);
}

void LuaDebugger::Stop()
{

}

void LuaDebugger::SetMode(const Mode mode)
{
	int mask = lua_gethookmask(m_pDebuggeeState);
	m_emMode = mode;
	switch(m_emMode)
	{
	case DM_NORM:
		if(m_listBreakPoints.empty())
			lua_sethook(m_pDebuggeeState , NULL , 0 , 0);
		else
			lua_sethook(m_pDebuggeeState , Hook , LUA_MASKLINE , 0);
		break;
	case DM_FUNC:
		lua_sethook(m_pDebuggeeState , Hook , LUA_MASKCALL | mask , 0);
		break;
	case DM_STEP:
		lua_sethook(m_pDebuggeeState , Hook , LUA_MASKLINE | mask , 0);
		break;
	}
}

void LuaDebugger::AddBreakPoint(int lineNum)
{
	/// TODO: check the validation of lineNum
	if(m_listBreakPoints.empty())
		lua_sethook(m_pDebuggeeState , Hook , LUA_MASKLINE , 0);
	
	std::list<int>::const_iterator iter = m_listBreakPoints.begin();
	bool bExist = false;
	for(; iter != m_listBreakPoints.end(); ++iter)
	{
		if(*iter == lineNum)
		{
			fprintf(stderr , "the breakpoint has been added!\n");
			bExist  = true;
			break;
		}
	}

	if(!bExist)
	{
		m_listBreakPoints.push_back(lineNum);
		printf("add a breakpoint on #%d.\n" , lineNum);
	}
	else
	{
		if(m_listBreakPoints.empty())
			lua_sethook(m_pDebuggeeState , NULL , 0 , 0);
	}
}

void LuaDebugger::RemoveBreakPoint(int lineNum)
{
	std::list<int>::iterator iter = m_listBreakPoints.begin();
	bool bExist = false;
	for(; iter != m_listBreakPoints.end(); ++iter)
	{
		if(*iter == lineNum)
		{
			m_listBreakPoints.erase(iter);
			printf("remove a breakpoint on #%d.\n" , lineNum);
			bExist  = true;
			break;
		}
	}

	if(!bExist)
		fprintf(stderr , "no breakpoint found on #%d.\n" , lineNum);
}

bool LuaDebugger::FindBreakPoint(int lineNum)
{
	std::list<int>::const_iterator iter = m_listBreakPoints.begin();
	for(; iter != m_listBreakPoints.end(); ++iter)
	{
		if(*iter == lineNum)
		{
			return true;
		}
	}

	return false;
}

void LuaDebugger::WatchVarValue()
{
	lua_Debug ar;
	int level = 0;
	char buf[256] = {0};

	while(lua_getstack(m_pDebuggeeState , level++ , &ar))
	{
		int i = 1;
		const char* name;
		lua_getinfo(m_pDebuggeeState , "Sln" , &ar);
		printf("%s %s function %s in %s :\n" , ar.namewhat , ar.what , ar.name , ar.short_src);
		while(name = lua_getlocal(m_pDebuggeeState , &ar , i++))
		{
			lua_getinfo(m_pDebuggeeState , "Sln" , &ar);
			
			buf[0] = NULL;
			if(lua_isnumber(m_pDebuggeeState , -1))
				sprintf(buf , "\t%s = %f\n" , name , lua_tonumber(m_pDebuggeeState , -1));
			else if(lua_isstring(m_pDebuggeeState , -1))
				sprintf(buf , "\t%s = %s\n" , name , lua_tostring(m_pDebuggeeState , -1));
			else if(lua_istable(m_pDebuggeeState , -1))
				sprintf(buf , "\t%s is table.\n" , name);

			printf(buf);
			lua_pop(m_pDebuggeeState , 1);
		}
	}
}
