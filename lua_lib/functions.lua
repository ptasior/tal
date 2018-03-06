-- Like WTF? Lua does not provide this?

function keys(table) 
	local ret = {};
	for k,v in pairs(table) do
		ret[#ret+1] = k;
	end
	return ret;
end

function without(array, elem)
	ret = {};
	for i = 0, #array do
		if(array[i] ~= elem) then
			ret[#ret+1] = array[i];
		end
	end
	return ret;
end

function inList(array, elem)
	for i = 1, #array do
		if(array[i] == elem) then
			return true;
		end
	end
	return false;
end

function hasKey(array, elem)
	return inList(keys(array), elem);
end

function size(table)
	local count = 0
	for _ in pairs(table) do
		count = count + 1
	end
	return count
end

function join(delimiter, list)
	local len = #list
	if len == 0 then
		return ""
	end
	local string = list[1]
	for i = 2, len do
		string = string .. delimiter .. list[i]
	end
	return string
end

function split(div, str)
	if (div=='') then return false end
	local pos,arr = 0,{}
	-- for each divider found
	for st,sp in function() return string.find(str,div,pos,true) end do
		table.insert(arr,string.sub(str,pos,st-1)) -- Attach chars left of current divider
		pos = sp + 1 -- Jump past current divider
	end
	table.insert(arr,string.sub(str,pos)) -- Attach chars right of last divider
	return arr
end

function find(what, array)
	for i = 1,#array do
		if(array[i] == what) then
			return i;
		end
	end
	return -1;
end

function startsWith(text, part)
   return string.sub(text, 1, string.len(part)) == part;
end

function endsWith(text, part)
   return part == '' or string.sub(text,-string.len(part)) == part;
end

function map(table, handler)
	local ret = {};

	for k, v in pairs(table) do
		ret[#ret+1] = handler(v);
	end

	return ret;
end

