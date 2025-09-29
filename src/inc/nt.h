#pragma once

#include <windows.h>
#include <winternl.h>

// warning C4201: nonstandard extension used: nameless struct/union
#pragma warning( disable : 4201 )

#define STATUS_SUCCESS                       ((NTSTATUS)0x00000000L)

#define STATUS_OBJECT_NAME_EXISTS            ((NTSTATUS)0x40000000L)

#define STATUS_BUFFER_OVERFLOW               ((NTSTATUS)0x80000005L)
#define STATUS_NO_MORE_ENTRIES               ((NTSTATUS)0x8000001AL)

#define STATUS_UNSUCCESSFUL                  ((NTSTATUS)0xC0000001L)
#define STATUS_INFO_LENGTH_MISMATCH          ((NTSTATUS)0xC0000004L)
#define STATUS_NO_SUCH_FILE                  ((NTSTATUS)0xC000000FL)
#define STATUS_INVALID_DEVICE_REQUEST        ((NTSTATUS)0xC0000010L)
#define STATUS_ACCESS_DENIED                 ((NTSTATUS)0xC0000022L)
#define STATUS_BUFFER_TOO_SMALL              ((NTSTATUS)0xC0000023L)
#define STATUS_OBJECT_TYPE_MISMATCH          ((NTSTATUS)0xC0000024L)
#define STATUS_OBJECT_NAME_INVALID           ((NTSTATUS)0xC0000033L)
#define STATUS_OBJECT_NAME_NOT_FOUND         ((NTSTATUS)0xC0000034L)
#define STATUS_OBJECT_PATH_NOT_FOUND         ((NTSTATUS)0xC000003AL)
#define STATUS_OBJECT_NAME_COLLISION         ((NTSTATUS)0xC0000035L)
#define STATUS_PRIVILEGE_NOT_HELD            ((NTSTATUS)0xC0000061L)
#define STATUS_INSUFFICIENT_RESOURCES        ((NTSTATUS)0xC000009AL)     // ntsubauth
#define STATUS_MEMORY_NOT_ALLOCATED          ((NTSTATUS)0xC00000A0L)
#define STATUS_ILLEGAL_FUNCTION              ((NTSTATUS)0xC00000AFL)
#define STATUS_NOT_SUPPORTED                 ((NTSTATUS)0xC00000BBL)
#define STATUS_INVALID_PARAMETER_1           ((NTSTATUS)0xC00000EFL)
#define STATUS_INVALID_PARAMETER_2           ((NTSTATUS)0xC00000F0L)
#define STATUS_INVALID_PARAMETER_3           ((NTSTATUS)0xC00000F1L)
#define STATUS_INVALID_PARAMETER_4           ((NTSTATUS)0xC00000F2L)
#define STATUS_INVALID_PARAMETER_5           ((NTSTATUS)0xC00000F3L)
#define STATUS_INVALID_PARAMETER_6           ((NTSTATUS)0xC00000F4L)
#define STATUS_NOT_ALL_ASSIGNED              ((NTSTATUS)0x00000106L)
#define STATUS_IMAGE_ALREADY_LOADED          ((NTSTATUS)0xC000010EL)
#define STATUS_CANNOT_DELETE                 ((NTSTATUS)0xC0000121L)
#define STATUS_DATATYPE_MISALIGNMENT_ERROR   ((NTSTATUS)0xC00002C5L)
#define STATUS_IMAGE_CERT_REVOKED            ((NTSTATUS)0xC0000603L)
#define STATUS_ELEVATION_REQUIRED            ((NTSTATUS)0xC000042CL)



#define SE_LOAD_DRIVER_PRIVILEGE (10L)
#define SE_DEBUG_PRIVILEGE (20L)

//
// Directory Object Access Rights
//
#define DIRECTORY_QUERY                 (0x0001)
#define DIRECTORY_TRAVERSE              (0x0002)
#define DIRECTORY_CREATE_OBJECT         (0x0004)
#define DIRECTORY_CREATE_SUBDIRECTORY   (0x0008)
#define DIRECTORY_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xF)

//
//  RTL_CONTAINS_FIELD usage:
//
//      if (RTL_CONTAINS_FIELD(pBlock, pBlock->cbSize, dwMumble))
//          // safe to use pBlock->dwMumble
//
#define RTL_CONTAINS_FIELD(Struct, Size, Field) \
    ( (((PCHAR)(&(Struct)->Field)) + sizeof((Struct)->Field)) <= (((PCHAR)(Struct))+(Size)) )

//
// Return the number of elements in a statically sized array.
//   DWORD Buffer[100];
//   RTL_NUMBER_OF(Buffer) == 100
// This is also popularly known as: NUMBER_OF, ARRSIZE, _countof, NELEM, etc.
//
#define RTL_NUMBER_OF_V1(A) (sizeof(A)/sizeof((A)[0]))
#define RTL_NUMBER_OF(A) RTL_NUMBER_OF_V1(A)

#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)

#ifndef PAGE_SIZE
#define PAGE_SIZE (0x1000)
#endif

typedef LARGE_INTEGER PHYSICAL_ADDRESS;


#ifndef ALIGN_UP_TO_NEXT_BY
#define ALIGN_UP_TO_NEXT_BY(Address, Align) ( (((Align)-1)&((ULONG_PTR)(Address))) ? ( ((ULONG_PTR)(Address) + (Align) - 1) & ~((Align) - 1) ) : ((ULONG_PTR)(Address)+(Align)) )
#endif

#ifndef ALIGN_UP_BY
#define ALIGN_UP_BY(Address, Align) ( ((ULONG_PTR)(Address) + (Align) - 1) & ~((Align) - 1) )
#endif

#ifndef ALIGN_DOWN_BY
#define ALIGN_DOWN_BY(Address, Align) ((ULONG_PTR)(Address) & ~((ULONG_PTR)(Align) - 1))
#endif

