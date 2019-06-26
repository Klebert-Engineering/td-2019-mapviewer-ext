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

class BookmarksPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.nds-association.mapviewer.ext.bookmarks")

public:
    BookmarksPlugin() = default;
    ~BookmarksPlugin() override = default;

    BookmarksPlugin(const BookmarksPlugin& p) = delete;
    BookmarksPlugin(BookmarksPlugin&& p) = delete;
    BookmarksPlugin& operator=(const BookmarksPlugin& p) = delete;
    BookmarksPlugin& operator=(BookmarksPlugin&& p) = delete;

public:
    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;
};

}

#endif
