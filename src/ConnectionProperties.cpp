/*
obs-websocket
Copyright (C) 2016-2019	Stéphane Lepin <stephane.lepin@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include "ConnectionProperties.h"

ConnectionProperties::ConnectionProperties() :
	_authenticated(false),
	_volMeters(),
	_volMetersMutex(QMutex::Recursive)
{
}

ConnectionProperties::~ConnectionProperties()
{
	QMutexLocker locker(&_volMetersMutex);

	for (auto volMeter : _volMeters.values()) {
		obs_volmeter_destroy(volMeter);
	}
	_volMeters.clear();
}

bool ConnectionProperties::isAuthenticated()
{
    return _authenticated.load();
}

void ConnectionProperties::setAuthenticated(bool authenticated)
{
    _authenticated.store(authenticated);
}

bool ConnectionProperties::addVolMeter(obs_source_t* source)
{
	QMutexLocker locker(&_volMetersMutex);

	if (_volMeters.contains(source)) {
		return false;
	}

	obs_volmeter_t* volMeter = obs_volmeter_create(OBS_FADER_CUBIC);
	_volMeters.insert(source, volMeter);
	return true;
}

void ConnectionProperties::removeVolMeter(obs_source_t* source)
{
	obs_volmeter_t* volMeter = _volMeters.take(source);
	obs_volmeter_destroy(volMeter);
}

obs_volmeter_t* ConnectionProperties::getVolMeter(obs_source_t* source)
{
	return _volMeters.value(source);
}
