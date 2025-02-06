#pragma once 

FORCEINLINE
PCHAR getTypeString(ULONG type)
{
    switch ( type )
    {
        case SERVICE_FILE_SYSTEM_DRIVER: return "File system driver";
        case SERVICE_KERNEL_DRIVER: return "Device driver";
        case SERVICE_WIN32_OWN_PROCESS: return "Own process";
        case SERVICE_WIN32_SHARE_PROCESS: return "Shares a process with other services";
        case SERVICE_USER_OWN_PROCESS: return "Own process under the logged-on user account";
        case SERVICE_USER_SHARE_PROCESS: return "Shares a process with one or more other services that run under the logged-on user account";
        default: return "Unknown";
    }
}

FORCEINLINE
PCHAR getStateString(ULONG state)
{
    switch ( state )
    {
        case 0: return "None";
        case SERVICE_CONTINUE_PENDING: return "Pending";
        case SERVICE_PAUSE_PENDING: return "Pause pending";
        case SERVICE_PAUSED: return "Paused";
        case SERVICE_RUNNING: return "Running";
        case SERVICE_START_PENDING: return "Starting";
        case SERVICE_STOP_PENDING: return "Stopping";
        case SERVICE_STOPPED: return "Running";
        default: return "Unknown";
    }
}

FORCEINLINE
PCHAR getControlsAcceptedStr(ULONG ctrl)
{
    switch ( ctrl )
    {
        case 0: return "None";
        case SERVICE_ACCEPT_NETBINDCHANGE: return "Network component that can accept changes in its binding without being stopped and restarted.";
        case SERVICE_ACCEPT_PARAMCHANGE: return "Can reread its startup parameters without being stopped and restarted.";
        case SERVICE_ACCEPT_PAUSE_CONTINUE: return "Can be paused and continued.";
        case SERVICE_ACCEPT_PRESHUTDOWN: return "Can perform preshutdown tasks.";
        case SERVICE_ACCEPT_SHUTDOWN: return "Is notified when system shutdown occurs.";
        case SERVICE_ACCEPT_STOP: return "Can be stopped.";
        case SERVICE_ACCEPT_HARDWAREPROFILECHANGE: return "Is notified when the computer's hardware profile has changed. This enables the system to send SERVICE_CONTROL_HARDWAREPROFILECHANGE notifications to the service.";
        case SERVICE_ACCEPT_POWEREVENT: return "Is notified when the computer's power status has changed. This enables the system to send SERVICE_CONTROL_POWEREVENT notifications to the service.";
        case SERVICE_ACCEPT_SESSIONCHANGE: return "Is notified when the computer's session status has changed. This enables the system to send SERVICE_CONTROL_SESSIONCHANGE notifications to the service.";
        case SERVICE_ACCEPT_TIMECHANGE: return "Is notified when the system time has changed. This enables the system to send SERVICE_CONTROL_TIMECHANGE notifications to the service.";
        case SERVICE_ACCEPT_TRIGGEREVENT: return "Is notified when an event for which the service has registered occurs. This enables the system to send SERVICE_CONTROL_TRIGGEREVENT notifications to the service.";
        case SERVICE_ACCEPT_USER_LOGOFF: return "SERVICE_ACCEPT_USER_LOGOFF.";
        case SERVICE_ACCEPT_LOWRESOURCES: return "SERVICE_ACCEPT_LOWRESOURCES.";
        case SERVICE_ACCEPT_SYSTEMLOWRESOURCES: return "SERVICE_ACCEPT_SYSTEMLOWRESOURCES.";
        default: return "Unknown";
    }
}
