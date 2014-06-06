@echo off
cd shader
set i= 0
for /r %%f in (*) do (
	echo Validating shader: %%~nf
	..\glslangValidator.exe %%f
	set /a i+=1
)
echo ------ Validated %i% shader files. ------
cd ..