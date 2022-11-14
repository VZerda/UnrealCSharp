﻿#pragma once

#include "FContainerPropertyDescriptor.h"

class FArrayPropertyDescriptor final : public FContainerPropertyDescriptor
{
public:
	using FContainerPropertyDescriptor::FContainerPropertyDescriptor;

public:
	virtual void Get(void* Src, void** Dest) const override;

	virtual void Set(void* Src, void* Dest) const override;
};
