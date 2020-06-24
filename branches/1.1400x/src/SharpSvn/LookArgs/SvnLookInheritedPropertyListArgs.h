#pragma once

namespace SharpSvn {

    /// <summary>Extended Parameter container of <see cref="SvnLookClient" />'s InheritedPropertyList method</summary>
    /// <threadsafety static="true" instance="false"/>
    public ref class SvnLookInheritedPropertyListArgs : public SvnClientArgs
    {
    public:
        SvnLookInheritedPropertyListArgs()
        {
        }

    public:
        DECLARE_EVENT(SvnLookInheritedPropertyListEventArgs^, InheritedPropertyList);

    protected public:
        virtual void OnInheritedPropertyList(SvnLookInheritedPropertyListEventArgs^ e)
        {
            InheritedPropertyList(this, e);
        }

    public:
        virtual property SvnCommandType CommandType
        {
            virtual SvnCommandType get() override sealed
            {
                return SvnCommandType::Unknown;
            }
        }
    };
}
