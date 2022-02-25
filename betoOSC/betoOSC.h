#pragma once

#include "ReaperExt_include_in_plug_hdr.h"

enum EControlTags
{
  kCtrlTagText = 0,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class betoOSC : public ReaperExtBase
{
public:
  betoOSC(reaper_plugin_info_t* pRec);
  
private:
	int _iterFxOnOff = 16;
	int _iterFxMove = 16;
	int _iterFxMoveSpaces = 16;
};

