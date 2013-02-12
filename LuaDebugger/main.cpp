#include "LuaDebugger.h"

int main(int argc , const char* argv[])
{
	LuaDebugger debugger;
	LuaDebugger::m_spActiveDebugger = &debugger;

	if(argc > 1)
	{
		debugger.LoadLuaFile(argv[1]);
	}

	char cmd[256] = {0};
	int line;
	while(1)
	{
		fprintf(stdout , "LuaDBG:>");
		fgets(cmd , 255 , stdin);
		cmd[strlen(cmd) - 1] = '\0';
		switch (cmd[0])
		{
		case 'g':
			debugger.Run();
			break;
		case 'b':
			line = atoi(&cmd[2]);
			if(!line)
				fprintf(stderr , "a invalid line number!\n");
			else
				debugger.AddBreakPoint(line);
			break;
		case 'c':
			line = atoi(&cmd[2]);
			if(!line)
				fprintf(stderr , "a invalid line number!\n");
			else
				debugger.RemoveBreakPoint(line);
			break;
		case 'l':
			debugger.LoadLuaFile(&cmd[2]);
			break;
		case 'n':
			debugger.SetMode(LuaDebugger::DM_STEP);
			debugger.Run();
			break;
		case 'f':
			debugger.SetMode(LuaDebugger::DM_FUNC);
			debugger.Run();
			break;
		case 'q':
			printf("end debugger.\n");
			return 0;
		default:
			fprintf(stderr , "unknown command!\n");
			break;
		}
	}

	return 0;
}