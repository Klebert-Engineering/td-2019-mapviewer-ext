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
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlResult>
#include <functional>
#include <lib.afw.core/diagnostics.h>

#include "extension.h"
#include <QtConcurrent>

namespace ndsafw {

namespace {

auto& logCat = NDSAFW_LOGCAT("mapviewer.bookmarks");
const QString SQLITE_FILENAME = "bookmarks.sqlite";
const QString BOOKMARKS_SEARCH = "bookmarks";

const QString SHOW_OPTION = "show";
const QString PLACE_OPTION = "place";
const QString CAPTION_OPTION = "caption";

const QString BOOKMARK_BATCH = "bookmarks";

const QString LABEL_STYLE = "labelStyle";
const QString POINT_STYLE = "pointStyle";
const QString WIP_LABEL_STYLE = "wipLabelStyle";
const QString WIP_POINT_STYLE = "wipPointStyle";

const QString AUTO_DELETE_MESSAGE =
    "New bookmark will auto-delete in <span style='color:red;'>%1</span> seconds.\n"
    "Edit caption to persist it.";

static MapElementMetadata wipLabelMetadata = {
    {MapElementMetadataKey::StyleName, WIP_LABEL_STYLE}
};

static MapElementMetadata wipPointMetadata = {
    {MapElementMetadataKey::StyleName, WIP_POINT_STYLE},
    {MapElementMetadataKey::Flags, MapElementFlags::Unselectable},
};

static MapElementMetadata labelMetadata = {
    {MapElementMetadataKey::StyleName, LABEL_STYLE},
};

static MapElementMetadata pointMetadata = {
    {MapElementMetadataKey::StyleName, POINT_STYLE},
};
}

BookmarksExt::BookmarksExt(IMapDataProxy & proxy)
{
    connect(&transientTimeout_, &QTimer::timeout, [&proxy, this]()
    {
        if (!underConstruction_ || underConstruction_->isPersistent)
            return;

        if (transientCountdown_ > 0) {
            transientCountdown_ -= 2;
            proxy.setStatusMessage(*this, QString(AUTO_DELETE_MESSAGE).arg(transientCountdown_));
        }

        if (transientCountdown_ == 0)
            cancelNewBookmark(proxy);
        else
            transientTimeout_.start(2000);
    });

    connect(&bookmarkEditDebounce_, &QTimer::timeout, [&proxy, this]()
    {
        if (underConstruction_)
        {
            if (!underConstruction_->isPersistent) {
                underConstruction_->isPersistent = true;
                transientTimeout_.stop();
                proxy.setStatusMessage(*this, QString("Created new Bookmark with id %1!").arg(underConstruction_->id));
                persistenceUpdateBookmark(*underConstruction_);
            }

            if (!bookmarkCaption_.isEmpty()) {
                underConstruction_->caption = bookmarkCaption_;
                persistenceUpdateBookmark(*underConstruction_);
            }
            else {
                proxy.setStatusMessage(*this, QString("Bookmark %1 deleted!").arg(underConstruction_->id));
                persistenceDeleteBookmark(*underConstruction_);
                bookmarks_.remove(underConstruction_->id);
                underConstruction_ = nullptr;
            }
        }
        else
            proxy.setStatusMessage(*this, "No Bookmark selected!");

        updateBookmarks(proxy);
    });

    connect(&newBookmarkTimer_, &QTimer::timeout, [&proxy, this]() {
        startNewBookmark(proxy);
    });

    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(SQLITE_FILENAME);
}

bool BookmarksExt::initialize(IMapDataProxy& proxy, IMapViewerExtensionUserOptions& opts)
{
    if (!db_.open()) {
        NDSAFW_CRITICAL(QString("Cannot open database '%1'").arg(SQLITE_FILENAME));
        return false;
    }
    else {
        // Create bookmarks table if it doesn't exist yet
        QSqlQuery q("CREATE TABLE IF NOT EXISTS bookmarks (id INTEGER UNIQUE, lon REAL, lat REAL, caption TEXT, timestamp TEXT)", db_);
        if (q.exec())
            NDSAFW_CDEBUG(logCat, "Initialized bookmarks persistence");
        persistenceLoadBookmarks();
    }

    opts.addBool(SHOW_OPTION, "Show Bookmarks", true, true, true);
    opts.addBool(PLACE_OPTION, "Place Bookmarks", false, true, false);
    opts.addString(CAPTION_OPTION, "Edit caption", "", "Edit caption here after placing bookmark", false);
    opts.addSearchAction(BOOKMARKS_SEARCH, "Filter Bookmarks");

    using namespace std::placeholders;

    proxy.onStyleForName(*this, [this](IMapDataProxy& proxy, QString const& styleName, MapGeomType /*geomType*/) -> IMapViewerStylePtr
    {
        if (styleName == WIP_LABEL_STYLE) {
            return proxy.newLabelStyle(AfwColor::LightSkyBlue, 1.f, {
                {IMapViewerLabelStyle::Top, "2em"},
                {IMapViewerLabelStyle::FontFamily, "sans-serif"},
                {IMapViewerLabelStyle::TextShadow, "0 0 5px black, 0 0 5px black"},
                {IMapViewerLabelStyle::FontSize, "1.1em"},
            });
        }
        else if (styleName == WIP_POINT_STYLE) {
            return proxy.newPointStyle(AfwColor::LightSkyBlue);
        }
        else if (styleName == POINT_STYLE) {
            return proxy.newPointStyle(QString(":/bookmark.png"), 1., 32.);
        }
        else if (styleName == LABEL_STYLE) {
            return proxy.newLabelStyle(AfwColor::Yellow, 1.f, {
                {IMapViewerLabelStyle::Top, "2em"},
                {IMapViewerLabelStyle::FontFamily, "sans-serif"},
                {IMapViewerLabelStyle::TextShadow, "0 0 5px black, 0 0 5px black"},
                {IMapViewerLabelStyle::FontSize, "1.1em"},
            });
        }

        return nullptr;
    });

    proxy.onPersistentUserOptionsParsed(*this, std::bind(&BookmarksExt::optionsChanged, this, _1, _2));
    proxy.onOptionValueChanged(*this, std::bind(&BookmarksExt::optionsChanged, this, _1, _2));
    proxy.onLeftClick(*this, std::bind(&BookmarksExt::leftClicked, this, _1, _2));
    proxy.onRequestAttributes(*this, std::bind(&BookmarksExt::attribsRequested, this, _1, _2));
    proxy.onSearchActionRequested(*this, std::bind(&BookmarksExt::searchRequested, this, _1, _2, _3, _4, _5));
    proxy.onSearchActionTerminateRequested(*this, std::bind(&BookmarksExt::searchTerminateRequested, this, _1, _2));

    return true;
}

void BookmarksExt::optionsChanged(IMapDataProxy &proxy, IMapViewerExtensionUserOptions &userOptions)
{
    QString newBookmarkCaption = userOptions.getString(CAPTION_OPTION);
    bool newShowBookmarks = userOptions.getBool(SHOW_OPTION);
    bool newPlaceBookmarks = userOptions.getBool(PLACE_OPTION);

    if (newShowBookmarks != showBookmarks_) {
        showBookmarks_ = newShowBookmarks;
        // When bookmarks are hidden, placement is disabled
        if (!newShowBookmarks && placeBookmarks_) {
            placeBookmarks_ = false;
            userOptions.setOptionValue(PLACE_OPTION, false);
            cancelNewBookmark(proxy);
        }
    }
    else if (newPlaceBookmarks != placeBookmarks_) {
        placeBookmarks_ = newPlaceBookmarks;
        // When placement is enabled, bookmarks are shown
        if (placeBookmarks_ && !showBookmarks_) {
            showBookmarks_ = true;
            userOptions.setOptionValue(SHOW_OPTION, true);
        }
        else if (!placeBookmarks_) {
            cancelNewBookmark(proxy);
        }
    }

    if (newBookmarkCaption != bookmarkCaption_) {
        bookmarkCaption_ = newBookmarkCaption;
        bookmarkEditDebounce_.setSingleShot(true);
        bookmarkEditDebounce_.start(500);
    }

    updateBookmarks(proxy);
}

void BookmarksExt::leftClicked(IMapDataProxy& proxy, HighPrecWgs84 location)
{
    proxy.userOptions(*this).setOptionValue(CAPTION_OPTION, "");
    bookmarkCaption_ = "";

    if (placeBookmarks_)
    {
        clickedLocation_ = location;
        newBookmarkTimer_.setSingleShot(true);
        newBookmarkTimer_.start(200);
    }
    else
    {
        underConstruction_ = nullptr;
        proxy.setStatusMessage(*this, "");
    }
}

QStringList BookmarksExt::attribsRequested(IMapDataProxy& proxy, MapElementMetadata const& metadata)
{
    uint32_t id = metadata[MapElementMetadataKey::AppModelId].toUInt();
    if (bookmarks_.contains(id)) {
        newBookmarkTimer_.stop();
        cancelNewBookmark(proxy);
        underConstruction_ = &bookmarks_[id];
        proxy.setStatusMessage(*this, QString("Editing bookmark %1!").arg(id));
        proxy.userOptions(*this).setOptionValue(CAPTION_OPTION, underConstruction_->caption);
        bookmarkCaption_ = underConstruction_->caption;

        // Show timestamp in attribute panel
        QtConcurrent::run([&proxy, metadata, this](){
            auto panel = proxy.newAttributePanel(*this);
            panel->addAttrib("Created on", underConstruction_->timestamp);
            panel->commit(metadata[MapElementMetadataKey::VisualId].toUInt(), 0);        
        });
    }

    return {"Test Panel"};
}

void BookmarksExt::searchRequested(IMapDataProxy& proxy, QString name, QStringList const& arguments, IMapViewerExtensionUserOptions const& opts, QString& resultError)
{
    if (arguments.size() < 1) {
        resultError = "Need a filter string!";
        return;
    }
    proxy.setStatusMessage(*this, QString("Searching for %1").arg(arguments[0]));
    searchFilter_ = arguments[0].toLower();
    updateBookmarks(proxy);
}

void BookmarksExt::searchTerminateRequested(IMapDataProxy& proxy, QString name) {
    searchFilter_ = "";
    proxy.setStatusMessage(*this, "");
    updateBookmarks(proxy);
}

void BookmarksExt::updateBookmarks(IMapDataProxy& proxy)
{
    if (showBookmarks_ && !bookmarks_.empty()) {
        auto batch = proxy.newMapElementBatch(*this, BOOKMARK_BATCH);
        for (auto const& bm : bookmarks_)
        {
            if (bm.isPersistent)
            {
                if (!searchFilter_.isEmpty() && !bm.caption.toLower().contains(searchFilter_)) {
                    continue;
                }
                batch->addLabelElement(bm.location, bm.caption, labelMetadata);
                pointMetadata[MapElementMetadataKey::AppModelId] = bm.id;
                pointMetadata[MapElementMetadataKey::VisualLabel] = QString("Bookmark #%1").arg(bm.id);
                batch->addPointElement(bm.location, pointMetadata);
            }
            else {
                batch->addLabelElement(bm.location, bm.caption, wipLabelMetadata);
                batch->addPointElement(bm.location, wipPointMetadata);
            }
        }
        batch->commit();
    }
    else
        proxy.removeMapElementBatch(*this, BOOKMARK_BATCH);
}


void BookmarksExt::cancelNewBookmark(ndsafw::IMapDataProxy &proxy) {
    if (underConstruction_ && !underConstruction_->isPersistent) {
        bookmarks_.remove(underConstruction_->id);
        underConstruction_ = nullptr;
        updateBookmarks(proxy);
        proxy.setStatusMessage(*this, "");
    }
}

void BookmarksExt::startNewBookmark(IMapDataProxy& proxy)
{
    // Only allow one transient bookmark at a time
    if (!underConstruction_ || underConstruction_->isPersistent) {
        uint32_t newId = 0;
        while (bookmarks_.contains(newId))
            ++newId;

        bookmarks_[newId] = Bookmark{
            newId,
            "Edit description to add new bookmark.",
            clickedLocation_,
            false,
            QDateTime::currentDateTime().toString()
        };
        underConstruction_ = &bookmarks_[newId];
    }
    else {
        // Use existing transient bookmark, but move it to new location
        underConstruction_->location = clickedLocation_;

    }

    transientCountdown_ = 20;
    transientTimeout_.setSingleShot(true);
    transientTimeout_.start(2000);
    proxy.setStatusMessage(*this, QString(AUTO_DELETE_MESSAGE).arg(transientCountdown_));
    updateBookmarks(proxy);
}

void BookmarksExt::persistenceLoadBookmarks()
{
    QSqlQuery q("select * from bookmarks", db_);
    if (!q.exec() || !q.isActive()) {
        NDSAFW_CWARNING(logCat, QString("Failed to execute query %1: %2")
        .arg(q.lastQuery())
        .arg(q.lastError().text()));
        return;
    }

    auto rec = q.record();
    int idCol = rec.indexOf("id");
    int lonCol = rec.indexOf("lon");
    int latCol = rec.indexOf("lat");
    int capCol = rec.indexOf("caption");
    int tsCol = rec.indexOf("timestamp");

    while (q.next()) {
        bookmarks_[q.value(idCol).toUInt()] = Bookmark{
            q.value(idCol).toUInt(),
            q.value(capCol).toString().replace("\\'", "'"),
            HighPrecWgs84{q.value(lonCol).toDouble(), q.value(latCol).toDouble()},
            true,
            q.value(tsCol).toString()
        };
    }

    NDSAFW_CDEBUG(logCat, QString("Loaded %1 bookmarks.").arg(bookmarks_.size()));
}

void BookmarksExt::persistenceUpdateBookmark(Bookmark const& bm)
{
    QSqlQuery q(
        QString("replace into bookmarks (id, lon, lat, caption, timestamp) values (%1, %2, %3, '%4', '%5')")
        .arg(bm.id)
        .arg(bm.location.longitude())
        .arg(bm.location.latitude())
        .arg(QString(bm.caption).replace("'", "\\'"))
        .arg(bm.timestamp)
    , db_);
    if (!q.exec() || !q.isActive()) {
        NDSAFW_CWARNING(logCat, QString("Failed to execute query %1: %2")
        .arg(q.lastQuery())
        .arg(q.lastError().text()));
    }
}

void BookmarksExt::persistenceDeleteBookmark(Bookmark const& bm) {
    QSqlQuery q(
        QString("delete from bookmarks where id=%1").arg(bm.id)
        , db_);
    if (!q.exec() || !q.isActive()) {
        NDSAFW_CWARNING(logCat, QString("Failed to execute query %1: %2")
        .arg(q.lastQuery())
        .arg(q.lastError().text()));
    }
}

}
