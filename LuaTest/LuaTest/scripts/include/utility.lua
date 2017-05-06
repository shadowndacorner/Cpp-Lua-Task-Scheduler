function print_table(tab, depth)
	local tab = ""
	depth=depth or 0;
	for i=0, depth do
		tab = tab.."\t"
	end
	
	for k, v in pairs(tab) do
		if (type(v) == "table") then
			print(k .. ":")
			print_table(tab, depth + 1)
		else
			print(k .. ": " .. tostring(v))
		end
	end
end