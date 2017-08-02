#include <nan.h>
#include <string>
#include <lm.h>
#pragma comment(lib,"netapi32.lib")

using namespace std;

void userCheck(const Nan::FunctionCallbackInfo<v8::Value>& info) {

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
	string str = string(*param1);
	char* str2 = NULL;
	bool uexists = false;

	LPUSER_INFO_0 pBuf = NULL; // 
	LPUSER_INFO_0 pTmpBuf;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	DWORD dwResumeHandle = 0;
	NET_API_STATUS nStatus;
	LPCWSTR pszServerName = NULL;

	do // L1
    {
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
					// Проверка доступа
					if (pTmpBuf == NULL) break;
					int length = WideCharToMultiByte(CP_UTF8, 0, pTmpBuf->usri0_name, -1, 0, 0, NULL, NULL);
					str2 = new char[length];
					WideCharToMultiByte(CP_UTF8, 0, pTmpBuf->usri0_name, -1, str2 , length, NULL, NULL);
					if (str == (string)str2)
					{
						uexists = true;
						break;
					}
					pTmpBuf++;
				}
				// Если пользователь найден, завершаем цикл L1
				if (uexists) break;
			}
		}
	
		if (pBuf != NULL)
		{
			NetApiBufferFree(pBuf);
			pBuf = NULL;
		}
	}
	// Вызывать NetUserEnum, пока есть больше доступных записей
	while (nStatus == ERROR_MORE_DATA);
	
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);
	if (str2 != NULL)
		delete[] str2;

	v8::Local<v8::Boolean> v8b = Nan::New(uexists);
	info.GetReturnValue().Set(v8b);
}

void Init(v8::Local<v8::Object> exports) {
	exports->Set(Nan::New("usercheck").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(userCheck)->GetFunction());
}

NODE_MODULE(usercheck, Init)