/*
	Copyright 2012 - Le Padellec Sylvain

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "WeaponHookListener.h"

#include "Console/convar.h"
#include "Interfaces/InterfacesProxy.h"
#include "Interfaces/iserverunknown.h"

#include "Misc/Helpers.h"
#include "plugin.h"
#include "Systems/ConfigManager.h"

WeaponHookListener::WeaponHookListenersListT WeaponHookListener::m_listeners;

WeaponHookListener::WeaponHookListener ()
{
	HookGuard<WeaponHookListener>::Required ();
}

WeaponHookListener::~WeaponHookListener ()
{
	if( HookGuard<WeaponHookListener>::IsCreated () )
	{
		HookGuard<WeaponHookListener>::GetInstance ()->UnhookAll ();
		HookGuard<WeaponHookListener>::DestroyInstance ();
	}
}

void WeaponHookListener::HookWeapon ( PlayerHandler::const_iterator ph )
{
	LoggerAssert ( Helpers::isValidEdict ( ph->GetEdict () ) );
	void* unk ( ph->GetEdict ()->m_pUnk );

	HookInfo info_equip ( unk, ConfigManager::GetInstance ()->vfid_weaponequip, ( DWORD ) RT_nWeapon_Equip );
	HookInfo info_drop ( unk, ConfigManager::GetInstance ()->vfid_weapondrop, ( DWORD ) RT_nWeapon_Drop );
	HookGuard<WeaponHookListener>::GetInstance ()->VirtualTableHook ( info_equip );
	HookGuard<WeaponHookListener>::GetInstance ()->VirtualTableHook ( info_drop );
}

#ifdef GNUC
void HOOKFN_INT WeaponHookListener::RT_nWeapon_Equip ( void * const basePlayer, void * const weapon )
#else
void HOOKFN_INT WeaponHookListener::RT_nWeapon_Equip ( void * const basePlayer, void * const, void * const weapon )
#endif
{
	PlayerHandler::const_iterator ph ( NczPlayerManager::GetInstance ()->GetPlayerHandlerByBasePlayer ( basePlayer ) );

	if( ph != SlotStatus::INVALID )
	{
		SourceSdk::edict_t const * const weapon_ent ( SourceSdk::InterfacesProxy::Call_BaseEntityToEdict ( weapon ) );

		LoggerAssert ( Helpers::isValidEdict ( weapon_ent ) );

		WeaponHookListenersListT::elem_t* it2 ( m_listeners.GetFirst () );
		while( it2 != nullptr )
		{
			it2->m_value.listener->RT_WeaponEquipCallback ( ph, weapon_ent );

			it2 = it2->m_next;
		}
	}

	WeaponEquip_t gpOldFn;
	*( DWORD* )&( gpOldFn ) = HookGuard<WeaponHookListener>::GetInstance ()->RT_GetOldFunction ( basePlayer, ConfigManager::GetInstance ()->vfid_weaponequip );
	gpOldFn ( basePlayer, weapon );
}

#ifdef GNUC
void HOOKFN_INT WeaponHookListener::RT_nWeapon_Drop ( void * const basePlayer, void * const weapon, SourceSdk::Vector const * const targetVec, SourceSdk::Vector const * const velocity )
#else
void HOOKFN_INT WeaponHookListener::RT_nWeapon_Drop ( void * const basePlayer, void * const, void * const weapon, SourceSdk::Vector const * const targetVec, SourceSdk::Vector const * const velocity )
#endif
{
	PlayerHandler::const_iterator ph ( NczPlayerManager::GetInstance ()->GetPlayerHandlerByBasePlayer ( basePlayer ) );

	if( ph != SlotStatus::INVALID && weapon != nullptr )
	{
		SourceSdk::edict_t* const weapon_ent ( SourceSdk::InterfacesProxy::Call_BaseEntityToEdict ( weapon ) );

		LoggerAssert ( Helpers::isValidEdict ( weapon_ent ) );

		WeaponHookListenersListT::elem_t* it2 ( m_listeners.GetFirst () );
		while( it2 != nullptr )
		{
			it2->m_value.listener->RT_WeaponDropCallback ( ph, weapon_ent );

			it2 = it2->m_next;
		}
	}

	WeaponDrop_t gpOldFn;
	*( DWORD* )&( gpOldFn ) = HookGuard<WeaponHookListener>::GetInstance ()->RT_GetOldFunction ( basePlayer, ConfigManager::GetInstance ()->vfid_weapondrop );
	gpOldFn ( basePlayer, weapon, targetVec, velocity );
}

void WeaponHookListener::RegisterWeaponHookListener ( WeaponHookListener const * const listener )
{
	m_listeners.Add ( listener );
}

void WeaponHookListener::RemoveWeaponHookListener ( WeaponHookListener const * const listener )
{
	m_listeners.Remove ( listener );
}
