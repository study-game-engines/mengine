#   include "C11TimerSystem.h"

#	include <chrono>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimerSystem, Menge::C11TimerSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    C11TimerSystem::C11TimerSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    C11TimerSystem::~C11TimerSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool C11TimerSystem::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void C11TimerSystem::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t C11TimerSystem::getMilliseconds()
    {
		std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point::duration epoch = tp.time_since_epoch();

		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

		uint64_t ms64 = (uint64_t)ms.count();

		return ms64;
    }
}
