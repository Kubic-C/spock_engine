

echo "options: $1, $2, $3, $4 $5 "
echo "starting compiling"
shaderc -f "./$1.sc" --type $3 -o "./$2/$1.bin" -i "../dep/bgfx.cmake/bgfx/src" --platform linux --varyingdef ./varying.def.sc $4 $5
echo "done compiling"
sleep 120