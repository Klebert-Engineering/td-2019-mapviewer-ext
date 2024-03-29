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

#include "extensionfactory.h"
#include "extension.h"

namespace ndsafw
{

IMapViewerExtensionInstancePtr BookmarksExtFactory::instance(IMapDataProxy & proxy) const
{
    return IMapViewerExtensionInstancePtr(new BookmarksExt(proxy));
}

}

