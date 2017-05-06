function PrintTable(tab, depth)
	local tabs = ""
	depth=depth or 0;
	for i=0, depth do
		tabs = tabs.."\t"
	end
	
	for k, v in pairs(tab) do
		if (type(v) == "table") then
			print(tabs .. k .. ":")
			PrintTable(tab, depth + 1)
		else
			print(tabs .. k .. ": " .. tostring(v))
		end
	end
end