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

#ifndef MAPVIEWER_EXTENSION_LOCMARKFACTORY
#define MAPVIEWER_EXTENSION_LOCMARKFACTORY

#include <plugin.mapviewer.backend/imapviewerextension.h>

namespace ndsafw
{

class BookmarksExtFactory : public IMapViewerExtensionFactory
{
public:
    IMapViewerExtensionInstancePtr instance(IMapDataProxy &proxy) const override;
};

}

#endif // MAPVIEWER_EXTENSION_LOCMARKFACTORY
