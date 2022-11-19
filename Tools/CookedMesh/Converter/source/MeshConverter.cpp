// ====================================================================================
//
//                                    NVIDIA PHYSX
//
// Content: Converter tool for cooked mesh files to substitute classic collision trees
//          with non-recursive, i.e., stackless collision trees
//
// Comment: Works for convex and non-convex mesh files
//
// Author:  Michael Sauter
//
// ====================================================================================

#include "MeshConverter.h"

// Physics SDK globals
NxCookingInterface* gCooking = NULL;

// Character buffer globals
const int text_buf_size = 500;


class MyErrorReport : public NxUserOutputStream
{
	public:
        virtual void reportError(NxErrorCode code, const char *message, const char *file, int line)
        {
            printf("Error %d: %s\nFILE: %s\nLINE %d\n", code, message, file, line);
        }

        virtual NxAssertResponse reportAssertViolation(const char *message, const char *file, int line)
        {
            printf("Assert violation: %s\nFILE: %s\nLINE %d\n", message, file, line);
            return NX_AR_BREAKPOINT;
        }

        virtual void print(const char *message)
        {
            printf(message);
        }
} gMyErrorReport;


bool convertMeshFile(const char* old_file, const char* new_file)
{
    assert(old_file != NULL);
    assert(new_file != NULL);

	bool status = false;
	CmMeshData meshDesc;
	CmMeshType meshType;
	bool hintCollisionSpeed;

	status = GetCookedData(old_file, meshType, meshDesc, hintCollisionSpeed);

	if (status)
	{
		// try to open file for writing
		UserStream out_stream(new_file, false);
		if (out_stream.isOpen())
		{
			// set cooking params
			NxCookingParams cookParams = gCooking->NxGetCookingParams();
			cookParams.hintCollisionSpeed = hintCollisionSpeed;
			gCooking->NxSetCookingParams(cookParams);

			if (meshType == MT_TRIANGLE_MESH)
			{
				NxTriangleMeshDesc triMeshDesc;
					
				triMeshDesc.numVertices					= meshDesc.numVertices;
				triMeshDesc.numTriangles			 	= meshDesc.numTriangles;
				triMeshDesc.pointStrideBytes			= meshDesc.pointStrideBytes;
				triMeshDesc.triangleStrideBytes			= meshDesc.triangleStrideBytes;
				triMeshDesc.points						= meshDesc.points;
				triMeshDesc.triangles					= meshDesc.triangles;
				triMeshDesc.flags						= meshDesc.flags;

				triMeshDesc.materialIndexStride			= meshDesc.materialIndexStride;
				triMeshDesc.materialIndices				= meshDesc.materialIndices;
				triMeshDesc.heightFieldVerticalAxis		= (NxHeightFieldAxis)meshDesc.heightFieldVerticalAxis;
				triMeshDesc.heightFieldVerticalExtent	= meshDesc.heightFieldVerticalExtent;
				triMeshDesc.convexEdgeThreshold			= meshDesc.convexEdgeThreshold;

				// cook mesh again to get new tree format
				status = gCooking->NxCookTriangleMesh(triMeshDesc, out_stream);
			}
			else if(meshType == MT_CONVEX_MESH)
			{
				NxConvexMeshDesc convMeshDesc;

				convMeshDesc.numVertices				= meshDesc.numVertices;
				convMeshDesc.numTriangles			 	= meshDesc.numTriangles;
				convMeshDesc.pointStrideBytes			= meshDesc.pointStrideBytes;
				convMeshDesc.triangleStrideBytes		= meshDesc.triangleStrideBytes;
				convMeshDesc.points						= meshDesc.points;
				convMeshDesc.triangles					= meshDesc.triangles;
				convMeshDesc.flags						= meshDesc.flags;

				// cook mesh again to get new tree format
				status = gCooking->NxCookConvexMesh(convMeshDesc, out_stream);
			}
		}
		else
		{
			printf("Error: Could not open file \"%s\" for writing.\n", new_file);
		}
	}

	return status;
}


