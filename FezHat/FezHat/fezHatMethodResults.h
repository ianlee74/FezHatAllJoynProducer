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
#pragma once

namespace com { namespace ianlee { namespace fezHat {

ref class fezHatConsumer;

public ref class fezHatSwitchD2Result sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    
    static fezHatSwitchD2Result^ CreateSuccessResult()
    {
        auto result = ref new fezHatSwitchD2Result();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        return result;
    }
    
    static fezHatSwitchD2Result^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new fezHatSwitchD2Result();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
};

public ref class fezHatSwitchD3Result sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    
    static fezHatSwitchD3Result^ CreateSuccessResult()
    {
        auto result = ref new fezHatSwitchD3Result();
        result->Status = Windows::Devices::AllJoyn::AllJoynStatus::Ok;
        return result;
    }
    
    static fezHatSwitchD3Result^ CreateFailureResult(_In_ int32 status)
    {
        auto result = ref new fezHatSwitchD3Result();
        result->Status = status;
        return result;
    }

private:
    int32 m_status;
};

public ref class fezHatJoinSessionResult sealed
{
public:
    property int32 Status
    {
        int32 get() { return m_status; }
    internal:
        void set(_In_ int32 value) { m_status = value; }
    }

    property fezHatConsumer^ Consumer
    {
        fezHatConsumer^ get() { return m_consumer; }
    internal:
        void set(_In_ fezHatConsumer^ value) { m_consumer = value; }
    };

private:
    int32 m_status;
    fezHatConsumer^ m_consumer;
};

} } } 
