#pragma once

#include <stdint.h>
#include "Texture.h"

namespace salvation
{
    namespace asset
    {
        struct Material
        {
            // #todo Many more properties to add in the near future

            Texture m_diffuseTexture;
        };
    }
}