//-----------------------------------------------------------------------------
// <auto-generated> 
//   This code was generated by a tool. 
// 
//   Changes to this file may cause incorrect behavior and will be lost if  
//   the code is regenerated.
//
//   Tool: AllJoynCodeGenerator.exe
//
//   This tool is located in the Windows 10 SDK and the Windows 10 AllJoyn 
//   Visual Studio Extension in the Visual Studio Gallery.  
//
//   The generated code should be packaged in a Windows 10 C++/CX Runtime  
//   Component which can be consumed in any UWP-supported language using 
//   APIs that are available in Windows.Devices.AllJoyn.
//
//   Using AllJoynCodeGenerator - Invoke the following command with a valid 
//   Introspection XML file and a writable output directory:
//     AllJoynCodeGenerator -i <INPUT XML FILE> -o <OUTPUT DIRECTORY>
// </auto-generated>
//-----------------------------------------------------------------------------
#include "pch.h"

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Devices::AllJoyn;
using namespace com::ianlee::fezhatnode;

std::map<alljoyn_busobject, WeakReference*> fezhatnodeProducer::SourceObjects;
std::map<alljoyn_interfacedescription, WeakReference*> fezhatnodeProducer::SourceInterfaces;

fezhatnodeProducer::fezhatnodeProducer(AllJoynBusAttachment^ busAttachment)
    : m_busAttachment(busAttachment),
    m_sessionListener(nullptr),
    m_busObject(nullptr),
    m_sessionPort(0),
    m_sessionId(0)
{
    m_weak = new WeakReference(this);
    ServiceObjectPath = ref new String(L"/Service");
    m_signals = ref new fezhatnodeSignals();
    m_busAttachmentStateChangedToken.Value = 0;
}

fezhatnodeProducer::~fezhatnodeProducer()
{
    UnregisterFromBus();
    delete m_weak;
}

void fezhatnodeProducer::UnregisterFromBus()
{
    if ((nullptr != m_busAttachment) && (0 != m_busAttachmentStateChangedToken.Value))
    {
        m_busAttachment->StateChanged -= m_busAttachmentStateChangedToken;
        m_busAttachmentStateChangedToken.Value = 0;
    }
    if (nullptr != SessionPortListener)
    {
        alljoyn_busattachment_unbindsessionport(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), m_sessionPort);
        alljoyn_sessionportlistener_destroy(SessionPortListener);
        SessionPortListener = nullptr;
    }
    if (nullptr != BusObject)
    {
        alljoyn_busattachment_unregisterbusobject(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject);
        alljoyn_busobject_destroy(BusObject);
        BusObject = nullptr;
    }
    if (nullptr != SessionListener)
    {
        alljoyn_sessionlistener_destroy(SessionListener);
        SessionListener = nullptr;
    }
}

bool fezhatnodeProducer::OnAcceptSessionJoiner(_In_ alljoyn_sessionport sessionPort, _In_ PCSTR joiner, _In_ const alljoyn_sessionopts opts)
{
    UNREFERENCED_PARAMETER(sessionPort); UNREFERENCED_PARAMETER(joiner); UNREFERENCED_PARAMETER(opts);
    
    return true;
}

void fezhatnodeProducer::OnSessionJoined(_In_ alljoyn_sessionport sessionPort, _In_ alljoyn_sessionid id, _In_ PCSTR joiner)
{
    UNREFERENCED_PARAMETER(joiner);

    // We initialize the Signals object after the session has been joined, because it needs
    // the session id.
    m_signals->Initialize(BusObject, id);
    m_sessionPort = sessionPort;
    m_sessionId = id;

    alljoyn_sessionlistener_callbacks callbacks =
    {
        AllJoynHelpers::SessionLostHandler<fezhatnodeProducer>,
        AllJoynHelpers::SessionMemberAddedHandler<fezhatnodeProducer>,
        AllJoynHelpers::SessionMemberRemovedHandler<fezhatnodeProducer>
    };

    SessionListener = alljoyn_sessionlistener_create(&callbacks, m_weak);
    alljoyn_busattachment_setsessionlistener(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), id, SessionListener);
}

