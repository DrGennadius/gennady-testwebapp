#include <nan.h>
#include <string>
#include <lm.h>
//#include <io.h>
#pragma comment(lib,"netapi32.lib")

using namespace std;

void userCheck(const Nan::FunctionCallbackInfo<v8::Value>& info) {

	//SetConsoleOutputCP(CP_UTF8);
	//_setmode(_fileno(stdout), _O_U8TEXT);

	if (info.Length() < 1)
	{
		Nan::ThrowTypeError("Wrong number of arguments");
		return;
	}

	if (!info[0]->IsString())
	{
		Nan::ThrowTypeError("Wrong arguments");
		return;
	}
	
	v8::String::Utf8Value param1(info[0]->ToString());
	char* str = *param1;
	//fprintf(stderr, "Selected user name: %s.\nSearch user name:\n", str);
	bool uexists = false;

	LPUSER_INFO_0 pBuf = NULL;
	LPUSER_INFO_0 pTmpBuf;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	DWORD dwResumeHandle = 0;
	NET_API_STATUS nStatus;
	LPCWSTR pszServerName = NULL;

	nStatus = NetUserEnum(pszServerName,
		dwLevel,
		FILTER_NORMAL_ACCOUNT,
		(LPBYTE*)&pBuf,
		dwPrefMaxLen,
		&dwEntriesRead,
		&dwTotalEntries,
		&dwResumeHandle);
	if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
	{
		if ((pTmpBuf = pBuf) != NULL)
		{
			for (DWORD i = 0; (i < dwEntriesRead); i++)
			{
				if (pTmpBuf == NULL) break;
				int length = WideCharToMultiByte(CP_UTF8, 0, pTmpBuf->usri0_name, -1, 0, 0, NULL, NULL);
				char* str2 = new char[length];
				WideCharToMultiByte(CP_UTF8, 0, pTmpBuf->usri0_name, -1, str2 , length, NULL, NULL);
				//fprintf(stderr, "%d. %s\n", i+1, str2);
				if ((string)str == (string)str2)
				{
					uexists = true;
					break;
				}
				delete[] str2;
				pTmpBuf++;
			}
		}
	}
 
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);

	v8::Local<v8::Boolean> v8b = Nan::New(uexists);
	info.GetReturnValue().Set(v8b);
}

void Init(v8::Local<v8::Object> exports) {
	exports->Set(Nan::New("usercheck").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(userCheck)->GetFunction());
}

NODE_MODULE(usercheck, Init)