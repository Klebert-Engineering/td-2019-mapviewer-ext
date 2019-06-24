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
const QString PLACE_OPTION = "place";
const QString CAPTION_OPTION = "caption";

const QString WIP_BOOKMARK_BATCH = "wipBatch";

const QString WIP_LABEL_STYLE = "wipLabelStyle";
const QString WIP_POINT_STYLE = "wipPointStyle";

static MapElementMetadata wipLabelMetadata = {
    {MapElementMetadataKey::StyleName, WIP_LABEL_STYLE},
    {MapElementMetadataKey::Flags, MapElementFlags::Unselectable},
};

static MapElementMetadata wipPointMetadata = {
    {MapElementMetadataKey::StyleName, WIP_POINT_STYLE},
    {MapElementMetadataKey::Flags, MapElementFlags::Unselectable},
};
}

MapViewerExtensionTd2019::MapViewerExtensionTd2019(IMapDataProxy & proxy) {
    connect(&wipBookmarkTimeout_, &QTimer::timeout, [&proxy, this](){
        proxy.removeMapElementBatch(*this, wipBookmarkBatchName_);
        wipBookmarkBatchName_ = false;
    });
}

bool MapViewerExtensionTd2019::initialize(IMapDataProxy& proxy, IMapViewerExtensionUserOptions& opts)
{
    opts.addBool(SHOW_OPTION, "Show Bookmarks", true, true, true);
    opts.addBool(PLACE_OPTION, "Place Bookmarks", false, true, false);
    opts.addString(CAPTION_OPTION, "Edit caption", "", "Edit caption here after placing bookmark", false);

    using namespace std::placeholders;

    proxy.onStyleForName(*this, [this](IMapDataProxy& proxy, QString const& styleName, MapGeomType /*geomType*/) -> IMapViewerStylePtr
    {
        if (styleName == WIP_LABEL_STYLE) {
            return proxy.newLabelStyle(AfwColor::Orange, 1.f, {
                {IMapViewerLabelStyle::Left, "-50%"},
                {IMapViewerLabelStyle::FontFamily, "sans-serif"},
                {IMapViewerLabelStyle::TextShadow, "0px 1em 9px black"},
                {IMapViewerLabelStyle::FontSize, "1.1em"},
            });
        }
        else if (styleName == WIP_POINT_STYLE) {
            return proxy.newPointStyle(AfwColor::Magenta);
        }

        return nullptr;
    });

    proxy.onPersistentUserOptionsParsed(*this, std::bind(&MapViewerExtensionTd2019::optionsChanged, this, _1, _2));
    proxy.onOptionValueChanged(*this, std::bind(&MapViewerExtensionTd2019::optionsChanged, this, _1, _2));
    proxy.onLeftClick(*this, std::bind(&MapViewerExtensionTd2019::leftClicked, this, _1, _2));

    return true;
}

void MapViewerExtensionTd2019::optionsChanged(IMapDataProxy &mapDataProxy, IMapViewerExtensionUserOptions &userOptions)
{

}

void MapViewerExtensionTd2019::leftClicked(IMapDataProxy& mapDataProxy, HighPrecWgs84 coordinate)
{
    auto wipBatch = mapDataProxy.newMapElementBatch(*this, WIP_BOOKMARK_BATCH);
    wipBatch->addLabelElement(coordinate, "[Edit description to add new bookmark]", wipLabelMetadata);
    wipBatch->addPointElement(coordinate, wipPointMetadata);
    wipBatch->commit();
    wipBookmarkBatchName_ = WIP_BOOKMARK_BATCH;
    wipBookmarkTimeout_.setSingleShot(true);
    wipBookmarkTimeout_.start(5000);
}

}
