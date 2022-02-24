#include "test.h"
#include "ReaperExt_include_in_plug_src.h"
#include "IControls.h"
#include <iostream>  
#include<sstream>
#include <charconv>
using namespace std;

int _iterFxOnOff = 16;

int _iterFxMove = 16;
int _iterFxMoveSpaces = 16;

/// <summary>
/// Toggle on off trigger for all the selected tracks
/// </summary>
/// <param name="fxIndex"></param>
void toggleFxOnOff(int fxIndex = 0) {

  int n = CountSelectedTracks2(0, false);

  for (int i = 0; i < n; i++) {
    MediaTrack* selectedTrack = GetSelectedTrack2(0, i, false);

    bool isOffline = TrackFX_GetOffline(selectedTrack, fxIndex);

    if (isOffline) { TrackFX_SetOffline(selectedTrack, fxIndex, false); }
    else { TrackFX_SetOffline(selectedTrack, fxIndex, true); }
  }

}

/// <summary>
/// Move FX of Selected Track to another index in the same track
/// </summary>
/// <param name="fxIndex"></param>
/// <param name="fxTarget"></param>
void moveFxInTrack(int fxIndex = 0, int fxTarget = 0) {

  int n = CountSelectedTracks2(0, false);

  if (n > 0) {
    MediaTrack* selectedTrack = GetSelectedTrack2(0, 0, false);
    TrackFX_CopyToTrack(selectedTrack, fxIndex, selectedTrack, fxTarget, true);
  }

}

test::test(reaper_plugin_info_t* pRec)
  : ReaperExtBase(pRec)
{
  //Use IMPAPI to register any Reaper APIs that you need to use
  IMPAPI(GetNumTracks);
  IMPAPI(CountTracks);
  IMPAPI(CountSelectedTracks2);
  IMPAPI(GetSelectedTrack2);
  IMPAPI(TrackFX_SetOffline);
  IMPAPI(TrackFX_GetOffline);
  IMPAPI(SetToggleCommandState);
  IMPAPI(TrackFX_CopyToTrack);


  // SEL TRACKS TOGGLE FX ON

  string base = "Beto_SelTracks_FX_OnOff_";
  char** textChar = new char* [_iterFxOnOff];

  for (int i = 0; i < _iterFxOnOff; i++) {
    textChar[i] = new char[26];
  }

  for (int i = 0; i < _iterFxOnOff; i++) {
    string text = base + std::to_string(i + 1);
    strcpy(textChar[i], text.c_str());

    RegisterAction(textChar[i], [i]() {toggleFxOnOff(i); });
  }


  // FX MOVE UP DOWN

  string baseFxMove = "Beto_SelTrack0_FX_Move_";
  int charSizeFxMove = _iterFxMove * _iterFxMoveSpaces;
  char** charFxMove = new char* [charSizeFxMove];

  for (int i = 0; i < charSizeFxMove; i++) {
    charFxMove[i] = new char[28];
  }

  for (int i = 0; i < _iterFxMove; i++) {
    for (int j = 0; j < _iterFxMoveSpaces; j++) {

      if (i != j) {
        string textFxMove = baseFxMove + std::to_string(i + 1) + "_" + std::to_string(j + 1);
        int index = (i * _iterFxMoveSpaces) + j;
        strcpy(charFxMove[index], textFxMove.c_str());

        RegisterAction(charFxMove[index], [i, j]() {moveFxInTrack(i, j); });
      }

    }
  }

}


