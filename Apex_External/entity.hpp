#pragma once
#include "Windows.h"
#include "utils/offsets.hpp"
#include "driver/Driver.h"

DWORD64 GetEntityById(int Entity, DWORD64 Base)
{
	DWORD64 EntityList = Base + OFF_ENTITYLIST;
	DWORD64 BaseEntity = drv.RPM<DWORD64>(EntityList);

	if (!BaseEntity)
		return NULL;
	return drv.RPM<DWORD64>(EntityList + (Entity << 5));