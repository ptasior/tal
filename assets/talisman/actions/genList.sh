#!/bin/bash

rm -f list.lua

echo "return {" >> list.lua
for i in `find * -name '*.lua' ! -name 'list.lua'`
do
	echo "'${i:0:-4}'," >> list.lua
done
echo "}" >> list.lua

