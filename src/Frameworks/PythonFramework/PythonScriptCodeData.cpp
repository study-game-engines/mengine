#include "PythonScriptCodeData.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScriptCodeData::PythonScriptCodeData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptCodeData::~PythonScriptCodeData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptCodeData::acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptCodeData::release()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScriptCodeData::setScriptCode( const pybind::object & _moduleCode )
    {
        m_moduleCode = _moduleCode;
    }
    //////////////////////////////////////////////////////////////////////////
    const pybind::object & PythonScriptCodeData::getScriptCode() const
    {
        return m_moduleCode;
    }
}