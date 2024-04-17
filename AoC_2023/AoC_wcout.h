#pragma once

#include "AoC_2023.h"
#include "AoC_StreamBuff.h"

class TWout_Wrapper {
private:
   TWStreamWrapper wrapper;
public:
   TWout_Wrapper() = delete;
   TWout_Wrapper(std::wostream& stream) : wrapper(stream) { 
      wrapper.Activate(AoC_2023::output);
      }
   TWout_Wrapper(TWout_Wrapper const&) = delete;
   TWout_Wrapper(TWout_Wrapper&&) = default;
   ~TWout_Wrapper() { };

};