/*++

Copyright (c) 2005  Future Technology Devices International Ltd.

Module Name:

    ftcjtag.cpp

Abstract:

    API DLL for FT2232H and FT4232H Hi-Speed Dual Device and FT2232D Dual Device setup to simulate the
    Joint Test Action Group(JTAG) synchronous serial protocol.
    Defines the entry point for the DLL application.

Environment:

    kernel & user mode

Revision History:

    07/02/05    kra     Created.
    24/08/05    kra     Added new function JTAG_GenerateClockPulses and new error code FTC_INVALID_NUMBER_CLOCK_PULSES
    16/09/05    kra     Added break statements after DLL_THREAD_ATTACH and DLL_THREAD_DETACH for multiple threaded applications
    07/07/08    kra     Added new functions for FT2232H and FT4232H hi-speed devices.
    19/08/08    kra     Added new function JTAG_CloseDevice.
	
--*/

#include "stdafx.h"

#include <windows.h>

#include "ftcjtag.h"

#include "ft2232hmpssejtag.h"

static FT2232hMpsseJtag *pFT2232hMpsseJtag = NULL;

#include "stdafx.h"

#include <time.h>
#include <cstdio>

static FILE* g_pLogFile = NULL;
static const char* g_szDefaultLog = "dll_debug.log";
void InitDllLog(const char* logFileName)
{
    const char* szLogName = logFileName ? logFileName : g_szDefaultLog;
    if (fopen_s(&g_pLogFile, szLogName, "a+") != 0) {
        OutputDebugStringA("DLL日志打开失败！\n");
        g_pLogFile = NULL;
    }
}

void CloseDllLog()
{
    if (g_pLogFile != NULL) {
        fclose(g_pLogFile);
        g_pLogFile = NULL;
    }
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
  	{
		case DLL_PROCESS_ATTACH:
            InitDllLog(NULL);
      if (pFT2232hMpsseJtag == NULL)
        pFT2232hMpsseJtag = new FT2232hMpsseJtag();
      break;
		case DLL_THREAD_ATTACH:
      break;
		case DLL_THREAD_DETACH:
      break;
		case DLL_PROCESS_DETACH:
            CloseDllLog();
      pFT2232hMpsseJtag->~FT2232hMpsseJtag();
		  break;
    }

    return TRUE;
}

//---------------------------------------------------------------------------

