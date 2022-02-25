/******************************************************************************
/ betoOSC.cpp
/
/ Copyright (c) 2012 Tim Payne (SWS) 2022 Alberto Valdez (betoOSC)
/
/
/ Permission is hereby granted, free of charge, to any person obtaining a copy
/ of this software and associated documentation files (the "Software"), to deal
/ in the Software without restriction, including without limitation the rights to
/ use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
/ of the Software, and to permit persons to whom the Software is furnished to
/ do so, subject to the following conditions:
/
/ The above copyright notice and this permission notice shall be included in all
/ copies or substantial portions of the Software.
/
/ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
/ OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
/ NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
/ HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
/ WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/ FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
/ OTHER DEALINGS IN THE SOFTWARE.
/
******************************************************************************/

#include "betoOSC.h"
#include "ReaperExt_include_in_plug_src.h"
#include "IControls.h"
#include <iostream>  
#include<sstream>
#include <charconv>

using namespace std;


/// <summary>
/// Toggle on off trigger for all the selected tracks
/// </summary>
/// <param name="fxIndex"></param>
void toggleFxOnOff(int fxIndex) {

    int n = CountSelectedTracks2(0, false);

    for (int k = 0; k < n; k++) {
        MediaTrack* selectedTrack = GetSelectedTrack2(0, k, false);

        bool isOffline = TrackFX_GetOffline(selectedTrack, fxIndex);

        if (isOffline) { 
            TrackFX_SetOffline(selectedTrack, fxIndex, false); 
        }
        else { 
            TrackFX_SetOffline(selectedTrack, fxIndex, true); 
        }
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
/// <summary>
/// This code comes from SWS MarkerListActions.cpp
/// </summary>
void SelPrevRegion()
{
    double dCurPos, d2;
    GetSet_LoopTimeRange(false, true, &dCurPos, &d2, false);
    if (dCurPos == d2)
        dCurPos = GetCursorPosition();

    int x = 0;
    bool bReg;
    double d1, dRegStart, dRegEnd;
    bool bFound = false;
    bool bRegions = false;
    while ((x = EnumProjectMarkers(x, &bReg, &d1, &d2, NULL, NULL)))
    {
        if (bReg)
        {
            bRegions = true;
            if (dCurPos > d1)
            {
                dRegStart = d1;
                dRegEnd = d2;
                bFound = true;
            }
        }
    }
    if (bFound)
        GetSet_LoopTimeRange(true, true, &dRegStart, &dRegEnd, false);
    else if (bRegions)
        GetSet_LoopTimeRange(true, true, &d1, &d2, false);
}

/// <summary>
/// This code comes from SWS MarkerListActions.cpp
/// </summary>
/// <param name=""></param>
void SelNextRegion()
{
    double dCurPos, d2;
    GetSet_LoopTimeRange(false, true, &dCurPos, &d2, false);
    if (dCurPos == d2)
        dCurPos = GetCursorPosition();

    int x = 0;
    bool bReg;
    double dRegStart;
    double dRegEnd;
    while ((x = EnumProjectMarkers(x, &bReg, &dRegStart, &dRegEnd, NULL, NULL)))
    {
        if (bReg && dRegStart > dCurPos)
        {
            GetSet_LoopTimeRange(true, true, &dRegStart, &dRegEnd, false);
            return;
        }
    }
    // Nothing found, loop again and set to first region
    while ((x = EnumProjectMarkers(x, &bReg, &dRegStart, &dRegEnd, NULL, NULL)))
    {
        if (bReg)
        {
            GetSet_LoopTimeRange(true, true, &dRegStart, &dRegEnd, false);
            return;
        }
    }
}


void CollapseFolder(bool collapse, const char* actionName)
{
    int n = CountSelectedTracks2(0, false);

    for (int k = 0; k < n; k++) {
        MediaTrack* selectedTrack = GetSelectedTrack2(0, k, false);

        //int depth = GetMediaTrackInfo_Value(selectedTrack, "I_FOLDERDEPTH");
        //double compact = GetMediaTrackInfo_Value(selectedTrack, "I_FOLDERCOMPACT");

        if (collapse) {
            SetMediaTrackInfo_Value(selectedTrack, "I_FOLDERCOMPACT", 2);
            Main_OnCommand(41665, (double)0); // Mixer: Toggle Children of track view in mcp
        }
        else {
            SetMediaTrackInfo_Value(selectedTrack, "I_FOLDERCOMPACT", 0);
            Main_OnCommand(41665, (double)0);
        }
    }

    //SetToggleCommandState(0, NamedCommandLookup(actionName), 1);

    //int actionID = GetToggleCommandState(NamedCommandLookup(actionName));
    //char actionChar[8];

    //string actionText = std::to_string(actionID);
    //strcpy(actionChar, actionText.c_str());

    //MB(actionChar, "Command ID", 0);

}


betoOSC::betoOSC(reaper_plugin_info_t* pRec)
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
    IMPAPI(GetSet_LoopTimeRange);
    IMPAPI(GetCursorPosition);
    IMPAPI(EnumProjectMarkers);
    IMPAPI(NamedCommandLookup);
    IMPAPI(SetToggleCommandState);
    IMPAPI(UpdateTimeline);
    IMPAPI(GetMediaTrackInfo_Value);
    IMPAPI(SetMediaTrackInfo_Value);
    IMPAPI(Main_OnCommand);
    IMPAPI(GetToggleCommandState);
    IMPAPI(MB);

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

    // Select Previous Region

    RegisterAction("Beto_Region_Select_Previous", []() {SelPrevRegion(); } );

    // Select Next Region

    RegisterAction("Beto_Region_Select_Next", []() {SelNextRegion(); });

    // To do find command ID and enable states on off

    // Collapse Folder
    RegisterAction("Beto_SelTracks_Folder_Collapse", []() {
        CollapseFolder(true, "_Beto_SelTracks_Folder_Collapse"); } );

    // UnCollapse Folder

    RegisterAction("Beto_SelTracks_Folder_UnCollapse", []() {
        CollapseFolder(false, "_Beto_SelTracks_Folder_UnCollapse"); });

    
}


