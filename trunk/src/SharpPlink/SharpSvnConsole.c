//#include "SharpSvn-Plink.h"
#include "winsock2.h"
#include "windows.h"
#include "WinCred.h"
#include "Tlhelp32.h"
#pragma comment(lib, "credui.lib")

#define console_get_userpass_input putty_console_get_userpass_input
#define cleanup_exit putty_cleanup_exit

#include "PuttySrc/Windows/wincons.c"
#undef console_get_userpass_input
#undef cleanup_exit


#ifndef MIN
#define MIN(x, y) ((x < y) ? x : y)
#endif

extern Config* sPlinkCurrentConfig ;
static char userName[CREDUI_MAX_USERNAME_LENGTH+1] = "";
static char password[CREDUI_MAX_PASSWORD_LENGTH+1] = "";
static BOOL keptForNext = FALSE;
static BOOL sPlinkShouldConfirm = FALSE;
static DWORD parentPid = 0;
static HWND ownerHwnd = NULL;

BOOL CALLBACK sPFindOwnerWalker(HWND hwnd, LPARAM lParam)
{
	DWORD pid;
	if(!GetWindowThreadProcessId(hwnd, &pid))
		return TRUE; // Continue

	if(pid != parentPid)
		return TRUE; // Continue

	if(GetWindow(hwnd, GW_OWNER)) // Window has an owner
		return TRUE; // Continue

	if(IsWindowVisible(hwnd))
	{
		ownerHwnd = hwnd;
		return FALSE; // Break
	}

	return TRUE;
}


HWND GetOwnerHwnd()
{
	HANDLE hSnap;
	PROCESSENTRY32 pe32;
	DWORD myPid;
	static BOOL calculatedOwner = FALSE;

	if(ownerHwnd && IsWindow(ownerHwnd))
		return ownerHwnd;
	else if(calculatedOwner)
		return NULL; // Just use desktop

	ownerHwnd = NULL;
	calculatedOwner = TRUE;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hSnap == INVALID_HANDLE_VALUE)
		return NULL; // Snapshot failed

	// Find ourselves
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if(TRUE != Process32First(hSnap, &pe32))
	{
		CloseHandle(hSnap);
		return NULL;
	}

	myPid = GetCurrentProcessId();

	do
	{
		if(myPid == pe32.th32ProcessID)
			break;
	}
	while(TRUE == Process32Next(hSnap, &pe32));
	CloseHandle(hSnap);

	if(myPid != pe32.th32ProcessID)
		return NULL;

	parentPid = pe32.th32ParentProcessID;

	EnumWindows(sPFindOwnerWalker, 0);


	return ownerHwnd;
}

int console_get_userpass_input(prompts_t *p, unsigned char *in, int inlen)
{
	CREDUI_INFO info;
	BOOL bSave = 0;
	DWORD dwResult;
	char captionBuffer[256];
	char messageBuffer[256];
	const char* host = sPlinkCurrentConfig ? sPlinkCurrentConfig->host : NULL;
	static int nRun;

	if(!host)
		host = "localhost";

	if(keptForNext && !p->prompts[0]->echo)
	{
		keptForNext = FALSE;
		strcpy_s(p->prompts[0]->result, MIN(p->prompts[0]->result_len, CREDUI_MAX_PASSWORD_LENGTH), password);

		memset(password, 0, sizeof(password));
		return 1;
	}

	nRun++;
	sPlinkShouldConfirm = FALSE;	

	if(!userName[0] && sPlinkCurrentConfig && sPlinkCurrentConfig->username && sPlinkCurrentConfig->username[0])
		strcpy_s(userName, sizeof(userName), sPlinkCurrentConfig->username);

	memset(password, 0, sizeof(password));
	memset(&info, 0, sizeof(info));
	sprintf_s(captionBuffer, sizeof(captionBuffer), "Connect to %s", host);
	sprintf_s(messageBuffer, sizeof(messageBuffer), "Connecting to %s with ssh:", host);

	info.cbSize = sizeof(info);
	info.hwndParent = GetOwnerHwnd();
	info.pszCaptionText = captionBuffer;
	info.pszMessageText = messageBuffer;

	dwResult = CredUIPromptForCredentials(
		&info, 
		host, 
		NULL, 
		nRun > 1 ? ERROR_LOGON_FAILURE : 0, 
		userName, CREDUI_MAX_USERNAME_LENGTH, 
		password, CREDUI_MAX_PASSWORD_LENGTH, 
		&bSave,
		CREDUI_FLAGS_EXPECT_CONFIRMATION | CREDUI_FLAGS_GENERIC_CREDENTIALS | CREDUI_FLAGS_SHOW_SAVE_CHECK_BOX
		| CREDUI_FLAGS_ALWAYS_SHOW_UI
		| (p->prompts[0]->echo ? 0 : CREDUI_FLAGS_KEEP_USERNAME));

	if(p->prompts[0]->echo)
	{
		strcpy_s(p->prompts[0]->result, MIN(p->prompts[0]->result_len, CREDUI_MAX_USERNAME_LENGTH), userName);
		keptForNext = TRUE;
	}
	else
	{
		strcpy_s(p->prompts[0]->result, MIN(p->prompts[0]->result_len, CREDUI_MAX_PASSWORD_LENGTH), password);
		memset(password, 0, sizeof(password));
	}

	if(!dwResult)
	{
		sPlinkShouldConfirm = TRUE;
		return 1;
	}
	else
		return 0;
}

void cleanup_exit(int code)
{
	if(sPlinkCurrentConfig && sPlinkCurrentConfig->host)
	{		
		CredUIConfirmCredentials(sPlinkCurrentConfig->host, sPlinkShouldConfirm);
	}
	putty_cleanup_exit(code);
}