static void DLLPrint(const char* fmt, ...)
{
    char szBuf[1024] = { 0 };
    va_list args;
    va_start(args, fmt);
    vsnprintf_s(szBuf, sizeof(szBuf), _TRUNCATE, fmt, args);
    va_end(args);

    OutputDebugStringA(szBuf);
    OutputDebugStringA("\n");
    if (g_pLogFile != NULL)
    {
        time_t tNow = time(NULL);
        struct tm* pTm = localtime(&tNow);
        fprintf(g_pLogFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec, szBuf);
        fflush(g_pLogFile);
    }
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetNumDevices(LPDWORD lpdwNumDevices)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_GetNumDevices(lpdwNumDevices);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetNumHiSpeedDevices(LPDWORD lpdwNumHiSpeedDevices)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_GetNumHiSpeedDevices(lpdwNumHiSpeedDevices);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetDeviceNameLocID(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID)
{
    DLLPrint("%s: dwDeviceNameIndex:%d\n", __func__, dwDeviceNameIndex);
  return pFT2232hMpsseJtag->JTAG_GetDeviceNameLocationID(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetHiSpeedDeviceNameLocIDChannel(DWORD dwDeviceNameIndex, LPSTR lpDeviceNameBuffer, DWORD dwBufferSize, LPDWORD lpdwLocationID, LPSTR lpChannel, DWORD dwChannelBufferSize, LPDWORD lpdwHiSpeedDeviceType)
{
    DLLPrint("%s: dwDeviceNameIndex:%d, lpdwLocationID:%d, lpChannel:%d, lpdwHiSpeedDeviceType:%s\n", __func__, dwDeviceNameIndex, lpdwLocationID, lpChannel, lpdwHiSpeedDeviceType);
  return pFT2232hMpsseJtag->JTAG_GetHiSpeedDeviceNameLocationIDChannel(dwDeviceNameIndex, lpDeviceNameBuffer, dwBufferSize, lpdwLocationID, lpChannel, dwChannelBufferSize, lpdwHiSpeedDeviceType);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_Open(FTC_HANDLE *pftHandle)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_OpenDevice(pftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_OpenEx(LPSTR lpDeviceName, DWORD dwLocationID, FTC_HANDLE *pftHandle)
{
    DLLPrint("%s: lpDeviceName:%s, dwLocationID:%d\n", __func__, lpDeviceName, dwLocationID);
  return pFT2232hMpsseJtag->JTAG_OpenSpecifiedDevice(lpDeviceName, dwLocationID, pftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_OpenHiSpeedDevice(LPSTR lpDeviceName, DWORD dwLocationID, LPSTR lpChannel, FTC_HANDLE *pftHandle)
{
    DLLPrint("%s: lpDeviceName:%s, dwLocationID:%d, lpChannel:%s\n", __func__, lpDeviceName, dwLocationID, lpChannel);
  return pFT2232hMpsseJtag->JTAG_OpenSpecifiedHiSpeedDevice(lpDeviceName, dwLocationID, lpChannel, pftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetHiSpeedDeviceType(FTC_HANDLE ftHandle, LPDWORD lpdwHiSpeedDeviceType)
{
  return pFT2232hMpsseJtag->JTAG_GetHiSpeedDeviceType(ftHandle, lpdwHiSpeedDeviceType);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_Close(FTC_HANDLE ftHandle)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_CloseDevice(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_CloseDevice(FTC_HANDLE ftHandle, PFTC_CLOSE_FINAL_STATE_PINS pCloseFinalStatePinsData)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_CloseDevice(ftHandle, pCloseFinalStatePinsData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_InitDevice(FTC_HANDLE ftHandle, DWORD dwClockDivisor)
{
    DLLPrint("%s: dwClockDivisor:%d\n", __func__, dwClockDivisor);
  return pFT2232hMpsseJtag->JTAG_InitDevice(ftHandle, dwClockDivisor);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_TurnOnDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_TurnOnDivideByFiveClockingHiSpeedDevice(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_TurnOffDivideByFiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_TurnOffDivideByFiveClockingHiSpeedDevice(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_TurnOnAdaptiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_TurnOnAdaptiveClockingHiSpeedDevice(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_TurnOffAdaptiveClockingHiSpeedDevice(FTC_HANDLE ftHandle)
{
    DLLPrint("%s\n", __func__);

  return pFT2232hMpsseJtag->JTAG_TurnOffAdaptiveClockingHiSpeedDevice(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_SetDeviceLatencyTimer(FTC_HANDLE ftHandle, BYTE timerValue)
{
    DLLPrint("%s: timerValue:%d\n", __func__, timerValue);

  return pFT2232hMpsseJtag->JTAG_SetDeviceLatencyTimer(ftHandle, timerValue);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetDeviceLatencyTimer(FTC_HANDLE ftHandle, LPBYTE lpTimerValue)
{
    DLLPrint("%s: lpTimerValue:%d\n", __func__, lpTimerValue);
  return pFT2232hMpsseJtag->JTAG_GetDeviceLatencyTimer(ftHandle, lpTimerValue);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
    DLLPrint("%s:lpdwClockFrequencyHz:%d\n", __func__, lpdwClockFrequencyHz);
  return pFT2232hMpsseJtag->JTAG_GetClock(dwClockDivisor, lpdwClockFrequencyHz);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetHiSpeedDeviceClock(DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
    DLLPrint("%s:lpdwClockFrequencyHz:%d\n", __func__, lpdwClockFrequencyHz);
  return pFT2232hMpsseJtag->JTAG_GetHiSpeedDeviceClock(dwClockDivisor, lpdwClockFrequencyHz);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_SetClock(FTC_HANDLE ftHandle, DWORD dwClockDivisor, LPDWORD lpdwClockFrequencyHz)
{
    DLLPrint("%s:dwClockDivisor:%d lpdwClockFrequencyHz:%d\n", __func__, dwClockDivisor, lpdwClockFrequencyHz);
  return pFT2232hMpsseJtag->JTAG_SetClock(ftHandle, dwClockDivisor, lpdwClockFrequencyHz);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_SetLoopback(FTC_HANDLE ftHandle, BOOL bLoopbackState)
{
    DLLPrint("%s: bLoopbackState:%d\n", __func__, bLoopbackState);
  return pFT2232hMpsseJtag->JTAG_SetDeviceLoopbackState(ftHandle, bLoopbackState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_SetGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                PFTC_INPUT_OUTPUT_PINS pLowInputOutputPinsData,
                                BOOL bControlHighInputOutputPins,
                                PFTC_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
    DLLPrint("%s\n", __func__);

  return pFT2232hMpsseJtag->JTAG_SetGeneralPurposeInputOutputPins(ftHandle, bControlLowInputOutputPins,
                                                                  pLowInputOutputPinsData, bControlHighInputOutputPins,
                                                                  pHighInputOutputPinsData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_SetHiSpeedDeviceGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                             PFTC_INPUT_OUTPUT_PINS pLowInputOutputPinsData,
                                             BOOL bControlHighInputOutputPins,
                                             PFTH_INPUT_OUTPUT_PINS pHighInputOutputPinsData)
{
    DLLPrint("%s\n", __func__);

  return pFT2232hMpsseJtag->JTAG_SetHiSpeedDeviceGeneralPurposeInputOutputPins(ftHandle, bControlLowInputOutputPins,
                                                                               pLowInputOutputPinsData, bControlHighInputOutputPins,
                                                                               pHighInputOutputPinsData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                PFTC_LOW_HIGH_PINS pLowPinsInputData,
                                BOOL bControlHighInputOutputPins,
                                PFTC_LOW_HIGH_PINS pHighPinsInputData)
{
    DLLPrint("%s\n", __func__);

  return pFT2232hMpsseJtag->JTAG_GetGeneralPurposeInputOutputPins(ftHandle, bControlLowInputOutputPins,
                                                                  pLowPinsInputData, bControlHighInputOutputPins,
                                                                  pHighPinsInputData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetHiSpeedDeviceGPIOs(FTC_HANDLE ftHandle, BOOL bControlLowInputOutputPins,
                                             PFTC_LOW_HIGH_PINS pLowPinsInputData,
                                             BOOL bControlHighInputOutputPins,
                                             PFTH_LOW_HIGH_PINS pHighPinsInputData)
{
    DLLPrint("%s\n", __func__); 
    return pFT2232hMpsseJtag->JTAG_GetHiSpeedDeviceGeneralPurposeInputOutputPins(ftHandle, bControlLowInputOutputPins,
                                                                               pLowPinsInputData, bControlHighInputOutputPins,
                                                                               pHighPinsInputData);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_Write(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                             PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                             DWORD dwTapControllerState)
{
    DLLPrint("%s: bInstructionTestData:%d dwNumBitsToWrite:%d dwNumBytesToWrite:%d dwTapControllerState:%d pWriteDataBuffer:", __func__, bInstructionTestData, dwNumBitsToWrite, dwNumBytesToWrite, dwTapControllerState);
    for (size_t i = 0; i < dwNumBytesToWrite; i++)
    {
        DLLPrint("%#x ", pWriteDataBuffer[i]);
    }
    DLLPrint("\n");

  return pFT2232hMpsseJtag->JTAG_WriteDataToExternalDevice(ftHandle, bInstructionTestData, dwNumBitsToWrite,
                                                           pWriteDataBuffer, dwNumBytesToWrite, dwTapControllerState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_Read(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead,
                            PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
                            DWORD dwTapControllerState)
{
    FTC_STATUS ret = 0;
    ret = pFT2232hMpsseJtag->JTAG_ReadDataFromExternalDevice(ftHandle, bInstructionTestData, dwNumBitsToRead,
        pReadDataBuffer, lpdwNumBytesReturned, dwTapControllerState);
    DLLPrint("%s: bInstructionTestData:%d dwNumBitsToRead:%d lpdwNumBytesReturned:%d dwTapControllerState:%d pReadDataBuffer:", __func__, bInstructionTestData, dwNumBitsToRead, *lpdwNumBytesReturned, dwTapControllerState);
    for (size_t i = 0; i < *lpdwNumBytesReturned; i++)
    {
        DLLPrint("%#x ", pReadDataBuffer[i]);
    }
    DLLPrint("\n");

    return ret;
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_WriteRead(FTC_HANDLE ftHandle, BOOL bInstructionData, DWORD dwNumBitsToWriteRead,
                                 PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                 PReadDataByteBuffer pReadDataBuffer, LPDWORD lpdwNumBytesReturned,
                                 DWORD dwTapControllerState)
{
    FTC_STATUS ret = 0;

  ret = pFT2232hMpsseJtag->JTAG_WriteReadDataToFromExternalDevice(ftHandle, bInstructionData, dwNumBitsToWriteRead,
                                                                   pWriteDataBuffer,  dwNumBytesToWrite,
                                                                   pReadDataBuffer, lpdwNumBytesReturned, dwTapControllerState);
  DLLPrint("%s: bInstructionData:%d dwNumBitsToWriteRead:%d dwNumBytesToWrite:%d lpdwNumBytesReturned:%d dwTapControllerState:%d\npWriteDataBuffer:", __func__, bInstructionData, dwNumBitsToWriteRead, dwNumBytesToWrite, *lpdwNumBytesReturned, dwTapControllerState);
  for (size_t i = 0; i < dwNumBytesToWrite; i++)
  {
      DLLPrint("%#x ", pWriteDataBuffer[i]);
  }
  DLLPrint("\npReadDataBuffer:");
  for (size_t i = 0; i < *lpdwNumBytesReturned; i++)
  {
      DLLPrint("%#x ", pReadDataBuffer[i]);
  }
  DLLPrint("\n");
  return ret;
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GenerateClockPulses(FTC_HANDLE ftHandle, DWORD dwNumClockPulses)
{
    DLLPrint("%s: dwNumClockPulses:%d\n", __func__, dwNumClockPulses);
  return pFT2232hMpsseJtag->JTAG_GenerateTCKClockPulses(ftHandle, dwNumClockPulses);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GenerateClockPulsesHiSpeedDevice(FTC_HANDLE ftHandle, BOOL bPulseClockTimesEightFactor, DWORD dwNumClockPulses, BOOL bControlLowInputOutputPin, BOOL bStopClockPulsesState)
{
    DLLPrint("%s: bPulseClockTimesEightFactor:%d dwNumClockPulses:%d bControlLowInputOutputPin:%d bStopClockPulsesState:%d\n", __func__, bPulseClockTimesEightFactor, dwNumClockPulses, bControlLowInputOutputPin, bStopClockPulsesState);
  return pFT2232hMpsseJtag->JTAG_GenerateClockPulsesHiSpeedDevice(ftHandle, bPulseClockTimesEightFactor, dwNumClockPulses, bControlLowInputOutputPin, bStopClockPulsesState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_ClearCmdSequence(void)
{
    DLLPrint("%s\n", __func__);
  return pFT2232hMpsseJtag->JTAG_ClearCommandSequence();
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_AddWriteCmd(BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                   PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                   DWORD dwTapControllerState)
{
    DLLPrint("%s: bInstructionTestData:%d dwNumBitsToWrite:%d dwNumBytesToWrite:%d dwTapControllerState:%d pWriteDataBuffer:\n", __func__, bInstructionTestData, dwNumBitsToWrite, dwNumBytesToWrite, dwTapControllerState);
    for (size_t i = 0; i < dwNumBytesToWrite; i++)
    {
        DLLPrint("%#x ", pWriteDataBuffer[i]);
    }
    DLLPrint("\n");
  return pFT2232hMpsseJtag->JTAG_AddWriteCommand(bInstructionTestData, dwNumBitsToWrite, pWriteDataBuffer,
                                                 dwNumBytesToWrite, dwTapControllerState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_AddReadCmd(BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState)
{
    DLLPrint("%s: bInstructionTestData:%d dwNumBitsToRead:%d dwTapControllerState:%d\n", __func__, bInstructionTestData, dwNumBitsToRead, dwTapControllerState);
    return pFT2232hMpsseJtag->JTAG_AddReadCommand(bInstructionTestData, dwNumBitsToRead, dwTapControllerState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_AddWriteReadCmd(BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                       PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                       DWORD dwTapControllerState)
{
    FTC_STATUS ret = 0;

  ret = pFT2232hMpsseJtag->JTAG_AddWriteReadCommand(bInstructionTestData, dwNumBitsToWriteRead, pWriteDataBuffer,
                                                     dwNumBytesToWrite, dwTapControllerState);
  
  DLLPrint("%s: bInstructionTestData:%d dwNumBitsToWriteRead:%d dwNumBytesToWrite:%d dwTapControllerState:%d pWriteDataBuffer:\n", __func__, bInstructionTestData, dwNumBitsToWriteRead, dwNumBytesToWrite, dwTapControllerState);
  for (size_t i = 0; i < dwNumBytesToWrite; i++)
  {
      DLLPrint("%#x ", pWriteDataBuffer[i]);
  }
  DLLPrint("\n");
  return ret;
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_ClearDeviceCmdSequence(FTC_HANDLE ftHandle)
{
    DLLPrint("%s\n", __func__);

  return pFT2232hMpsseJtag->JTAG_ClearDeviceCommandSequence(ftHandle);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_AddDeviceWriteCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWrite,
                                         PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                         DWORD dwTapControllerState)
{
    DLLPrint("%s: bInstructionTestData:%d dwNumBitsToWrite:%d dwNumBytesToWrite:%d dwTapControllerState:%d pWriteDataBuffer:\n", __func__, bInstructionTestData, dwNumBitsToWrite, dwNumBytesToWrite, dwTapControllerState);
    for (size_t i = 0; i < dwNumBytesToWrite; i++)
    {
        DLLPrint("%#x ", pWriteDataBuffer[i]);
    }
    DLLPrint("\n");
    return pFT2232hMpsseJtag->JTAG_AddDeviceWriteCommand(ftHandle, bInstructionTestData, dwNumBitsToWrite,
                                                       pWriteDataBuffer, dwNumBytesToWrite, dwTapControllerState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_AddDeviceReadCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToRead, DWORD dwTapControllerState)
{
    DLLPrint("%s: bInstructionTestData:%d dwNumBitsToRead:%d dwTapControllerState:%d\n", __func__, bInstructionTestData, dwNumBitsToRead, dwTapControllerState);

    return pFT2232hMpsseJtag->JTAG_AddDeviceReadCommand(ftHandle, bInstructionTestData, dwNumBitsToRead, dwTapControllerState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_AddDeviceWriteReadCmd(FTC_HANDLE ftHandle, BOOL bInstructionTestData, DWORD dwNumBitsToWriteRead,
                                             PWriteDataByteBuffer pWriteDataBuffer, DWORD dwNumBytesToWrite,
                                             DWORD dwTapControllerState)
{
    DLLPrint("%s: bInstructionTestData:%d dwNumBitsToWriteRead:%d dwNumBytesToWrite:%d dwTapControllerState:%d pWriteDataBuffer:\n", __func__, bInstructionTestData, dwNumBitsToWriteRead, dwNumBytesToWrite, dwTapControllerState);
    for (size_t i = 0; i < dwNumBytesToWrite; i++)
    {
        DLLPrint("%#x ", pWriteDataBuffer[i]);
    }
    DLLPrint("\n");

    return pFT2232hMpsseJtag->JTAG_AddDeviceWriteReadCommand(ftHandle, bInstructionTestData, dwNumBitsToWriteRead, pWriteDataBuffer,
                                                           dwNumBytesToWrite, dwTapControllerState);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_ExecuteCmdSequence(FTC_HANDLE ftHandle, PReadCmdSequenceDataByteBuffer pReadCmdSequenceDataBuffer,
                                          LPDWORD lpdwNumBytesReturned)
{
    FTC_STATUS ret = 0;
    DLLPrint("%s: ", __func__);

  ret = pFT2232hMpsseJtag->JTAG_ExecuteCommandSequence(ftHandle, pReadCmdSequenceDataBuffer, lpdwNumBytesReturned);
  for (size_t i = 0; i < *lpdwNumBytesReturned; i++)
  {
      DLLPrint("%#x ", pReadCmdSequenceDataBuffer[i]);
  }
  DLLPrint("\n");

  return ret;
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetDllVersion(LPSTR lpDllVersionBuffer, DWORD dwBufferSize)
{
    DLLPrint("%s: dwBufferSize:%d lpDllVersionBuffer:", __func__, dwBufferSize);
    for (size_t i = 0; i < dwBufferSize; i++)
    {
        DLLPrint("%#x ", lpDllVersionBuffer[i]);
    }DLLPrint("\n");
  return pFT2232hMpsseJtag->JTAG_GetDllVersion(lpDllVersionBuffer, dwBufferSize);
}

extern "C" __declspec(dllexport)
FTC_STATUS WINAPI JTAG_GetErrorCodeString(LPSTR lpLanguage, FTC_STATUS StatusCode, LPSTR lpErrorMessageBuffer, DWORD dwBufferSize)
{
    FTC_STATUS ret = 0;

  ret = pFT2232hMpsseJtag->JTAG_GetErrorCodeString(lpLanguage, StatusCode, lpErrorMessageBuffer, dwBufferSize);
  DLLPrint("%s: lpLanguage:%s StatusCode:%#x dwBufferSize:%d lpErrorMessageBuffer:%s", lpLanguage, StatusCode, dwBufferSize, lpErrorMessageBuffer);
  return ret;
}
//---------------------------------------------------------------------------

