#pragma once

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class TimepipeService
        : public ServiceBase<TimepipeServiceInterface>
    {
    public:
        TimepipeService();
        ~TimepipeService() override;

    public:
        uint32_t addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentPtr & _doc ) override;
        TimepipeInterfacePtr removeTimepipe( uint32_t _id ) override;

    public:
        void tick( const UpdateContext * _context ) override;

    protected:
        struct TimepipeDesc
        {
            uint32_t id;
            TimepipeInterfacePtr timepipe;
            DocumentPtr doc;
        };

        typedef Vector<TimepipeDesc> VectorTimepipe;
        VectorTimepipe m_timepipeAdd;
        VectorTimepipe m_timepipe;
    };
};
