-- Like WTF? Lua does not provide this?

function keys(table) 
	local ret = {};
	for k,v in pairs(table) do
		ret[#ret+1] = k;
	end
	return ret;
end

function without(array, elem)
	log('without');
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
