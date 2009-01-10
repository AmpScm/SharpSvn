//#include "SharpSvn-Plink.h"
#include "winsock2.h"
#include "windows.h"
#include "WinCred.h"
#include "Tlhelp32.h"
#pragma comment(lib, "credui.lib")

#define verify_ssh_host_key putty_verify_ssh_host_key
#define console_get_userpass_input putty_console_get_userpass_input
#define cleanup_exit putty_cleanup_exit

#include "PuttySrc/Windows/wincons.c"
#undef verify_ssh_host_key
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

	if (!IsWindow(ownerHwnd))
		return ownerHwnd;

	if (!IsWindowEnabled(ownerHwnd))
	{	
		// Parent window is disabled; it probably has a dialog open
		// TODO: Find the active owned popup window

		// HACK: Check if the foreground window is the right window
		HWND hFg = GetForegroundWindow();

		if (IsWindow(hFg) && hFg != ownerHwnd)
		{
			// Check if the foreground window is of the right process
			DWORD thrOwner = GetWindowThreadProcessId(ownerHwnd, NULL);
			DWORD pidFg;
			DWORD thrFg = GetWindowThreadProcessId(hFg, &pidFg);
			
			if ((pidFg == parentPid) && (thrOwner != 0) && (thrFg == thrOwner))
				return ownerHwnd = hFg; // Return the active window instead
		}
	}

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

int verify_ssh_host_key(void *frontend, char *host, int port, char *keytype,
						char *keystr, char *fingerprint,
						void (*callback)(void *ctx, int result), void *ctx)
{
	int ret;

	static const char absentmsg[] =
		"The server's host key is not cached in the registry. You\n"
		"have no guarantee that the server is the computer you\n"
		"think it is.\n\n"
		"The server's %s key fingerprint is:\n"
		"%s\n\n"
		"If you trust this host, enter \"y\" to add the key to\n"
		"PuTTY's cache and carry on connecting.\n\n"
		"If you want to carry on connecting just once, without\n"
		"adding the key to the cache, enter \"n\".\n\n"
		"If you do not trust this host, press Return to abandon the\n"
		"connection.\n\n"
		"Store key in cache? (y/n) ";

	static const char wrongmsg[] =
		"WARNING - POTENTIAL SECURITY BREACH!\n"
		"The server's host key does not match the one PuTTY has\n"
		"cached in the registry. This means that either the\n"
		"server administrator has changed the host key, or you\n"
		"have actually connected to another computer pretending\n"
		"to be the server.\n\n"
		"The new %s key fingerprint is:\n"
		"%s\n\n"
		"If you were expecting this change and trust the new key,\n"
		"enter \"y\" to update PuTTY's cache and continue connecting.\n\n"
		"If you want to carry on connecting but without updating\n"
		"the cache, enter \"n\".\n\n"
		"If you want to abandon the connection completely, press\n"
		"Return to cancel. Pressing Return is the ONLY guaranteed\n"
		"safe choice.\n\n"
		"Update cached key? (y/n, Return cancels connection)";

	char buffer[2048];
	int result;

	if(console_batch_mode)
		return putty_verify_ssh_host_key(frontend, host, port, keytype, keystr, fingerprint, callback, ctx);

	/*
	* Verify the key against the registry.
	*/
	ret = verify_host_key(host, port, keytype, keystr);

	if (ret == 0)		       /* success - key matched OK */
		return 1;

	if (ret == 2) {		       /* key was different */
		sprintf_s(buffer, sizeof(buffer), wrongmsg, keytype, fingerprint);
	}
	if (ret == 1) {		       /* key was absent */
		sprintf_s(buffer, sizeof(buffer), absentmsg, keytype, fingerprint);
	}

	result = MessageBox(GetOwnerHwnd(), buffer, (ret == 2) ? "SharpPlink - POTENTIAL SECURITY BREACH" : "SharpPlink - Unknown Host Key", 
		MB_YESNOCANCEL | ((ret == 2) ? (MB_ICONSTOP | MB_DEFBUTTON3) : MB_ICONINFORMATION));

	if(result == IDYES)
		store_host_key(host, port, keytype, keystr);

	if(result == IDYES || result == IDNO)
		return 1;

	return 0; // Abort
}

void cleanup_exit(int code)
{
	if(sPlinkCurrentConfig && sPlinkCurrentConfig->host)
	{		
		CredUIConfirmCredentials(sPlinkCurrentConfig->host, sPlinkShouldConfirm);
	}
	putty_cleanup_exit(code);
}