void fezhatnodeProducer::OnSessionLost(_In_ alljoyn_sessionid sessionId, _In_ alljoyn_sessionlostreason reason)
{
    if (sessionId == m_sessionId)
    {
        AllJoynSessionLostEventArgs^ args = ref new AllJoynSessionLostEventArgs(static_cast<AllJoynSessionLostReason>(reason));
        AllJoynHelpers::DispatchEvent([=]() {
            SessionLost(this, args);
        });
    }
}

void fezhatnodeProducer::OnSessionMemberAdded(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberAddedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        AllJoynHelpers::DispatchEvent([=]() {
            SessionMemberAdded(this, args);
        });
    }
}

void fezhatnodeProducer::OnSessionMemberRemoved(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberRemovedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        AllJoynHelpers::DispatchEvent([=]() {
            SessionMemberRemoved(this, args);
        });
    }
}

void fezhatnodeProducer::BusAttachmentStateChanged(_In_ AllJoynBusAttachment^ sender, _In_ AllJoynBusAttachmentStateChangedEventArgs^ args)
{
    if (args->State == AllJoynBusAttachmentState::Connected)
    {   
        QStatus result = AllJoynHelpers::CreateProducerSession<fezhatnodeProducer>(m_busAttachment, m_weak);
        if (ER_OK != result)
        {
            StopInternal(result);
            return;
        }
    }
    else if (args->State == AllJoynBusAttachmentState::Disconnected)
    {
        StopInternal(ER_BUS_STOPPING);
    }
}

void fezhatnodeProducer::CallSetRedLedStateHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    fezhatnodeProducer^ producer = source->second->Resolve<fezhatnodeProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));

        bool inputArg0;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "b", &inputArg0);

        create_task(producer->Service->SetRedLedStateAsync(callInfo, inputArg0)).then([busObject, message](fezhatnodeSetRedLedStateResult^ result)
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 0;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }).wait();
    }
}

void fezhatnodeProducer::CallGetRedLedStateHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    fezhatnodeProducer^ producer = source->second->Resolve<fezhatnodeProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));


        create_task(producer->Service->GetRedLedStateAsync(callInfo)).then([busObject, message](fezhatnodeGetRedLedStateResult^ result)
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 1;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            status = TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(outputs, 0), "b", result->State);
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                alljoyn_msgarg_destroy(outputs);
                return;
            }

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }).wait();
    }
}

void fezhatnodeProducer::CallSetRgbLedD2ColorHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    fezhatnodeProducer^ producer = source->second->Resolve<fezhatnodeProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));

        bool inputArg0;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "b", &inputArg0);
        byte inputArg1;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 1), "y", &inputArg1);
        byte inputArg2;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 2), "y", &inputArg2);
        byte inputArg3;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 3), "y", &inputArg3);

        create_task(producer->Service->SetRgbLedD2ColorAsync(callInfo, inputArg0, inputArg1, inputArg2, inputArg3)).then([busObject, message](fezhatnodeSetRgbLedD2ColorResult^ result)
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 0;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }).wait();
    }
}

void fezhatnodeProducer::CallSetRgbLedD3ColorHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    fezhatnodeProducer^ producer = source->second->Resolve<fezhatnodeProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));

        bool inputArg0;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "b", &inputArg0);
        byte inputArg1;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 1), "y", &inputArg1);
        byte inputArg2;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 2), "y", &inputArg2);
        byte inputArg3;
        TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 3), "y", &inputArg3);

        create_task(producer->Service->SetRgbLedD3ColorAsync(callInfo, inputArg0, inputArg1, inputArg2, inputArg3)).then([busObject, message](fezhatnodeSetRgbLedD3ColorResult^ result)
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 0;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }).wait();
    }
}

