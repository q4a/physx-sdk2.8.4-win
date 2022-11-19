/*
 *  FileAndDirUtil.cpp
 * 
 *  Author: Michael Sauter
 *
 *  Content: Tools for directory structure handling and file processing.
 *
 */

#include "FileAndDirUtil.h"


#ifdef WIN32


/**
 *
 * Check whether the given file really exists
 *
 * @param      filename     file path
 *
 * @return                  true if file exists else false
 *
 */
bool isFile(const char* filename)
{
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	FILE* fp;
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


	assert(filename != NULL);
    
    fp = fopen(filename, "r");
    if (fp)
    {
        fclose(fp);
        return true;
    }
    else
        return false;
}


/**
 *
 * Check whether the given directory really exists
 *
 * @param      dirpath      directory path
 *
 * @return                  true if directory exists else false
 *
 */
bool isDir(const char* dirpath)
{
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;
    bool found;
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


	assert(dirpath != NULL);

	searchHandle = FindFirstFile(dirpath, &fileData);

    found = ((searchHandle != INVALID_HANDLE_VALUE) && ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY));
    
    // end search
    FindClose(searchHandle);
    
    return found;
}


/**
 *
 * Extract directory from a file path
 *
 * @param      path         file path to get the directory from
 * @param      cutLastSlash specifies whether you want to keep the slash/backslash at the end of the directory path or not
 *
 * @return                  true if successful else false
 *
 */
bool getDir(char* path, bool cutLastSlash)
{
    char* ptr = strrchr(path, '\\');
    if (ptr == NULL)
        char* ptr = strrchr(path, '/');

    if (ptr != NULL)
    {
        if (!cutLastSlash)
            ptr++;

        *ptr = '\0';
        return true;
    }
    else
        return false;
}


/*
 *
 * Initialize search of files with a certain extension in a directory
 *
 */
/*HANDLE initFileSearch(const char* path, char* file_name)
{
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	HANDLE searchHandle;
    HANDLE returnHandle = NULL;
	WIN32_FIND_DATA fileData;
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	assert(path != NULL);
    assert(file_name != NULL);

	searchHandle = FindFirstFile(path, &fileData);
	if (searchHandle != INVALID_HANDLE_VALUE)
	{
        if (fileData.cFileName != NULL)
        {
            strcpy(file_name, fileData.cFileName);
            returnHandle = searchHandle;
        }
    }
    
    return returnHandle;
}*/


/*
 *
 * get next search result for an initialized file search
 *
 */
/*bool fileSearchGetNext(HANDLE searchHandle, char* file_name)
{
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	WIN32_FIND_DATA fileData;
    bool success = false;
    BOOL status;
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	assert(searchHandle != NULL);
    assert(searchHandle != INVALID_HANDLE_VALUE);
    assert(file_name != NULL);

    status = FindNextFile(searchHandle, &fileData);

    while((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
            status = FindNextFile(searchHandle, &fileData);

    if (status)
    {
        if (fileData.cFileName != NULL)
        {
            strcpy(file_name, fileData.cFileName);
            success = true;
        }
    }
    else
    {
        // end search
        FindClose(searchHandle);
    }

    return success;
}*/


/**
 *
 * Search for files with a certain extension in a given directory
 *
 * @param      searchHandle handle for the file search (pass NULL to initialize search)
 * @param      path         directory path to search for files and file type (this parameter is only needed when the search is initialized)
 *                          Ex.: "C:\MyDir\*.exe" or "C:\MyDir\*.*"
 * @param      file_name    buffer to return the name of the next file in the search directory (make sure the buffer is large enough)
 *
 * @return                  handle for the file search if successful, NULL if no more results available
 *
 */
HANDLE fileSearchGetNext(HANDLE searchHandle, const char* path, char* file_name)
{
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	WIN32_FIND_DATA fileData;
    BOOL status;
    HANDLE retHandle = NULL;
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    assert(searchHandle != INVALID_HANDLE_VALUE);
    assert(path != NULL);
    assert(file_name != NULL);

    if (searchHandle == NULL)   // start search
    {
        searchHandle = FindFirstFile(path, &fileData);
	    if (searchHandle == INVALID_HANDLE_VALUE)
	    {
            return NULL;
        }
        status = TRUE;
    }
    else
        status = FindNextFile(searchHandle, &fileData);

    // do not process directories (directories are returned if the file extension is specified as *.*)
    while(((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) && (status))
            status = FindNextFile(searchHandle, &fileData);

    if (status)
    {
        if (fileData.cFileName != NULL)
        {
            strcpy(file_name, fileData.cFileName);
            retHandle = searchHandle;
        }
    }
    else
    {
        // end search
        FindClose(searchHandle);
    }

    return retHandle;
}


#endif // WIN32