bool runConverter(const char* source, const char* dest)
{
	bool status = false;

#ifdef WIN32
	if (isFile(source))
    {
#endif
        // single file to convert
		status = convertMeshFile(source, dest);
#ifdef WIN32
    }
    else
    {
        // convert all files in a folder
        char dir_search_path[text_buf_size];
        char dir_path_in[text_buf_size];
        char dir_path_out[text_buf_size];
        strcpy(dir_search_path, source);
        strcpy(dir_path_in, source);
        strcpy(dir_path_out, dest);

        getDir(dir_path_in, true);
        int len_in  = strlen(dir_path_in);
        int len_out = strlen(dir_path_out);
        if ((dir_path_out[len_out - 1] == '\\') || (dir_path_out[len_out - 1] == '/'))
        {
            dir_path_out[len_out - 1] = '\0';
            len_out--;
        }

        if (isDir(dir_path_in) && isDir(dir_path_out))
        {
            dir_path_out[len_out]     = '\\';  // no '/' ???
            dir_path_out[len_out + 1] = '\0';
            dir_path_in[len_in]       = '\\';  // no '/' ???
            dir_path_in[len_in + 1]   = '\0';
            len_in++;

            char file_name_in[text_buf_size];
            char file_name_out[text_buf_size];
            HANDLE sHandle = fileSearchGetNext(NULL, dir_search_path, file_name_in);
            if (sHandle == NULL)
            {
				printf("Error: Invalid parameters.\n");
                //printUsage(argv[0]);
            }
            else
            {
                while (sHandle)
                {
                    printf("\nConverting file \"%s\" ...\n", file_name_in);
                    strcpy(file_name_out, dir_path_out);
                    strcat(file_name_out, file_name_in);
                    strcpy(&dir_path_in[len_in], file_name_in);

					if (convertMeshFile(dir_path_in, file_name_out))
						status = true;
					else
                        printf("Error: Mesh convertion failed.\n");
                    
                    sHandle = fileSearchGetNext(sHandle, dir_search_path, file_name_in);
                }
            }
        }
        else
        {
			if (!isDir(source))
                printf("Error: Specified input file/directory is invalid.\n");
			else if (!isDir(dir_path_out))
                printf("Error: Specified output directory is invalid.\n");
            else
                printf("Error: Invalid parameters.\n");
            //printUsage(argv[0]);
        }
    }
#endif // WIN32

	return status;
}


bool convertCookedMesh(const char* target, const char* source, const char* dest)
{
	// Init cooking lib
    gCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
    if (gCooking == NULL)
    {
		printf("Error: Cooking library could not be loaded.\n");
        return false;
    }
    gCooking->NxInitCooking();

	// Get target platform
	NxPlatform targetPlatform;

	if (_stricmp(target, "PC") == 0)
	{
		targetPlatform = PLATFORM_PC;
	}
	else if (_stricmp(target, "XENON") == 0)
	{
		targetPlatform = PLATFORM_XENON;
	}
	else if (_stricmp(target, "PS3") == 0)
	{
		targetPlatform = PLATFORM_PLAYSTATION3;
	}
	else
	{
		printf("Error: Unknown target platform.\n");
		//printUsage(argv[0]);
		gCooking->NxCloseCooking();
		return false;
	}

	// Set cooking params
	NxCookingParams cookParams = gCooking->NxGetCookingParams();
	cookParams.targetPlatform = targetPlatform;
	gCooking->NxSetCookingParams(cookParams);

	bool success = runConverter(source, dest);

	gCooking->NxCloseCooking();

	return success;
}


void printUsage(const char* exec_name)
{
    printf("\n=================================================================\n\n"
           "Usage (1):\n\n%s platform f_old f_new\n\n"
		   "  platform:   target platform (PC, XENON or PS3)\n"
           "  f_old:      file with cooked mesh in the old format\n"
           "  f_new:      file to store the cooked mesh with the new format\n\n"
#ifdef WIN32
           "Usage (2):\n\n%s platform dir_old dir_new\n\n"
		   "  platform:   target platform (PC, XENON or PS3)\n"
           "  dir_old:    directory with cooked mesh files in the old format plus a file extension classifier\n"
           "              Ex: C:\\OldDir\\*.bin  or  C:\\OldDir\\*.*\n"
           "  dir_new:    directory to store the cooked mesh files with the new format\n\n"
#endif
           "=================================================================\n", exec_name, exec_name);
}


int main(int argc, char** argv)
{
	bool success = false;

	/*char** old_argv = argv;
    argv = new char*[4];
    argv[0] = old_argv[0];
	argv[1] = "PC";
    //
    //argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_old\\prison1.bin";
    //argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\prison1.bin";
	//
	//argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_old\\convex_SDK_2_4_1.bin";
    //argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\convex_new.bin";
    //
    // overwrite old file should work
    //argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\prison1.bin";
    //argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\prison1.bin";
    //
	// convert certain files from one folder and store them in another folder
    //argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_old\\*.bin";
    //argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\";
    //
    argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_old\\*.bin";
    argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new";
    //
	// convert all files from one folder and store them in another folder
    //argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_old\\*.*";
    //argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\";
    //
    // overwrite all files in a folder should work
    //argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\*.*";
    //argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\";
    //
    // error
    //argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_old\\";
    //argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\";
    //
    // error
    //argv[2] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_old\\";
    //argv[3] = "D:::";
    //
    // error
    //argv[2] = "D:::";
    //argv[3] = "D:\\Programs\\Test\\SLTreeConverter\\data\\data_new\\";
    //
    argc = 4;*/

    // check parameters and convert mesh files
    if (argc == 4)
    {
		success = convertCookedMesh(argv[1], argv[2], argv[3]);
    }
	else
	{
		printf("Error: Wrong number of parameters.\n");
		printUsage(argv[0]);
	}

	if (!success)
    {
		printf("Error: Mesh convertion failed.\n");
    }

	
	return 0;
}