void fezhatnodeProducer::CallGetLightSensorValueHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    fezhatnodeProducer^ producer = source->second->Resolve<fezhatnodeProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));


        create_task(producer->Service->GetLightSensorValueAsync(callInfo)).then([busObject, message](fezhatnodeGetLightSensorValueResult^ result)
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 1;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            status = TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(outputs, 0), "d", result->Value);
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                alljoyn_msgarg_destroy(outputs);
                return;
            }

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }).wait();
    }
}

void fezhatnodeProducer::CallGetTemperatureSensorValueHandler(_Inout_ alljoyn_busobject busObject, _In_ alljoyn_message message)
{
    auto source = SourceObjects.find(busObject);
    if (source == SourceObjects.end())
    {
        return;
    }

    fezhatnodeProducer^ producer = source->second->Resolve<fezhatnodeProducer>();
    if (producer->Service != nullptr)
    {
        AllJoynMessageInfo^ callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));


        create_task(producer->Service->GetTemperatureSensorValueAsync(callInfo)).then([busObject, message](fezhatnodeGetTemperatureSensorValueResult^ result)
        {
            int32 status;

            if (nullptr == result)
            {
                alljoyn_busobject_methodreply_status(busObject, message, ER_BUS_NO_LISTENER);
                return;
            }

            status = result->Status;
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                return;
            }

            size_t argCount = 2;
            alljoyn_msgarg outputs = alljoyn_msgarg_array_create(argCount);

            status = TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(outputs, 0), "d", result->ValueC);
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                alljoyn_msgarg_destroy(outputs);
                return;
            }

            status = TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(outputs, 1), "d", result->ValueF);
            if (AllJoynStatus::Ok != status)
            {
                alljoyn_busobject_methodreply_status(busObject, message, static_cast<QStatus>(status));
                alljoyn_msgarg_destroy(outputs);
                return;
            }

            alljoyn_busobject_methodreply_args(busObject, message, outputs, argCount);
            alljoyn_msgarg_destroy(outputs);
        }).wait();
    }
}

void fezhatnodeProducer::CallButtonDio18PressedSignalHandler(_In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message message)
{
    auto source = SourceInterfaces.find(member->iface);
    if (source == SourceInterfaces.end())
    {
        return;
    }

    auto producer = source->second->Resolve<fezhatnodeProducer>();
    if (producer->Signals != nullptr)
    {
        auto callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));
        auto eventArgs = ref new fezhatnodeButtonDio18PressedReceivedEventArgs();
        eventArgs->MessageInfo = callInfo;


        producer->Signals->CallButtonDio18PressedReceived(producer->Signals, eventArgs);
    }
}

void fezhatnodeProducer::CallButtonDio22PressedSignalHandler(_In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message message)
{
    auto source = SourceInterfaces.find(member->iface);
    if (source == SourceInterfaces.end())
    {
        return;
    }

    auto producer = source->second->Resolve<fezhatnodeProducer>();
    if (producer->Signals != nullptr)
    {
        auto callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));
        auto eventArgs = ref new fezhatnodeButtonDio22PressedReceivedEventArgs();
        eventArgs->MessageInfo = callInfo;


        producer->Signals->CallButtonDio22PressedReceived(producer->Signals, eventArgs);
    }
}

