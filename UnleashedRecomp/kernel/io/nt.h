#pragma once
#include <stdafx.h>
#include <kernel/xbox.h>

void NtOpenFile();
uint32_t NtCreateFile(be<uint32_t>* FileHandle, uint32_t DesiredAccess, XOBJECT_ATTRIBUTES* Attributes, XIO_STATUS_BLOCK* IoStatusBlock, uint64_t* AllocationSize, uint32_t FileAttributes, uint32_t ShareAccess, uint32_t CreateDisposition, uint32_t CreateOptions);
uint32_t NtClose(uint32_t handle);
void NtSetInformationFile();
uint32_t FscSetCacheElementCount();
void NtWriteFile();
void NtQueryInformationFile();
void NtQueryVolumeInformationFile();
void NtQueryDirectoryFile();
void NtReadFileScatter();
void NtReadFile();
void NtDuplicateObject();
void NtQueryFullAttributesFile();
void NtFlushBuffersFile();
void IoInvalidDeviceRequest();
void ObReferenceObject();
uint32_t ObReferenceObjectByHandle(uint32_t handle, uint32_t objectType, be<uint32_t>* object);
void ObDereferenceObject();
void IoCreateDevice();
void IoDeleteDevice();
void IoCompleteRequest();
void ObIsTitleObject();
void IoCheckShareAccess();
void IoSetShareAccess();
void IoRemoveShareAccess();
void ObDeleteSymbolicLink();
void ObCreateSymbolicLink();
