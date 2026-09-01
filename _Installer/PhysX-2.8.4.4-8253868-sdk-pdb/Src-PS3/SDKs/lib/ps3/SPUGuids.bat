@echo off

rem   Run this from where the PhysX SPU elf files are compiled.
rem   (E.g. D:\Code\PhysX\Epic\PS3-PhysX_280_A1_2252\SDKs\compiler\ps3\build-SPU)

echo Generating spu_guids.txt:
echo PhysX SPU GUIDs: > spu_guids.txt


for %%i in (*.elf) do (
	scan-spu_guid %%i >> spu_guids.txt
)

type spu_guids.txt
