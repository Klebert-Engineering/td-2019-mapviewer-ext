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

#include <lib.afw.core/diagnostics.h>
#include <plugin.mapviewer.backend/imapviewerextensionregistry.h>
#include <lib.extensionsystem/extensionsystem/pluginmanager.h>

#include "td2019plugin.h"
#include "td2019extensionfactory.h"

namespace ndsafw {

void MapViewerExtensionTd2019Plugin::extensionsInitialized()
{
}

bool MapViewerExtensionTd2019Plugin::initialize(const QStringList &, QString *)
{
    auto extensionReg = ExtensionSystem::PluginManager::getObject<IMapViewerExtensionRegistry>();
    NDSAFW_COND_CRITICAL_AND_RETURN(!extensionReg,
                                    "Failed to retrieve mapviewer extension registry!",
                                    false);
    extensionReg->registerExtension("Tooling Days 2019",
                                    IMapViewerExtensionFactoryPtr(new MapViewerExtensionTd2019Factory));
    return true;
}

ExtensionSystem::IPlugin::ShutdownFlag MapViewerExtensionTd2019Plugin::aboutToShutdown() {
    return ExtensionSystem::IPlugin::SynchronousShutdown;
}

}
