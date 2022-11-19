
=============================================================================================================

Documentation of converter tool for cooked mesh files


AGEIA PhysX 2006

=============================================================================================================


Functionality:
--------------

With version 2.7.0 of the PhysX SDK the internal data of cooked mesh files has changed. This tool can be used
to convert old cooked mesh files into the new format.


Usage:
------

There are two different ways to use this tool:


(1) Specify the cooked mesh file to convert and the target file

    CookedMeshConverter platform f_old f_new

       platform:    target platform (PC, XENON or PS3)
       f_old:       file with cooked mesh in the old format
       f_new:       file to store the cooked mesh with the new format


    EXample:  CookedMeshConverter PC C:\MyDir\oldmesh.bin C:\MyDir\newmesh.bin
    ********


(2) Windows Operating System only:

    Specify the directory which holds the cooked mesh files you would like to have converted and specify the
    directory to store the converted files in.

    CookedMeshConverter platform dir_old dir_new

       platform:    target platform (PC, XENON or PS3)
       dir_old:     directory with cooked mesh files in the old format plus a file extension classifier
       dir_new:     file to store the cooked mesh files with the new format


    EXample:  CookedMeshConverter PC C:\MyDir\*.bin C:\NewDir\
    ********
              CookedMeshConverter PS3 C:\MyDir\*.* C:\NewDir\
