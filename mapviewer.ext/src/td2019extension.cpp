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

#include <QtConcurrentRun>
#include <QDateTime>
#include <QFile>
#include <functional>

#include <lib.afw.core/polygon.h>
#include <iostream>

#include "td2019extension.h"

namespace ndsafw {

namespace {
    const QString SHOW_OPTION = "show";
    const QString LOC_MARKER_BATCH = "locMarker";
}

bool MapViewerExtensionTd2019::initialize(IMapDataProxy& proxy, IMapViewerExtensionUserOptions& opts)
{
    opts.addBool(SHOW_OPTION, "Show Marker", true, true, true);
    style_ = proxy.newLabelStyle(AfwColor::White, 1.f, {
        {IMapViewerLabelStyle::Left, "-50%"},
        {IMapViewerLabelStyle::FontFamily, "sans-serif"},
        {IMapViewerLabelStyle::TextShadow, "0px 1em 9px black"},
        {IMapViewerLabelStyle::FontSize, "1.1em"},
    });

    proxy.onStyleForName(*this, [this](IMapDataProxy& /*proxy*/, QString const& /*styleName*/, MapGeomType /*geomType*/)
    {
        return style_;
    });

    proxy.onOptionValueChanged(*this, [this](IMapDataProxy& mapDataProxy, IMapViewerExtensionUserOptions& userOptions)
    {
        if (!userOptions.getBool(SHOW_OPTION))
                mapDataProxy.removeMapElementBatch(*this, LOC_MARKER_BATCH);
    });

    proxy.onLeftClick(*this, [this](IMapDataProxy& mapDataProxy, HighPrecWgs84 coordinate)
    {
        if(!mapDataProxy.userOptions(*this).getBool(SHOW_OPTION))
            return;

        auto batch = mapDataProxy.newMapElementBatch(*this, LOC_MARKER_BATCH, PackedTileId(MortonCode::fromWgs84Coordinates(coordinate), 15));
        batch->addLabelElement(coordinate, "&#x1F4CD;", metaData_);
        batch->commit();
    });

    return true;
}

MapElementMetadata MapViewerExtensionTd2019::metaData_ =
{
    {MapElementMetadataKey::StyleName, "LocationMarker"},
    {MapElementMetadataKey::Type, MapElementType::Custom},
    {MapElementMetadataKey::TypeName, "LocationMarker"},
    {MapElementMetadataKey::Flags, MapElementFlags::Unselectable},
};

}
