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
#include <QTimer>

namespace ndsafw
{

class MapViewerExtensionTd2019 : public QObject, public IMapViewerExtensionInstance
{
    Q_OBJECT

public:
    MapViewerExtensionTd2019(IMapDataProxy & proxy);

    bool initialize(IMapDataProxy& proxy, IMapViewerExtensionUserOptions& opts) override;
    void shutdown() override {}

    void updateBookmarks() {}
    void optionsChanged(IMapDataProxy& mapDataProxy, IMapViewerExtensionUserOptions& userOptions);
    void leftClicked(IMapDataProxy& mapDataProxy, HighPrecWgs84 coordinate);

private:
    IMapViewerStylePtr wipBookmarkStyle_;
    QStringList bookmarks_;

    QString wipBookmarkBatchName_;

    bool showBookmarks_ = true;    // Make sure to keep in sync with default value
    bool placeBookmarks_ = false;  // Make sure to keep in sync with default value
    QString bookmarkDescription;   // Make sure to keep in sync with default value

    QTimer wipBookmarkTimeout_;
    QTimer bookmarkEditDebounce_;
};

}

#endif // MAPVIEWER_EXTENSION_LOCMARK
