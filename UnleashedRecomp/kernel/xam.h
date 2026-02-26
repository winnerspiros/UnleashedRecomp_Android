#pragma once
#include <xbox.h>

#define MSGID(Area, Number) (uint32_t)((uint16_t)(Area) << 16 | (uint16_t)(Number))
#define MSG_AREA(msgid)     (((msgid) >> 16) & 0xFFFF)
#define MSG_NUMBER(msgid)   ((msgid) & 0xFFFF)

XCONTENT_DATA XamMakeContent(uint32_t type, const std::string_view& name);
void XamRegisterContent(const XCONTENT_DATA& data, const std::string_view& root);

std::string_view XamGetRootPath(const std::string_view& root);
void XamRootCreate(const std::string_view& root, const std::string_view& path);

uint32_t XamNotifyCreateListener(uint64_t qwAreas);
void XamNotifyEnqueueEvent(uint32_t dwId, uint32_t dwParam); // i made it the fuck up
bool XNotifyGetNext(uint32_t hNotification, uint32_t dwMsgFilter, be<uint32_t>* pdwId, be<uint32_t>* pParam);

uint32_t XamShowMessageBoxUI(uint32_t dwUserIndex, be<uint16_t>* wszTitle, be<uint16_t>* wszText, uint32_t cButtons,
    xpointer<be<uint16_t>>* pwszButtons, uint32_t dwFocusButton, uint32_t dwFlags, be<uint32_t>* pResult, XXOVERLAPPED* pOverlapped);

uint32_t XamContentCreateEnumerator(uint32_t dwUserIndex, uint32_t DeviceID, uint32_t dwContentType,
    uint32_t dwContentFlags, uint32_t cItem, be<uint32_t>* pcbBuffer, be<uint32_t>* phEnum);
uint32_t XamEnumerate(uint32_t hEnum, uint32_t dwFlags, void* pvBuffer, uint32_t cbBuffer, be<uint32_t>* pcItemsReturned, XXOVERLAPPED* pOverlapped);

uint32_t XamContentCreateEx(uint32_t dwUserIndex, const char* szRootName, const XCONTENT_DATA* pContentData,
    uint32_t dwContentFlags, be<uint32_t>* pdwDisposition, be<uint32_t>* pdwLicenseMask,
    uint32_t dwFileCacheSize, uint64_t uliContentSize, PXXOVERLAPPED pOverlapped);
uint32_t XamContentGetDeviceData(uint32_t DeviceID, XDEVICE_DATA* pDeviceData);
uint32_t XamContentClose(const char* szRootName, XXOVERLAPPED* pOverlapped);

uint32_t XamInputGetCapabilities(uint32_t unk, uint32_t userIndex, uint32_t flags, XAMINPUT_CAPABILITIES* caps);
uint32_t XamInputGetState(uint32_t userIndex, uint32_t flags, XAMINPUT_STATE* state);
uint32_t XamInputSetState(uint32_t userIndex, uint32_t flags, XAMINPUT_VIBRATION* vibration);

uint32_t XamUserGetSigninState(uint32_t userIndex);
uint32_t XamGetSystemVersion();
void XamContentDelete();
uint32_t XamContentGetCreator(uint32_t userIndex, const XCONTENT_DATA* contentData, be<uint32_t>* isCreator, be<uint64_t>* xuid, XXOVERLAPPED* overlapped);
uint32_t XamContentGetDeviceState();
uint32_t XamUserGetSigninInfo(uint32_t userIndex, uint32_t flags, XUSER_SIGNIN_INFO* info);
void XamShowSigninUI();
uint32_t XamShowDeviceSelectorUI(uint32_t userIndex, uint32_t contentType, uint32_t contentFlags, uint64_t totalRequested, be<uint32_t>* deviceId, XXOVERLAPPED* overlapped);
void XamShowDirtyDiscErrorUI();
void XamEnableInactivityProcessing();
void XamResetInactivity();
void XamShowMessageBoxUIEx();
void XamLoaderTerminateTitle();
void XamGetExecutionId();
void XamLoaderLaunchTitle();
void XamUserReadProfileSettings(uint32_t titleId, uint32_t userIndex, uint32_t xuidCount, uint64_t* xuids, uint32_t settingCount, uint32_t* settingIds, be<uint32_t>* bufferSize, void* buffer, void* overlapped);
void StfsControlDevice();
void StfsCreateDevice();
