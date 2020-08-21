#pragma once
#include <metahost.h>
#include "XorStr.h"
#include "D3D.h"
#include "Esp.h"
#include "Bh.h"
#include "Wh.h"
#include "HitBox.h"
#include "PatcherC.h"
#include "ForDebug.h"
#include "ReaderWriterBin.h"
#include "protector.h"
#include "BasicPlayerInfo.h"
#include "Skin.h"
#include "Textures.h"
#include "CLTClient.h"

HRESULT hr;
ICLRMetaHost* pMetaHost = NULL;
ICLRRuntimeInfo* pRuntimeInfo = NULL;
ICLRRuntimeHost* pClrRuntimeHost = NULL;

Patcher_c* p;

LPVOID Hook_GetWpnById(int16_t idW)
{
	LPVOID res = fGetWpnById(p->AdrOfGetWpnById)(idW);
	PWeapon changedWpn = p->AllWpnsOriginals[idW];
	if (changedWpn != nullptr) return changedWpn;
	return res;
}
void TurnOnSkinChanger()
{
	SetPatches(p, (DWORD)(&Hook_GetWpnById));
	AddABanchOfWpns(p);
}


typedef PBYTE(*fGetModelById)(int32_t id);
HitBox::MyModelForHitBoxes* mn;
void setHitBoxAreaDims(int nodeIndex, float x, float y, float z) {
	fGetModelById f = (fGetModelById)(mn->origGetModelById);
	HitBox::__valueChanging((PBYTE)f(nodeIndex), x, y, z);
}
void setHitBoxAreaMul(int nodeIndex, float mul) {
	fGetModelById f = (fGetModelById)(mn->origGetModelById);
	HitBox::Node* nPtr = (HitBox::Node*)mn->Backup[nodeIndex];
	float dimsX = nPtr->DimensionX;
	float dimsY = nPtr->DimensionY;
	float dimsZ = nPtr->DimensionZ;
	HitBox::__valueChanging((PBYTE)f(nodeIndex), dimsX * mul, dimsY * mul, dimsZ * mul);
}
void* hook_GetNodeById(int32_t arg)
{
	if (arg < 23) return mn->Backup[arg];
	typedef PBYTE(*fGetModelById)(int32_t id);
	fGetModelById ret = (fGetModelById)(mn->origGetModelById);
	return ret(arg);
}
void TurnOnHitBoxes(const std::vector<int>& offs)
{
	mn = HitBox::HitBoxes(offs, (DWORD)(&hook_GetNodeById));
}
void restoreHitBoxValues() {
	for (size_t i = 0; i < 23; i++)
	{
		HitBox::Node* nPtr = (HitBox::Node*)mn->Backup[i];
		setHitBoxAreaDims(i, nPtr->DimensionX, nPtr->DimensionY, nPtr->DimensionZ);
	}
}
void TurnOffHitBoxes(bool restore = false)
{
	if (restore) restoreHitBoxValues();
	HitBox::SetDetour((DWORD)(&(mn->origGetModelById)));
}


BPI::BPIForHook* bpi;
void setDefuseTime(float seconds) {
	BPI::bpiData->C4DefuseTime = seconds;
}
void setSpeedHackOnShift(float speed) {
	BPI::bpiData->WalkRate = speed;
}
void setAcceleration(float speed) {
	BPI::bpiData->Acceleration = speed;
}
void setFriction(float speed) {
	BPI::bpiData->Friction = speed;
}
void setSideRate(float speed) {
	BPI::bpiData->SideMoveRate = speed;
}
void* hook_GetBPIById(int32_t arg)
{
	if (!arg) return bpi->backup[arg];
	return 0;
}
void TurnOnBPI(const std::vector<int>& offs)
{
	bpi = BPI::Init(offs);
	BPI::SetDetour((DWORD)(&hook_GetBPIById));
}
void restoreBPIValues() {
	auto orig = ((BPI::BasicPlayerInfo*)(bpi->backup[0]));
	setDefuseTime(orig->C4DefuseTime);
	setSpeedHackOnShift(orig->WalkRate);
	setAcceleration(orig->Acceleration);
	setFriction(orig->Friction);
	setSideRate(orig->SideMoveRate);
}
void TurnOffBPI(bool restore = false) {
	if (restore) restoreBPIValues();
	BPI::SetDetour((DWORD)(&(bpi->origFunc)));
}



Wh::WH* wh = NULL;
Wh::WH* TurnOnWH(const std::vector<int>& offs)
{
	return Wh::MakeWhDetour(offs, &Wh::__hkDIP);
}
void TurnOnCh()
{
	D3D::Constructor(XorStr("CROSSFIRE"));
}
void turnDrawOutline(bool flag) {
	Wh::espFlag = flag;
}
void turnWallHack(bool flag) {
	if (wh != NULL) {
		wh->IsOn = flag;
	}
	turnDrawOutline(flag);
}
void turnCrossHair(bool flag) {
	D3D::IsDrawCH = flag;
}