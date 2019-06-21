/*******************************************************************************
*
* Copyright (c) 2018, Navigation Data Standard e.V.
*
* All rights reserved.
*
* This source code must not be used in any context that would lead to license
* conditions of the GPL license!
* The Qt framework source code which is used by
* this code allows LGPL conditions when dynamically linked.
* Static linking of the used Qt framework is only allowed if the party building
* the source code is in possession of a valid commercial Qt license, otherwise
* this source code would be exposed under GPL terms.
*
*******************************************************************************/

#ifndef MAPVIEWER_EXTENSION_LOCMARKPLUGIN
#define MAPVIEWER_EXTENSION_LOCMARKPLUGIN

#include <lib.extensionsystem/extensionsystem/iplugin.h>

namespace ndsafw
{

class MapViewerExtensionTd2019Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.nds-association.mapviewer.extension.td2019")

public:
    MapViewerExtensionTd2019Plugin() = default;
    ~MapViewerExtensionTd2019Plugin() override = default;

    MapViewerExtensionTd2019Plugin(const MapViewerExtensionTd2019Plugin& p) = delete;
    MapViewerExtensionTd2019Plugin(MapViewerExtensionTd2019Plugin&& p) = delete;
    MapViewerExtensionTd2019Plugin& operator=(const MapViewerExtensionTd2019Plugin& p) = delete;
    MapViewerExtensionTd2019Plugin& operator=(MapViewerExtensionTd2019Plugin&& p) = delete;

public:
    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;
};

}

#endif
