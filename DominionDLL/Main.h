/*
Main.h
Written by Matthew Fisher

Main.h is included by all source files and includes every header file in the correct order.
*/
#pragma once

//
// Config.h includes a series of #defines used to control compiling options
//
#include "Config.h"

//
// Engine.h includes everything that rarely changes between applications, such as vector/Matrix4 libraries,
// OpenGL/DirectX graphics devices, software rasterizers, etc.
//
#include "Engine.h"

#include "Constants.h"

#include "BaseCodeDLL.h"

#include "AppParameters.h"

#include "Enums.h"

#include "CardDatabase.h"
#include "GameData.h"

#include "State.h"
#include "BuyAgenda.h"
#include "Player.h"
#include "TestChamber.h"
#include "MetaTestChamber.h"
#include "AIUtility.h"

#include "Event.h"
#include "EventBase.h"
#include "EventIntrigue.h"
#include "EventSeaside.h"
#include "EventAlchemy.h"
#include "EventProsperity.h"
#include "EventCustom.h"

#include "CardEffect.h"
#include "CardEffectBase.h"
#include "CardEffectIntrigue.h"
#include "CardEffectSeaside.h"
#include "CardEffectAlchemy.h"
#include "CardEffectProsperity.h"
#include "CardEffectCustom.h"

#include "DominionGame.h"

#include "App.h"
