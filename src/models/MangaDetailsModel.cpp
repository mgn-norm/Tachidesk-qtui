#include "MangaDetailsModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <qcoreapplication.h>
#include <QQmlEngine>

#define exportQmlType(ns, cls) qmlRegisterType<cls>(#ns, 1, 0, #cls)
#define IMPLEMENT_QTQUICK_TYPE(ns, cls) \
  void register ## cls ## Type() { exportQmlType(ns, cls); } \
  Q_COREAPP_STARTUP_FUNCTION(register ## cls ## Type)

IMPLEMENT_QTQUICK_TYPE(Tachidesk.Models, MangaDetailsModel)

/******************************************************************************
 *
 * MangaDetailsModel
 *
 *****************************************************************************/
MangaDetailsModel::MangaDetailsModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

void MangaDetailsModel::classBegin() { }

/******************************************************************************
 *
 * componentComplete
 *
 *****************************************************************************/
void MangaDetailsModel::componentComplete()
{
  _networkManager->get(QStringLiteral("manga/%1").arg(_mangaNumber));

  connect(
      _networkManager,
      &NetworkManager::recievedReply,
      this,
      &MangaDetailsModel::recievedReply);
}

/******************************************************************************
 *
 * Method: recieveReply()
 *
 *****************************************************************************/
void MangaDetailsModel::recievedReply(const QJsonDocument& reply)
{
  if (reply.isObject()) {
    disconnect(_networkManager, &NetworkManager::recievedReply, this, nullptr);
  } else {
    return;
  }

  beginResetModel();
  _entries.clear();


  const auto& entry = reply.object();
  auto& info        = _entries.emplace_back();
  info.id           = entry["id"].toInt();
  info.sourceId     = entry["sourceId"].toString();
  info.url          = entry["url"].toString();
  info.title        = entry["title"].toString();
  info.thumbnailUrl = entry["thumbnailUrl"].toString();
  info.initalized   = entry["intialized"].toBool();
  info.author       = entry["author"].toString();
  info.artist       = entry["artist"].toString();
  info.description  = entry["description"].toString();
  info.genre        = entry["genre"].toString();
  info.status       = entry["status"].toString();

  endResetModel();

  emit loaded();
}

/******************************************************************************
 *
 * Method: rowCount()
 *
 *****************************************************************************/
int MangaDetailsModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return _entries.size();
}

/******************************************************************************
 *
 * Method: data()
 *
 *****************************************************************************/
QVariant MangaDetailsModel::data(const QModelIndex &index, int role) const {
  if (!((index.isValid()) &&
       (index.row() >= 0) &&
       (index.row() < rowCount())))
  {
    return {};
  }

  const auto& entry = _entries[index.row()];

  switch (role)
  {
    case RoleId:
      {
        return entry.id;
      }
    case RoleSourceId:
      {
        return entry.sourceId;
      }
    case RoleUrl:
      {
        return entry.url;
      }
    case RoleTitle:
      {
        return entry.title;
      }
    case RoleThumbnailUrl:
      {
        return QStringLiteral("%1:%2%3").arg(_networkManager->hostname()).arg(_networkManager->port()).arg(entry.thumbnailUrl);
      }
    case RoleInitialized:
      {
        return entry.initalized;
      }
    case RoleAuthor:
      {
        return entry.author;
      }
    case RoleArtist:
      {
        return entry.artist;
      }
    case RoleDescription:
      {
        return entry.description;
      }
    case RoleGenre:
      {
        return entry.genre;
      }
    case RoleStatus:
      {
        return entry.status;
      }
    //case Role
    default:
      return {};
  }

  return {};
}

/******************************************************************************
 *
 * Method: roleNames()
 *
 *****************************************************************************/
QHash<int, QByteArray> MangaDetailsModel::roleNames() const {
  static QHash<int, QByteArray> roles = {
    {RoleUrl, "url"},
    {RoleId, "id"},
    {RoleSourceId, "sourceId"},
    {RoleUrl, "url"},
    {RoleTitle, "title"},
    {RoleThumbnailUrl, "thumbnailUrl"},
    {RoleInitialized, "initialized"},
    {RoleAuthor, "author"},
    {RoleArtist, "artist"},
    {RoleDescription, "description"},
    {RoleGenre, "genre"},
    {RoleStatus, "status"}};

  return roles;
}

QVariantMap MangaDetailsModel::get(int row) const
{
  QVariantMap map;
  QModelIndex modelIndex = index(row, 0);
  QHash<int, QByteArray> roles = roleNames();

  for (auto it = roles.begin(); it != roles.end(); ++it) {
    map.insert(it.value(), data(modelIndex, it.key()));
  }
  return map;
}