#ifndef RtlOffsetToPointer
#define RtlOffsetToPointer(Base, Offset)  ((PCHAR)( ((PCHAR)(Base)) + ((ULONG_PTR)(Offset))  ))
#endif

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))
#endif
#ifndef UFIELD_OFFSET
#define UFIELD_OFFSET(type, field)    ((DWORD)(LONG_PTR)&(((type *)0)->field))
#endif


#ifndef INTERFACE_TYPE
typedef enum _INTERFACE_TYPE {
    InterfaceTypeUndefined = -1,
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    ProcessorInternal,
    InternalPowerBus,
    PNPISABus,
    PNPBus,
    Vmcs,
    ACPIBus,
    MaximumInterfaceType
} INTERFACE_TYPE, * PINTERFACE_TYPE;
#endif

typedef struct _OBJECT_DIRECTORY_INFORMATION {
    UNICODE_STRING Name;
    UNICODE_STRING TypeName;
} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

typedef struct _MY_SYSTEM_BASIC_INFORMATION {
    ULONG Reserved;
    ULONG TimerResolution;
    ULONG PageSize;
    ULONG NumberOfPhysicalPages;
    ULONG LowestPhysicalPageNumber;
    ULONG HighestPhysicalPageNumber;
    ULONG AllocationGranularity;
    ULONG_PTR MinimumUserModeAddress;
    ULONG_PTR MaximumUserModeAddress;
    ULONG_PTR ActiveProcessorsAffinityMask;
    CCHAR NumberOfProcessors;
} MY_SYSTEM_BASIC_INFORMATION, *PMY_SYSTEM_BASIC_INFORMATION;

