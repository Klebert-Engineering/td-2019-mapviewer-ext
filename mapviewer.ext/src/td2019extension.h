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

#ifndef MAPVIEWER_EXTENSION_LOCMARK
#define MAPVIEWER_EXTENSION_LOCMARK

#include <plugin.mapviewer.backend/imapviewerextension.h>

namespace ndsafw
{

class MapViewerExtensionTd2019 : public QObject, public IMapViewerExtensionInstance
{
    Q_OBJECT

public:
    bool initialize(IMapDataProxy& proxy, IMapViewerExtensionUserOptions& opts) override;
    void shutdown() override {}

private:
    IMapViewerStylePtr style_;
    static MapElementMetadata metaData_;
};

}

#endif // MAPVIEWER_EXTENSION_LOCMARK
