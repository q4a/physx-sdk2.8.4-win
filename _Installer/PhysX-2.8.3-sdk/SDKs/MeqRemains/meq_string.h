/*************************************************************************
*
* Copyright 2010 NVIDIA Corporation
*
*			www.nvidia.com
*
************************************************************************/

#ifndef MEQ_STRING_H
#define MEQ_STRING_H

/*#include "meq_config.h"
#include "meq_memorymanager.h"
#include "meq_debugstream.h"*/

#include <ctype.h>

#include <string.h>


namespace meq {
	const int MEQ_INTERNAL_STRING_SIZE = 16;

	class /*MEQ_PUBLIC_CLASS NXF_DLL_EXPORT*/ String {
	public:
		//MEQ_DECLARE_MMOBJECT(String);

		String() {
			length = MEQ_INTERNAL_STRING_SIZE - 1;
			str=NULL;
			m_internalData[0] = '\0';
			strLength = 0;
		}

		String(const String& string) {
			length = MEQ_INTERNAL_STRING_SIZE - 1;
			str=NULL;
			m_internalData[0] = '\0';
			strLength = 0;
			assign(string);
		}

		String(const char * string) {
			length = MEQ_INTERNAL_STRING_SIZE - 1;
			str=NULL;
			m_internalData[0] = '\0';
			strLength = 0;
			assign(string);
		}


		String(char ch) {
			length = MEQ_INTERNAL_STRING_SIZE - 1;
			str=NULL;
			m_internalData[0]=ch;
			m_internalData[1]='\0';
			strLength = 1;
		}

		~String() {
			// @@@ Not using allocator (it's temporary anyway)
			if(str!=NULL) delete str;
		}

		String& operator=(const char* string) {
			assign(string);
			return *this;
		}

		String& operator=(const String& string) {
			assign(string);
			return *this;
		}

		bool operator ==(const String& string) const {
			return compareTo(string)==0;
		}

		bool operator ==(const char* string) const {
			return compareTo(string)==0;
		}

		bool operator !=(const String& string) const {
			return compareTo(string)!=0;
		}

		bool operator !=(const char* string) const {
			return compareTo(string)!=0;
		}

		String& operator +=(const String& string) {
			strLength += string.strLength;
			grow(strLength);
			char* strData = str!=NULL?str:m_internalData;
			strcat(strData, string.getArray());
			return *this;
		}

		String& operator +=(const char* string) {
			strLength += (int)strlen(string);
			grow(strLength);
			char* strData = str!=NULL?str:m_internalData;
			strcat(strData, string);
			return *this;
		}

		String& operator +=(char ch) {
			grow(strLength+1);
			char* strData = str!=NULL?str:m_internalData;
			strData[strLength]=ch;
			strData[strLength+1]='\0';
			strLength++;
			return *this;
		}

		void append(const char* string){
			*this += string;
		}

		void append(const char* string, int l){
			int len=(int)strlen(string);
			if (l<len) len = l;
			strLength += len;
			grow(strLength);
			char* strData = str!=NULL?str:m_internalData;
			strncat(strData, string, len);
		}

		void append(const String& string) {
			*this += string;
		}

		void append(const String& string, int length) {
			append(string.getArray(), length);
		}

		void trim(){
			char* strData = str!=NULL?str:m_internalData;
			//Find beginning
			int i = 0;
			while(strData[i]==' ' && strData[i]!='\0') i++;
			//Find end
			int j = i;
			while(strData[j]!='\0') j++;
			while((strData[j]==' ' || strData[j]=='\0') && j>i) j--;
			//Terminate at end
			strData[j+1]='\0';
			//Create tmp string and assign
			String tmp(strData+i);
			assign(tmp);
		}

		const char* getArray() const { 
			return str!=NULL?str:m_internalData; 
		}

		char& operator[](int n) {
			char* strData = str!=NULL?str:m_internalData;
			return strData[n];
		}

		char operator[](int n) const {
			const char* strData = str!=NULL?str:m_internalData;
			return strData[n];
		}

		int getLength() const {
			return strLength;
		}

		int compareTo(const String& string) const {
			const char* strData = str!=NULL?str:m_internalData;
			return strcmp(strData,string.getArray());
		}

		int compareTo(const char* string) const {
			const char* strData = str!=NULL?str:m_internalData;
			return strcmp(strData,string);
		}

		bool beginsWith(const String& string) const {
			const char* strData = str!=NULL?str:m_internalData;
			return strncmp(strData, string.getArray(), string.strLength)==0;
		}

		bool beginsWith(const char* string) const {
			const char* strData = str!=NULL?str:m_internalData;
			return strncmp(strData, string, strlen(string))==0;
		}

		bool endsWith(const String& string) const {
			const char* strData = str!=NULL?str:m_internalData;
			int diff=(int)(strLength - string.strLength);
			return diff>=0 && strcmp(strData + diff,string.getArray())==0;
		}

		bool endsWith(const char* string) const {
			const char* strData = str!=NULL?str:m_internalData;
			int diff=(int)(strLength-strlen(string));
			return diff>=0 && strcmp(strData+diff, string)==0;
		}

		bool isEmpty() const {
			const char* strData = str!=NULL?str:m_internalData;
			return strData[0]=='\0';
		}

		bool contains(const String& string) const {
			const char* strData = str!=NULL?str:m_internalData;
			return strstr(strData, string.getArray())!=NULL;
		}

		bool contains(const char* string) const {
			const char* strData = str!=NULL?str:m_internalData;
			return strstr(strData, string)!=NULL;
		}


		bool replace(const char* search, const char* replace) {
			if(!contains(search)) return false;
			String result;

			int pos=0, lastPos=0;
			int l = (int)strlen(replace);

			char* strData = str!=NULL?str:m_internalData;
			while( (pos=(int)(strstr(strData+lastPos, search)-(strData)))>=0) {
				//append data before
				if(pos!=lastPos) {
					result.append(strData+lastPos, (pos-lastPos));
				}

				//replace and update lastPos
				result.append(replace);
				lastPos=pos+l;
			}

			*this = result;
			return true;
		}

	protected:
		char m_internalData[MEQ_INTERNAL_STRING_SIZE];
		char* str;
		int length;
		int strLength;

		void grow(int size) {
			if (size <= length) return;
			// @@@ Not using allocator (it's temporary anyway)
			char *tmp= new char[size * 2 + 1];
			memset(tmp, 0, size * 2 + 1);
			if (str != NULL)
				strcpy(tmp, str);
			else
				strcpy(tmp, m_internalData);
			// @@@ Not using deallocator (it's temporary anyway)
			if (str != NULL) delete str;
			str=tmp;
			length=size * 2;
		}

		void assign(const char * s) {
			if(s==NULL) s="";
			strLength = (int)strlen(s);
			grow(strLength);
			char* strData = str!=NULL?str:m_internalData;
			strcpy(strData, s);
		}

		void assign(const String& s) {
			strLength = s.strLength;
			grow(strLength);
			char* strData = str!=NULL?str:m_internalData;
			strcpy(strData, s.getArray());
		}
	};

	/*MEQ_PUBLIC_METHOD String operator+(const char* string1, const String& string2);
	MEQ_PUBLIC_METHOD String operator+(const String& string1, const char* string2);
	MEQ_PUBLIC_METHOD DebugStream & operator<< (DebugStream & stream, const String & s);*/
}

#endif
