#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "Containers/utlvector.h"

#include "Misc/temp_basicstring.h"
#include "Misc/temp_singleton.h"

class ConfigManager :
	public Singleton<ConfigManager>
{
	typedef Singleton<ConfigManager> singleton_class;

private:

	int const content_version; // Backward compatible if we ever change the layout of the config file

public:
	basic_string m_playerdataclass;

	// values
	float m_smoke_radius;
	float m_innersmoke_radius_sqr;
	float m_smoke_timetobang;
	float m_smoke_time;

	// virtual functions
	int vfid_getdatadescmap;
	int vfid_settransmit;
	int vfid_mhgroundentity;
	int vfid_weaponequip;
	int vfid_weapondrop;
	int vfid_playerruncommand;
	int vfid_dispatch;
	int vfid_thinkpost;

public:
	ConfigManager ();
	virtual ~ConfigManager () override final;

	/*
		Load the config file containing configuration. Returns false if error parsing the config file.
	*/
	bool LoadConfig ();
};

#endif // CONFIGMANAGER_H
