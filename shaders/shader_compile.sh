


 ../game/shaderc -f "./$1.sc" --type $3 -o "./$2/$1.bin" -i "../dep/bgfx.cmake/bgfx/src" --platform windows --verbose --varyingdef ./varying.def.sc
echo "done compiling"
sleep 120