QStatus fezhatnodeProducer::AddMethodHandler(_In_ alljoyn_interfacedescription interfaceDescription, _In_ PCSTR methodName, _In_ alljoyn_messagereceiver_methodhandler_ptr handler)
{
    alljoyn_interfacedescription_member member;
    if (!alljoyn_interfacedescription_getmember(interfaceDescription, methodName, &member))
    {
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    return alljoyn_busobject_addmethodhandler(
        m_busObject,
        member,
        handler,
        m_weak);
}

QStatus fezhatnodeProducer::AddSignalHandler(_In_ alljoyn_busattachment busAttachment, _In_ alljoyn_interfacedescription interfaceDescription, _In_ PCSTR methodName, _In_ alljoyn_messagereceiver_signalhandler_ptr handler)
{
    alljoyn_interfacedescription_member member;
    if (!alljoyn_interfacedescription_getmember(interfaceDescription, methodName, &member))
    {
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    return alljoyn_busattachment_registersignalhandler(busAttachment, handler, member, NULL);
}

QStatus fezhatnodeProducer::OnPropertyGet(_In_ PCSTR interfaceName, _In_ PCSTR propertyName, _Inout_ alljoyn_msgarg value)
{
    UNREFERENCED_PARAMETER(propertyName);
    UNREFERENCED_PARAMETER(value);
    UNREFERENCED_PARAMETER(interfaceName);


    return ER_BUS_NO_SUCH_PROPERTY;
}

QStatus fezhatnodeProducer::OnPropertySet(_In_ PCSTR interfaceName, _In_ PCSTR propertyName, _In_ alljoyn_msgarg value)
{
    UNREFERENCED_PARAMETER(propertyName);
    UNREFERENCED_PARAMETER(value);
    UNREFERENCED_PARAMETER(interfaceName);

    return ER_BUS_NO_SUCH_PROPERTY;
}

void fezhatnodeProducer::Start()
{
    if (nullptr == m_busAttachment)
    {
        StopInternal(ER_FAIL);
        return;
    }

    QStatus result = AllJoynHelpers::CreateInterfaces(m_busAttachment, c_fezhatnodeIntrospectionXml);
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AllJoynHelpers::CreateBusObject<fezhatnodeProducer>(m_weak);
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    alljoyn_interfacedescription interfaceDescription = alljoyn_busattachment_getinterface(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), "com.ianlee.fezhatnode");
    if (interfaceDescription == nullptr)
    {
        StopInternal(ER_FAIL);
        return;
    }
    alljoyn_busobject_addinterface_announced(BusObject, interfaceDescription);

    result = AddMethodHandler(
        interfaceDescription, 
        "SetRedLedState", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallSetRedLedStateHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddMethodHandler(
        interfaceDescription, 
        "GetRedLedState", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallGetRedLedStateHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddMethodHandler(
        interfaceDescription, 
        "SetRgbLedD2Color", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallSetRgbLedD2ColorHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddMethodHandler(
        interfaceDescription, 
        "SetRgbLedD3Color", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallSetRgbLedD3ColorHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddMethodHandler(
        interfaceDescription, 
        "GetLightSensorValue", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallGetLightSensorValueHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddMethodHandler(
        interfaceDescription, 
        "GetTemperatureSensorValue", 
        [](alljoyn_busobject busObject, const alljoyn_interfacedescription_member* member, alljoyn_message message) { UNREFERENCED_PARAMETER(member); CallGetTemperatureSensorValueHandler(busObject, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    result = AddSignalHandler(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        interfaceDescription,
        "ButtonDio18Pressed",
        [](const alljoyn_interfacedescription_member* member, PCSTR srcPath, alljoyn_message message) { UNREFERENCED_PARAMETER(srcPath); CallButtonDio18PressedSignalHandler(member, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }
    result = AddSignalHandler(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        interfaceDescription,
        "ButtonDio22Pressed",
        [](const alljoyn_interfacedescription_member* member, PCSTR srcPath, alljoyn_message message) { UNREFERENCED_PARAMETER(srcPath); CallButtonDio22PressedSignalHandler(member, message); });
    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }
    
    SourceObjects[m_busObject] = m_weak;
    SourceInterfaces[interfaceDescription] = m_weak;
    
    unsigned int noneMechanismIndex = 0;
    bool authenticationMechanismsContainsNone = m_busAttachment->AuthenticationMechanisms->IndexOf(AllJoynAuthenticationMechanism::None, &noneMechanismIndex);
    QCC_BOOL interfaceIsSecure = alljoyn_interfacedescription_issecure(interfaceDescription);

    // If the current set of AuthenticationMechanisms supports authentication,
    // determine whether a secure BusObject is required.
    if (AllJoynHelpers::CanSecure(m_busAttachment->AuthenticationMechanisms))
    {
        // Register the BusObject as "secure" if the org.alljoyn.Bus.Secure XML annotation
        // is specified, or if None is not present in AuthenticationMechanisms.
        if (!authenticationMechanismsContainsNone || interfaceIsSecure)
        {
            result = alljoyn_busattachment_registerbusobject_secure(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject);
        }
        else
        {
            result = alljoyn_busattachment_registerbusobject(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject);
        }
    }
    else
    {
        // If the current set of AuthenticationMechanisms does not support authentication
        // but the interface requires security, report an error.
        if (interfaceIsSecure)
        {
            result = ER_BUS_NO_AUTHENTICATION_MECHANISM;
        }
        else
        {
            result = alljoyn_busattachment_registerbusobject(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment), BusObject);
        }
    }

    if (result != ER_OK)
    {
        StopInternal(result);
        return;
    }

    m_busAttachmentStateChangedToken = m_busAttachment->StateChanged += ref new TypedEventHandler<AllJoynBusAttachment^,AllJoynBusAttachmentStateChangedEventArgs^>(this, &fezhatnodeProducer::BusAttachmentStateChanged);
    m_busAttachment->Connect();
}

void fezhatnodeProducer::Stop()
{
    StopInternal(AllJoynStatus::Ok);
}

void fezhatnodeProducer::StopInternal(int32 status)
{
    UnregisterFromBus();
    AllJoynHelpers::DispatchEvent([=]() {
        Stopped(this, ref new AllJoynProducerStoppedEventArgs(status));
    });
}

int32 fezhatnodeProducer::RemoveMemberFromSession(_In_ String^ uniqueName)
{
    return alljoyn_busattachment_removesessionmember(
        AllJoynHelpers::GetInternalBusAttachment(m_busAttachment),
        m_sessionId,
        AllJoynHelpers::PlatformToMultibyteString(uniqueName).data());
}

PCSTR com::ianlee::fezhatnode::c_fezhatnodeIntrospectionXml = "<interface name=\"com.ianlee.fezhatnode\">"
"  <method name=\"SetRedLedState\">"
"    <arg name=\"on\" type=\"b\" direction=\"in\" />"
"  </method>"
"  <method name=\"GetRedLedState\">"
"    <arg name=\"state\" type=\"b\" direction=\"out\" />"
"  </method>"
"  <method name=\"SetRgbLedD2Color\">"
"    <arg name=\"on\" type=\"b\" direction=\"in\" />"
"    <arg name=\"red\" type=\"y\" direction=\"in\" />"
"    <arg name=\"green\" type=\"y\" direction=\"in\" />"
"    <arg name=\"blue\" type=\"y\" direction=\"in\" />"
"  </method>"
"  <method name=\"SetRgbLedD3Color\">"
"    <arg name=\"on\" type=\"b\" direction=\"in\" />"
"    <arg name=\"red\" type=\"y\" direction=\"in\" />"
"    <arg name=\"green\" type=\"y\" direction=\"in\" />"
"    <arg name=\"blue\" type=\"y\" direction=\"in\" />"
"  </method>"
"  <method name=\"GetLightSensorValue\">"
"    <arg name=\"value\" type=\"d\" direction=\"out\" />"
"  </method>"
"  <method name=\"GetTemperatureSensorValue\">"
"    <arg name=\"valueC\" type=\"d\" direction=\"out\" />"
"    <arg name=\"valueF\" type=\"d\" direction=\"out\" />"
"  </method>"
"  <signal name=\"ButtonDio18Pressed\" sessless=\"true\">"
"    <description>Emitted when the DIO18 button has been pressed.</description>"
"  </signal>"
"  <signal name=\"ButtonDio22Pressed\" sessless=\"true\">"
"    <description>Emitted when the DIO22 button has been pressed.</description>"
"  </signal>"
"</interface>"
;