typedef struct _CURDIR {
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, *PCURDIR;

typedef struct _RTL_DRIVE_LETTER_CURDIR {
    USHORT Flags;
    USHORT Length;
    ULONG TimeStamp;
    STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

#define RTL_MAX_DRIVE_LETTERS 32

//typedef struct _MY_RTL_USER_PROCESS_PARAMETERS32 {
//    ULONG MaximumLength;
//    ULONG Length;
//
//    ULONG Flags;
//    ULONG DebugFlags;
//
//    WOW64_POINTER(HANDLE) ConsoleHandle;
//    ULONG ConsoleFlags;
//    WOW64_POINTER(HANDLE) StandardInput;
//    WOW64_POINTER(HANDLE) StandardOutput;
//    WOW64_POINTER(HANDLE) StandardError;
//
//    CURDIR32 CurrentDirectory;
//    UNICODE_STRING32 DllPath;
//    UNICODE_STRING32 ImagePathName;
//    UNICODE_STRING32 CommandLine;
//    WOW64_POINTER(PVOID) Environment;
//
//    ULONG StartingX;
//    ULONG StartingY;
//    ULONG CountX;
//    ULONG CountY;
//    ULONG CountCharsX;
//    ULONG CountCharsY;
//    ULONG FillAttribute;
//
//    ULONG WindowFlags;
//    ULONG ShowWindowFlags;
//    UNICODE_STRING32 WindowTitle;
//    UNICODE_STRING32 DesktopInfo;
//    UNICODE_STRING32 ShellInfo;
//    UNICODE_STRING32 RuntimeData;
//    RTL_DRIVE_LETTER_CURDIR32 CurrentDirectories[RTL_MAX_DRIVE_LETTERS];
//
//    ULONG EnvironmentSize;
//    ULONG EnvironmentVersion;
//} MY_RTL_USER_PROCESS_PARAMETERS32, *PMY_RTL_USER_PROCESS_PARAMETERS32;

typedef struct _MY_RTL_USER_PROCESS_PARAMETERS {
    ULONG MaximumLength;
    ULONG Length;

    ULONG Flags;
    ULONG DebugFlags;

    HANDLE ConsoleHandle;
    ULONG ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;

    CURDIR CurrentDirectory;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    PVOID Environment;

    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;

    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING WindowTitle;
    UNICODE_STRING DesktopInfo;
    UNICODE_STRING ShellInfo;
    UNICODE_STRING RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

    ULONG EnvironmentSize;
    ULONG EnvironmentVersion;
    PVOID PackageDependencyData; //8+
    ULONG ProcessGroupId;
    // ULONG LoaderThreads;
} MY_RTL_USER_PROCESS_PARAMETERS, *PMY_RTL_USER_PROCESS_PARAMETERS;

//struct _CLIENT_ID {
//    HANDLE UniqueProcess;
//    HANDLE UniqueThread;
//} CLIENT_ID, *PCLIENT_ID;
typedef struct _CLIENT_ID CLIENT_ID;
typedef struct _CLIENT_ID * PCLIENT_ID;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX {
    PVOID Object;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR HandleValue;
    ULONG GrantedAccess;
    USHORT CreatorBackTraceIndex;
    USHORT ObjectTypeIndex;
    ULONG HandleAttributes;
    ULONG Reserved;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX {
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;




#ifndef RtlInitEmptyUnicodeString
#define RtlInitEmptyUnicodeString(_ucStr,_buf,_bufSize) \
    ((_ucStr)->Buffer = (_buf), \
     (_ucStr)->Length = 0, \
     (_ucStr)->MaximumLength = (USHORT)(_bufSize))
#endif


DECLSPEC_NORETURN
FORCEINLINE
VOID
RtlFailFast(
    _In_ ULONG Code
    )

{

    __fastfail(Code);
}

//typedef enum _SYSTEM_INFORMATION_CLASS {
//    SystemBasicInformation = 0,
//    SystemProcessorInformation = 1,
//    SystemPerformanceInformation = 2,
//    SystemTimeOfDayInformation = 3,
//    SystemPathInformation = 4,
//    SystemProcessInformation = 5,
//    SystemCallCountInformation = 6,
//    SystemDeviceInformation = 7,
//    SystemProcessorPerformanceInformation = 8,
//    SystemFlagsInformation = 9,
//    SystemCallTimeInformation = 10,
//    SystemModuleInformation = 11,
//    SystemLocksInformation = 12,
//    SystemStackTraceInformation = 13,
//    SystemPagedPoolInformation = 14,
//    SystemNonPagedPoolInformation = 15,
//    SystemHandleInformation = 16,
//    SystemObjectInformation = 17,
//    SystemPageFileInformation = 18,
//    SystemVdmInstemulInformation = 19,
//    SystemVdmBopInformation = 20,
//    SystemFileCacheInformation = 21,
//    SystemPoolTagInformation = 22,
//    SystemInterruptInformation = 23,
//    SystemDpcBehaviorInformation = 24,
//    SystemFullMemoryInformation = 25,
//    SystemLoadGdiDriverInformation = 26,
//    SystemUnloadGdiDriverInformation = 27,
//    SystemTimeAdjustmentInformation = 28,
//    SystemSummaryMemoryInformation = 29,
//    SystemMirrorMemoryInformation = 30,
//    SystemPerformanceTraceInformation = 31,
//    SystemObsolete0 = 32,
//    SystemExceptionInformation = 33,
//    SystemCrashDumpStateInformation = 34,
//    SystemKernelDebuggerInformation = 35,
//    SystemContextSwitchInformation = 36,
//    SystemRegistryQuotaInformation = 37,
//    SystemExtendServiceTableInformation = 38,
//    SystemPrioritySeperation = 39,
//    SystemVerifierAddDriverInformation = 40,
//    SystemVerifierRemoveDriverInformation = 41,
//    SystemProcessorIdleInformation = 42,
//    SystemLegacyDriverInformation = 43,
//    SystemCurrentTimeZoneInformation = 44,
//    SystemLookasideInformation = 45,
//    SystemTimeSlipNotification = 46,
//    SystemSessionCreate = 47,
//    SystemSessionDetach = 48,
//    SystemSessionInformation = 49,
//    SystemRangeStartInformation = 50,
//    SystemVerifierInformation = 51,
//    SystemVerifierThunkExtend = 52,
//    SystemSessionProcessInformation = 53,
//    SystemLoadGdiDriverInSystemSpace = 54,
//    SystemNumaProcessorMap = 55,
//    SystemPrefetcherInformation = 56,
//    SystemExtendedProcessInformation = 57,
//    SystemRecommendedSharedDataAlignment = 58,
//    SystemComPlusPackage = 59,
//    SystemNumaAvailableMemory = 60,
//    SystemProcessorPowerInformation = 61,
//    SystemEmulationBasicInformation = 62,
//    SystemEmulationProcessorInformation = 63,
#define SystemExtendedHandleInformation ((SYSTEM_INFORMATION_CLASS)64)
//    SystemLostDelayedWriteInformation = 65,
//    SystemBigPoolInformation = 66,
//    SystemSessionPoolTagInformation = 67,
//    SystemSessionMappedViewInformation = 68,
//    SystemHotpatchInformation = 69,
//    SystemObjectSecurityMode = 70,
//    SystemWatchdogTimerHandler = 71,
//    SystemWatchdogTimerInformation = 72,
//    SystemLogicalProcessorInformation = 73,
//    SystemWow64SharedInformationObsolete = 74,
//    SystemRegisterFirmwareTableInformationHandler = 75,
//    SystemFirmwareTableInformation = 76,
//    SystemModuleInformationEx = 77,
//    SystemVerifierTriageInformation = 78,
//    SystemSuperfetchInformation = 79,
//    SystemMemoryListInformation = 80,
//    SystemFileCacheInformationEx = 81,
//    SystemThreadPriorityClientIdInformation = 82,
//    SystemProcessorIdleCycleTimeInformation = 83,
//    SystemVerifierCancellationInformation = 84,
//    SystemProcessorPowerInformationEx = 85,
//    SystemRefTraceInformation = 86,
//    SystemSpecialPoolInformation = 87,
//    SystemProcessIdInformation = 88,
//    SystemErrorPortInformation = 89,
//    SystemBootEnvironmentInformation = 90,
//    SystemHypervisorInformation = 91,
//    SystemVerifierInformationEx = 92,
//    SystemTimeZoneInformation = 93,
//    SystemImageFileExecutionOptionsInformation = 94,
//    SystemCoverageInformation = 95,
//    SystemPrefetchPatchInformation = 96,
//    SystemVerifierFaultsInformation = 97,
//    SystemSystemPartitionInformation = 98,
//    SystemSystemDiskInformation = 99,
//    SystemProcessorPerformanceDistribution = 100,
//    SystemNumaProximityNodeInformation = 101,
//    SystemDynamicTimeZoneInformation = 102,
//    SystemCodeIntegrityInformation = 103,
//    SystemProcessorMicrocodeUpdateInformation = 104,
//    SystemProcessorBrandString = 105,
//    SystemVirtualAddressInformation = 106,
//    SystemLogicalProcessorAndGroupInformation = 107,
//    SystemProcessorCycleTimeInformation = 108,
//    SystemStoreInformation = 109,
//    SystemRegistryAppendString = 110,
//    SystemAitSamplingValue = 111,
//    SystemVhdBootInformation = 112,
//    SystemCpuQuotaInformation = 113,
//    SystemNativeBasicInformation = 114,
//    SystemErrorPortTimeouts = 115,
//    SystemLowPriorityIoInformation = 116,
//    SystemBootEntropyInformation = 117,
//    SystemVerifierCountersInformation = 118,
//    SystemPagedPoolInformationEx = 119,
//    SystemSystemPtesInformationEx = 120,
//    SystemNodeDistanceInformation = 121,
//    SystemAcpiAuditInformation = 122,
//    SystemBasicPerformanceInformation = 123,
//    SystemQueryPerformanceCounterInformation = 124,
//    SystemSessionBigPoolInformation = 125,
//    SystemBootGraphicsInformation = 126,
//    SystemScrubPhysicalMemoryInformation = 127,
//    SystemBadPageInformation = 128,
//    SystemProcessorProfileControlArea = 129,
//    SystemCombinePhysicalMemoryInformation = 130,
//    SystemEntropyInterruptTimingInformation = 131,
//    SystemConsoleInformation = 132,
//    SystemPlatformBinaryInformation = 133,
//    SystemPolicyInformation = 134,
//    SystemHypervisorProcessorCountInformation = 135,
//    SystemDeviceDataInformation = 136,
//    SystemDeviceDataEnumerationInformation = 137,
//    SystemMemoryTopologyInformation = 138,
//    SystemMemoryChannelInformation = 139,
//    SystemBootLogoInformation = 140,
//    SystemProcessorPerformanceInformationEx = 141,
//    SystemSpare0 = 142,
//    SystemSecureBootPolicyInformation = 143,
//    SystemPageFileInformationEx = 144,
//    SystemSecureBootInformation = 145,
//    SystemEntropyInterruptTimingRawInformation = 146,
//    SystemPortableWorkspaceEfiLauncherInformation = 147,
//    SystemFullProcessInformation = 148,
//    SystemKernelDebuggerInformationEx = 149,
//    SystemBootMetadataInformation = 150,
//    SystemSoftRebootInformation = 151,
//    SystemElamCertificateInformation = 152,
//    SystemOfflineDumpConfigInformation = 153,
//    SystemProcessorFeaturesInformation = 154,
//    SystemRegistryReconciliationInformation = 155,
//    SystemEdidInformation = 156,
//    SystemManufacturingInformation = 157,
//    SystemEnergyEstimationConfigInformation = 158,
//    SystemHypervisorDetailInformation = 159,
//    SystemProcessorCycleStatsInformation = 160,
//    SystemVmGenerationCountInformation = 161,
//    SystemTrustedPlatformModuleInformation = 162,
//    SystemKernelDebuggerFlags = 163,
//    SystemCodeIntegrityPolicyInformation = 164,
//    SystemIsolatedUserModeInformation = 165,
//    SystemHardwareSecurityTestInterfaceResultsInformation = 166,
//    SystemSingleModuleInformation = 167,
//    SystemAllowedCpuSetsInformation = 168,
//    SystemDmaProtectionInformation = 169,
//    SystemInterruptCpuSetsInformation = 170,
//    SystemSecureBootPolicyFullInformation = 171,
//    SystemCodeIntegrityPolicyFullInformation = 172,
//    SystemAffinitizedInterruptProcessorInformation = 173,
//    SystemRootSiloInformation = 174,
//    SystemCpuSetInformation = 175,
//    SystemCpuSetTagInformation = 176,
//    SystemWin32WerStartCallout = 177,
//    SystemSecureKernelProfileInformation = 178,
//    SystemCodeIntegrityPlatformManifestInformation = 179,
//    SystemInterruptSteeringInformation = 180,
//    SystemSupportedProcessorArchitectures = 181,
//    SystemMemoryUsageInformation = 182,
//    SystemCodeIntegrityCertificateInformation = 183,
//    SystemPhysicalMemoryInformation = 184,
//    SystemControlFlowTransition = 185,
//    SystemKernelDebuggingAllowed = 186,
//    SystemActivityModerationExeState = 187,
//    SystemActivityModerationUserSettings = 188,
//    SystemCodeIntegrityPoliciesFullInformation = 189,
//    SystemCodeIntegrityUnlockInformation = 190,
//    SystemIntegrityQuotaInformation = 191,
//    SystemFlushInformation = 192,
//    SystemProcessorIdleMaskInformation = 193,
//    SystemSecureDumpEncryptionInformation = 194,
//    SystemWriteConstraintInformation = 195,
//    SystemKernelVaShadowInformation = 196,
//    SystemHypervisorSharedPageInformation = 197,
//    SystemFirmwareBootPerformanceInformation = 198,
//    SystemCodeIntegrityVerificationInformation = 199,
//    SystemFirmwarePartitionInformation = 200,
//    SystemSpeculationControlInformation = 201,
//    SystemDmaGuardPolicyInformation = 202,
//    SystemEnclaveLaunchControlInformation = 203,
//    SystemWorkloadAllowedCpuSetsInformation = 204,
//    SystemCodeIntegrityUnlockModeInformation = 205,
//    SystemLeapSecondInformation = 206,
//    SystemFlags2Information = 207,
//    SystemSecurityModelInformation = 208,
//    SystemCodeIntegritySyntheticCacheInformation = 209,
//    SystemFeatureConfigurationInformation = 210,
//    SystemFeatureConfigurationSectionInformation = 211,
//    SystemFeatureUsageSubscriptionInformation = 212,
//    SystemSecureSpeculationControlInformation = 213,
//    SystemSpacesBootInformation = 214,
//    SystemFwRamdiskInformation = 215,
//    SystemWheaIpmiHardwareInformation = 216,
//    SystemDifSetRuleClassInformation = 217,
//    SystemDifClearRuleClassInformation = 218,
//    SystemDifApplyPluginVerificationOnDriver = 219,
//    SystemDifRemovePluginVerificationOnDriver = 220,
//    SystemShadowStackInformation = 221,
//    SystemBuildVersionInformation = 222,
//    SystemPoolLimitInformation = 223,
//    SystemCodeIntegrityAddDynamicStore = 224,
//    SystemCodeIntegrityClearDynamicStores = 225,
//    SystemDifPoolTrackingInformation = 226,
//    SystemPoolZeroingInformation = 227,
//    MaxSystemInfoClass
//} SYSTEM_INFORMATION_CLASS, * PSYSTEM_INFORMATION_CLASS;


FORCEINLINE 
LUID 
RtlConvertUlongToLuid(
    _In_ ULONG Ulong
)
{
    LUID tempLuid;

    tempLuid.LowPart = Ulong;
    tempLuid.HighPart = 0;

    return tempLuid;
}

NTSYSAPI
NTSTATUS
NTAPI
RtlSetDaclSecurityDescriptor(
    _Inout_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_ BOOLEAN DaclPresent,
    _In_opt_ PACL Dacl,
    _In_ BOOLEAN DaclDefaulted);

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateSecurityDescriptor(
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor,
    _In_ ULONG Revision);

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessAllowedAce(
    _Inout_ PACL Acl,
    _In_ ULONG AceRevision,
    _In_ ACCESS_MASK AccessMask,
    _In_ PSID Sid);

NTSYSAPI
PULONG
NTAPI
RtlSubAuthoritySid(
    _In_ PSID Sid,
    _In_ ULONG SubAuthority);

NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeSid(
    _Out_ PSID Sid,
    _In_ PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
    _In_ UCHAR SubAuthorityCount);

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateAcl(
    _Out_writes_bytes_(AclLength) PACL Acl,
    _In_ ULONG AclLength,
    _In_ ULONG AclRevision);


NTSYSAPI
ULONG
NTAPI
RtlLengthRequiredSid(
    _In_ ULONG SubAuthorityCount);

NTSYSAPI
BOOLEAN
NTAPI
RtlDosPathNameToNtPathName_U(
    _In_ PCWSTR DosFileName,
    _Out_ PUNICODE_STRING NtFileName,
    _Out_opt_ PWSTR *FilePart,
    _Reserved_ PVOID Reserved);

NTSTATUS RtlDosPathNameToNtPathName_U_WithStatus(
    __in PCWSTR DosFileName,
    __out PUNICODE_STRING NtFileName,
    __deref_opt_out_opt PWSTR *FilePart,
    __reserved PVOID Reserved
    );

//NTSTATUS RtlStringCchPrintfW(
//  PWCHAR  pszDest,
//  SIZE_T cchDest,
//  PWCHAR pszFormat,
//  ...             
//);



typedef enum _EVENT_TYPE {
    NotificationEvent,
    SynchronizationEvent
} EVENT_TYPE, *PEVENT_TYPE;

NTSYSAPI NTSTATUS NtCreateEvent(
    PHANDLE            EventHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    EVENT_TYPE         EventType,
    BOOLEAN            InitialState
);

NTSYSAPI
NTSTATUS
NTAPI
NtAdjustPrivilegesToken(
    _In_ HANDLE TokenHandle,
    _In_ BOOLEAN DisableAllPrivileges,
    _In_opt_ PTOKEN_PRIVILEGES NewState,
    _In_ ULONG BufferLength,
    _Out_writes_bytes_to_opt_(BufferLength, *ReturnLength) PTOKEN_PRIVILEGES PreviousState,
    _Out_opt_ PULONG ReturnLength);

NTSYSAPI
NTSTATUS
NTAPI
NtOpenProcessToken(
    _In_ HANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _Out_ PHANDLE TokenHandle);

NTSYSAPI
NTSTATUS
NTAPI
NtDuplicateObject(
    _In_ HANDLE SourceProcessHandle,
    _In_ HANDLE SourceHandle,
    _In_opt_ HANDLE TargetProcessHandle,
    _Out_ PHANDLE TargetHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG HandleAttributes,
    _In_ ULONG Options);

NTSYSAPI
NTSTATUS
NTAPI
NtOpenDirectoryObject(
    _Out_ PHANDLE DirectoryHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes);

NTSYSAPI
NTSTATUS
NTAPI
NtQueryDirectoryObject(
    _In_ HANDLE DirectoryHandle,
    _Out_writes_bytes_opt_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_ BOOLEAN RestartScan,
    _Inout_ PULONG Context,
    _Out_opt_ PULONG ReturnLength);

NTSYSAPI
NTSTATUS
NTAPI
NtSetSecurityObject(
    _In_ HANDLE Handle,
    _In_ SECURITY_INFORMATION SecurityInformation,
    _In_ PSECURITY_DESCRIPTOR SecurityDescriptor);

NTSTATUS NtReadFile(
    _In_     HANDLE           FileHandle,
    _In_opt_ HANDLE           Event,
    _In_opt_ PIO_APC_ROUTINE  ApcRoutine,
    _In_opt_ PVOID            ApcContext,
    _Out_    PIO_STATUS_BLOCK IoStatusBlock,
    _Out_    PVOID            Buffer,
    _In_     ULONG            Length,
    _In_opt_ PLARGE_INTEGER   ByteOffset,
    _In_opt_ PULONG           Key
);

NTSTATUS NtWriteFile(
    _In_     HANDLE           FileHandle,
    _In_opt_ HANDLE           Event,
    _In_opt_ PIO_APC_ROUTINE  ApcRoutine,
    _In_opt_ PVOID            ApcContext,
    _Out_    PIO_STATUS_BLOCK IoStatusBlock,
    _Out_    PVOID            Buffer,
    _In_     ULONG            Length,
    _In_opt_ PLARGE_INTEGER   ByteOffset,
    _In_opt_ PULONG           Key
);

typedef struct _SYSTEM_PROCESSOR_INFORMATION
{
    USHORT ProcessorArchitecture;
    USHORT ProcessorLevel;
    USHORT ProcessorRevision;
    USHORT MaximumProcessors;
    ULONG ProcessorFeatureBits; // Size=4 Offset=8
} SYSTEM_PROCESSOR_INFORMATION, * PSYSTEM_PROCESSOR_INFORMATION;

#define PROCESSOR_ARCHITECTURE_INTEL            0
#define PROCESSOR_ARCHITECTURE_AMD64            9


NTSTATUS RtlGetVersion(
    PRTL_OSVERSIONINFOW lpVersionInformation
);

NTSYSAPI
NTSTATUS
NTAPI
NtLoadDriver(
    _In_ PUNICODE_STRING DriverServiceName);

NTSYSAPI
NTSTATUS
NTAPI
NtUnloadDriver(
    _In_ PUNICODE_STRING DriverServiceName);

NTSYSAPI
NTSTATUS
NTAPI
NtOpenProcess(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PCLIENT_ID ClientId);

NTSYSAPI
NTSTATUS
NTAPI
NtUnmapViewOfSection(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress);

NTSYSAPI
NTSTATUS
NTAPI
NtOpenSection(
    _Out_ PHANDLE SectionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes);

NTSYSAPI
NTSTATUS
NTAPI
LdrLoadDll(
    _In_opt_ PCWSTR DllPath,
    _In_opt_ PULONG DllCharacteristics,
    _In_  PCUNICODE_STRING DllName,
    _Out_ PVOID *DllHandle);

NTSYSAPI
NTSTATUS
NTAPI
LdrUnloadDll(
    _In_ PVOID DllHandle);

NTSYSAPI
NTSTATUS
NTAPI
LdrGetProcedureAddress(
    _In_ PVOID DllHandle,
    _In_opt_ CONST ANSI_STRING* ProcedureName,
    _In_ ULONG ProcedureNumber,
    _Out_ PVOID *ProcedureAddress);



NTSYSAPI
NTSTATUS
NTAPI
LdrFindResource_U(
    _In_ PVOID DllHandle,
    _In_ CONST ULONG_PTR* ResourceIdPath,
    _In_ ULONG ResourceIdPathLength,
    _Out_ PIMAGE_RESOURCE_DATA_ENTRY *ResourceDataEntry);

/**
 * The LdrAccessResource function returns a pointer to the first byte of the specified resource in memory.
 *
 * @param DllHandle A handle to the DLL.
 * @param ResourceDataEntry The resource information block.
 * @param ResourceBuffer The pointer to the specified resource in memory.
 * @param ResourceLength The size, in bytes, of the specified resource.
 * @return NTSTATUS Successful or errant status.
 * @sa https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadresource
 */
NTSYSAPI
NTSTATUS
NTAPI
LdrAccessResource(
    _In_ PVOID DllHandle,
    _In_ CONST IMAGE_RESOURCE_DATA_ENTRY* ResourceDataEntry,
    _Out_opt_ PVOID *Address,
    _Out_opt_ PULONG Size);


NTSYSAPI
LONG
NTAPI
RtlCompareUnicodeString(
    _In_ PCUNICODE_STRING String1,
    _In_ PCUNICODE_STRING String2,
    _In_ BOOLEAN CaseInSensitive
    );



typedef struct _RTL_BALANCED_NODE
{
    union
    {
        struct _RTL_BALANCED_NODE* Children[2];
        struct
        {
            struct _RTL_BALANCED_NODE* Left;
            struct _RTL_BALANCED_NODE* Right;
        };
    };
    union
    {
        UCHAR Red : 1;
        UCHAR Balance : 2;
        ULONG_PTR ParentValue;
    };
} RTL_BALANCED_NODE, * PRTL_BALANCED_NODE;

typedef enum _LDR_DLL_LOAD_REASON {
    LoadReasonStaticDependency,
    LoadReasonStaticForwarderDependency,
    LoadReasonDynamicForwarderDependency,
    LoadReasonDelayloadDependency,
    LoadReasonDynamicLoad,
    LoadReasonAsImageLoad,
    LoadReasonAsDataLoad,
    LoadReasonEnclavePrimary,
    LoadReasonEnclaveDependency,
    LoadReasonUnknown = -1
} LDR_DLL_LOAD_REASON, * PLDR_DLL_LOAD_REASON;

//
// Full declaration of LDR_DATA_TABLE_ENTRY
//
typedef struct _LDR_DATA_TABLE_ENTRY_FULL
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    union
    {
        LIST_ENTRY InInitializationOrderLinks;
        LIST_ENTRY InProgressLinks;
    };
    PVOID DllBase;
    PVOID EntryPoint; // PLDR_INIT_ROUTINE
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    union
    {
        UCHAR FlagGroup[4];
        ULONG Flags;
        struct
        {
            ULONG PackagedBinary : 1;
            ULONG MarkedForRemoval : 1;
            ULONG ImageDll : 1;
            ULONG LoadNotificationsSent : 1;
            ULONG TelemetryEntryProcessed : 1;
            ULONG ProcessStaticImport : 1;
            ULONG InLegacyLists : 1;
            ULONG InIndexes : 1;
            ULONG ShimDll : 1;
            ULONG InExceptionTable : 1;
            ULONG ReservedFlags1 : 2;
            ULONG LoadInProgress : 1;
            ULONG LoadConfigProcessed : 1;
            ULONG EntryProcessed : 1;
            ULONG ProtectDelayLoad : 1;
            ULONG ReservedFlags3 : 2;
            ULONG DontCallForThreads : 1;
            ULONG ProcessAttachCalled : 1;
            ULONG ProcessAttachFailed : 1;
            ULONG CorDeferredValidate : 1;
            ULONG CorImage : 1;
            ULONG DontRelocate : 1;
            ULONG CorILOnly : 1;
            ULONG ChpeImage : 1;
            ULONG ReservedFlags5 : 2;
            ULONG Redirected : 1;
            ULONG ReservedFlags6 : 2;
            ULONG CompatDatabaseProcessed : 1;
        };
    };
    USHORT ObsoleteLoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;
    ULONG TimeDateStamp;
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;
    PVOID Lock;
    PVOID DdagNode; // PLDR_DDAG_NODE
    LIST_ENTRY NodeModuleLink;
    struct _LDRP_LOAD_CONTEXT* LoadContext;
    PVOID ParentDllBase;
    PVOID SwitchBackContext;
    RTL_BALANCED_NODE BaseAddressIndexNode;
    RTL_BALANCED_NODE MappingInfoIndexNode;
    ULONG_PTR OriginalBase;
    LARGE_INTEGER LoadTime;
    ULONG BaseNameHashValue;
    LDR_DLL_LOAD_REASON LoadReason;
    ULONG ImplicitPathOptions;
    ULONG ReferenceCount;
    ULONG DependentLoadFlags;
    UCHAR SigningLevel;
} LDR_DATA_TABLE_ENTRY_FULL, * PLDR_DATA_TABLE_ENTRY_FULL;

_Must_inspect_result_
NTSYSAPI
PVOID
NTAPI
RtlAllocateHeap(
    _In_ PVOID HeapHandle,
    _In_ ULONG Flags,
    _In_ SIZE_T Size);

NTSYSAPI
VOID
NTAPI
RtlSetLastWin32Error(
    _In_ LONG Win32Error);

NTSYSAPI
NTSTATUS
NTAPI
RtlExpandEnvironmentStrings(
    _In_opt_ PVOID Environment,
    _In_reads_(SrcLength) PWSTR Src,
    _In_ SIZE_T SrcLength,
    _Out_writes_opt_(DstLength) PWSTR Dst,
    _In_ SIZE_T DstLength,
    _Out_opt_ PSIZE_T ReturnLength);



//
// Registry
//
// Value entry query structures
//
typedef struct _KEY_BASIC_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_NODE_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
//          Class[1];           // Variable length string not declared
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef struct _KEY_FULL_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   SubKeys;
    ULONG   MaxNameLen;
    ULONG   MaxClassLen;
    ULONG   Values;
    ULONG   MaxValueNameLen;
    ULONG   MaxValueDataLen;
    WCHAR   Class[1];           // Variable length
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation,
    KeyCachedInformation,
    KeyFlagsInformation,
    KeyVirtualizationInformation,
    KeyHandleTagsInformation,
    KeyTrustInformation,
    KeyLayerInformation,
    MaxKeyInfoClass  // MaxKeyInfoClass should always be the last enum
} KEY_INFORMATION_CLASS;

typedef struct _KEY_VALUE_BASIC_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataOffset;
    ULONG   DataLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    _Field_size_bytes_(DataLength) UCHAR Data[1]; // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION_ALIGN64 {
    ULONG   Type;
    ULONG   DataLength;
    _Field_size_bytes_(DataLength) UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION_ALIGN64, *PKEY_VALUE_PARTIAL_INFORMATION_ALIGN64;

typedef struct _KEY_VALUE_LAYER_INFORMATION {
    ULONG   IsTombstone     : 1;
    ULONG   Reserved        : 31;
} KEY_VALUE_LAYER_INFORMATION, *PKEY_VALUE_LAYER_INFORMATION;

//typedef struct _KEY_VALUE_ENTRY {
//    PUNICODE_STRING ValueName;
//    ULONG           DataLength;
//    ULONG           DataOffset;
//    ULONG           Type;
//} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64,
    KeyValueLayerInformation,
    MaxKeyValueInfoClass  // MaxKeyValueInfoClass should always be the last enum
} KEY_VALUE_INFORMATION_CLASS;

NTSYSAPI NTSTATUS ZwCreateKey(
  _Out_ PHANDLE KeyHandle,
  _In_ ACCESS_MASK DesiredAccess,
  _In_ POBJECT_ATTRIBUTES ObjectAttributes,
  _In_ ULONG              TitleIndex,
  _In_opt_ PUNICODE_STRING Class,
  _In_ ULONG CreateOptions,
  _Out_opt_ PULONG Disposition
);

NTSYSAPI NTSTATUS ZwSetValueKey(
  _In_ HANDLE KeyHandle,
  _In_ PUNICODE_STRING ValueName,
  _In_ ULONG TitleIndex,
  _In_ ULONG Type,
  _In_opt_ PVOID Data,
  _In_ ULONG DataSize
);

NTSYSAPI NTSTATUS ZwOpenKeyEx(
  _Out_ PHANDLE            KeyHandle,
  _In_  ACCESS_MASK        DesiredAccess,
  _In_  POBJECT_ATTRIBUTES ObjectAttributes,
  _In_  ULONG              OpenOptions
);

NTSYSAPI NTSTATUS ZwQueryValueKey(
  _In_ HANDLE KeyHandle,
  _In_ PUNICODE_STRING ValueName,
  _In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
  _Out_opt_ PVOID KeyValueInformation,
  _In_ ULONG Length,
  _Out_ PULONG ResultLength
);

NTSYSAPI NTSTATUS ZwDeleteKey(
  _In_ HANDLE KeyHandle
);

NTSYSAPI NTSTATUS ZwDeleteValueKey(
  _In_ HANDLE          KeyHandle,
  _In_ PUNICODE_STRING ValueName
);

NTSYSAPI NTSTATUS ZwEnumerateKey(
  _In_ HANDLE KeyHandle,
  _In_ ULONG Index,
  _In_ KEY_INFORMATION_CLASS KeyInformationClass,
  _Out_opt_ PVOID KeyInformation,
  _In_ ULONG Length,
  _Out_ PULONG ResultLength
);

NTSYSAPI NTSTATUS ZwEnumerateValueKey(
  _In_ HANDLE KeyHandle,
  _In_ ULONG Index,
  _In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
  _Out_opt_ PVOID KeyValueInformation,
  _In_ ULONG Length,
  _Out_ PULONG ResultLength
);

NTSYSAPI
NTSTATUS ZwClose(
    _In_ _Post_ptr_invalid_ HANDLE Handle
);

//NTSYSAPI
//NTSTATUS
//NTAPI
//NtClose(
//    _In_ _Post_ptr_invalid_ HANDLE Handle);

//
// Pool Allocation routines (in pool.c)
//
typedef _Enum_is_bitflag_ enum _POOL_TYPE {
    NonPagedPool,
    NonPagedPoolExecute = NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed = NonPagedPool + 2,
    DontUseThisType,
    NonPagedPoolCacheAligned = NonPagedPool + 4,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS = NonPagedPool + 6,
    MaxPoolType,

    //
    // Define base types for NonPaged (versus Paged) pool, for use in cracking
    // the underlying pool type.
    //

    NonPagedPoolBase = 0,
    NonPagedPoolBaseMustSucceed = NonPagedPoolBase + 2,
    NonPagedPoolBaseCacheAligned = NonPagedPoolBase + 4,
    NonPagedPoolBaseCacheAlignedMustS = NonPagedPoolBase + 6,

    //
    // Note these per session types are carefully chosen so that the appropriate
    // masking still applies as well as MaxPoolType above.
    //

    NonPagedPoolSession = 32,
    PagedPoolSession = NonPagedPoolSession + 1,
    NonPagedPoolMustSucceedSession = PagedPoolSession + 1,
    DontUseThisTypeSession = NonPagedPoolMustSucceedSession + 1,
    NonPagedPoolCacheAlignedSession = DontUseThisTypeSession + 1,
    PagedPoolCacheAlignedSession = NonPagedPoolCacheAlignedSession + 1,
    NonPagedPoolCacheAlignedMustSSession = PagedPoolCacheAlignedSession + 1,

    NonPagedPoolNx = 512,
    NonPagedPoolNxCacheAligned = NonPagedPoolNx + 4,
    NonPagedPoolSessionNx = NonPagedPoolNx + 32,

} _Enum_is_bitflag_ POOL_TYPE;


//
// Lists
// 

// single linked

FORCEINLINE
VOID
FatalListEntryError(
    _In_ PVOID p1,
    _In_ PVOID p2,
    _In_ PVOID p3
    )

{

    UNREFERENCED_PARAMETER(p1);
    UNREFERENCED_PARAMETER(p2);
    UNREFERENCED_PARAMETER(p3);

    RtlFailFast(FAST_FAIL_CORRUPT_LIST_ENTRY);
}

FORCEINLINE
PSINGLE_LIST_ENTRY
PopEntryList(
    _Inout_ PSINGLE_LIST_ENTRY ListHead
    )
{

    PSINGLE_LIST_ENTRY FirstEntry;

    FirstEntry = ListHead->Next;
    if (FirstEntry != NULL) {
        ListHead->Next = FirstEntry->Next;
    }

    return FirstEntry;
}


FORCEINLINE
VOID
PushEntryList(
    _Inout_ PSINGLE_LIST_ENTRY ListHead,
    _Inout_ __drv_aliasesMem PSINGLE_LIST_ENTRY Entry
    )

{

    Entry->Next = ListHead->Next;
    ListHead->Next = Entry;
    return;
}

// double linked

FORCEINLINE
VOID
InitializeListHead(
    _Out_ PLIST_ENTRY ListHead
    )

{

    ListHead->Flink = ListHead->Blink = ListHead;
    return;
}

FORCEINLINE
VOID
InsertTailList(
    _Inout_ PLIST_ENTRY ListHead,
    _Out_ __drv_aliasesMem PLIST_ENTRY Entry
    )
{

    PLIST_ENTRY PrevEntry;

#if DBG

    RtlpCheckListEntry(ListHead);

#endif

    PrevEntry = ListHead->Blink;
    if (PrevEntry->Flink != ListHead) {
        FatalListEntryError((PVOID)PrevEntry,
                            (PVOID)ListHead,
                            (PVOID)PrevEntry->Flink);
    }

    Entry->Flink = ListHead;
    Entry->Blink = PrevEntry;
    PrevEntry->Flink = Entry;
    ListHead->Blink = Entry;
    return;
}

FORCEINLINE
PLIST_ENTRY
RemoveTailList(
    _Inout_ PLIST_ENTRY ListHead
    )
{

    PLIST_ENTRY Entry;
    PLIST_ENTRY PrevEntry;

    Entry = ListHead->Blink;

#if DBG

    RtlpCheckListEntry(ListHead);

#endif

    PrevEntry = Entry->Blink;
    if ((Entry->Flink != ListHead) || (PrevEntry->Flink != Entry)) {
        FatalListEntryError((PVOID)PrevEntry,
                            (PVOID)Entry,
                            (PVOID)ListHead);
    }

    ListHead->Blink = PrevEntry;
    PrevEntry->Flink = ListHead;
    return Entry;
}

FORCEINLINE
PLIST_ENTRY
PeekTailList(
    _Inout_ PLIST_ENTRY ListHead
    )
{

    PLIST_ENTRY Entry;
    PLIST_ENTRY PrevEntry;

    Entry = ListHead->Blink;

#if DBG

    RtlpCheckListEntry(ListHead);

#endif

    PrevEntry = Entry->Blink;
    if ( (Entry->Flink != ListHead) ) {
        FatalListEntryError((PVOID)PrevEntry,
                            (PVOID)Entry,
                            (PVOID)ListHead);
    }

    //ListHead->Blink = PrevEntry;
    //PrevEntry->Flink = ListHead;
    return Entry;
}

_Must_inspect_result_
BOOLEAN
CFORCEINLINE
IsListEmpty(
    _In_ const LIST_ENTRY * ListHead
    )

{

    return (BOOLEAN)(ListHead->Flink == ListHead);
}
