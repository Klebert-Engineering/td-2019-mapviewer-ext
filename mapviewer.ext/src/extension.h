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
#include <QVector>

#include <QSqlDatabase>
#include <QSqlQuery>

namespace ndsafw
{

struct Bookmark {
    uint32_t id;
    QString caption;
    HighPrecWgs84 location;
    bool isPersistent;
};

class BookmarksExt : public QObject, public IMapViewerExtensionInstance
{
    Q_OBJECT

public:
    BookmarksExt(IMapDataProxy & proxy);

    bool initialize(IMapDataProxy& proxy, IMapViewerExtensionUserOptions& opts) override;
    void shutdown() override {}

    void updateBookmarks(IMapDataProxy& mapDataProxy);
    void optionsChanged(IMapDataProxy& mapDataProxy, IMapViewerExtensionUserOptions& userOptions);
    void leftClicked(IMapDataProxy& mapDataProxy, HighPrecWgs84 coordinate);
    QStringList attribsRequested(IMapDataProxy& proxy, MapElementMetadata const& metadata);

    void startNewBookmark(IMapDataProxy& proxy);
    void cancelNewBookmark(IMapDataProxy& proxy);

private:
    QMap<uint32_t, Bookmark> bookmarks_;
    Bookmark* underConstruction_ = nullptr; // points to element in bookmarks vec
    uint32_t transientCountdown_ = 0;
    HighPrecWgs84 clickedLocation_;

    bool showBookmarks_ = true;    // Make sure to keep in sync with default value
    bool placeBookmarks_ = false;  // Make sure to keep in sync with default value
    QString bookmarkCaption_;      // Make sure to keep in sync with default value

    QTimer transientTimeout_;
    QTimer bookmarkEditDebounce_;
    QTimer newBookmarkTimer_;

    QSqlDatabase db_;

    void persistenceLoadBookmarks();
    void persistenceUpdateBookmark(Bookmark const& bm);
    void persistenceDeleteBookmark(Bookmark const& bm);
};

}

#endif // MAPVIEWER_EXTENSION_LOCMARK
