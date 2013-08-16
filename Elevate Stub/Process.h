#pragma once


void CheckSuitability();

void StartIpcCommunication(TCHAR* lpCmdLine);

void ReceiveIpcData();

void PrepareForLaunching();

void CreateTargetProcess();

void FinishIpcCommunication();

void ResumeAndWaitTargetProcess();
