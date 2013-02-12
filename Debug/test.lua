local function test()
	local data = 1
	local name = "heath"
	for i = 1 , 10 do
		print(string.format("abc hello %d!%s" , i , name))
	end
	print(data)
end

test()
