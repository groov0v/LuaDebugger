local function scandir(directory)
	local i, t, popen = 0, {}, io.popen     
	for filename in popen('dir "'..directory..'" /b'):lines() do
		i = i + 1         
		t[i] = filename
	end     
	return t 
end

local pool = {}
local function parser(t , text)
	string.gsub(text , '<resource%s+path=%s-"(.-)"', function(s) 
		if pool[s] == nil then
			pool[s] = true
			table.insert(t , s)
			print(s)
		end
	end)
end

local dirs = scandir(".")
local res = {}
-- get resource path string
for _ , f in ipairs(dirs) do
	if string.find(f , "^%d+.*.xml$") ~= nil then
		local index = string.sub(f , string.find(f , "%d+"))
		if res[index] == nil then
			res[index] = {}
		end
		local h = io.input(f)
		parser(res[index] , h:read("*a"))
		h:close()
	end
end
-- generate lua script
local luafile = io.output("cutscene_preload.lua")
-- write table
luafile:write("-- AUTO GENERATED FILE, DO NOT MODIFY IT.\n")
luafile:write("local resources =\n{\n")
for k , v in pairs(res) do
	luafile:write("\t["..k.."] = \n\t\t{\n")
	for kk , vv in pairs(v) do
		luafile:write('\t\t"'..vv..'" ,\n')
	end
	luafile:write("\t\t} ,\n")
end
luafile:write("}\n\n")
-- write function
luafile:write('function CutScene_DoPreLoad(mapid)\n\tif resources[mapid] ~= nil then\n\t\tfor _ , v in ipairs(resources[mapid]) do\n\t\t\tGAME:DeclareMapResource(v)\n\t\tend\n\tend\nend')
luafile:close()

