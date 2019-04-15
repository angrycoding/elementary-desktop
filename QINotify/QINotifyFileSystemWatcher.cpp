/**
    Copyright 2018 Cucchetto Filippo

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to do
    so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "QINotifyFileSystemWatcher.h"
#include "QINotify.h"

#include <QDir>
#include <QDebug>
#include <QSet>
#include <QTimer>
#include <unordered_map>
#include <unordered_set>

namespace std {
  template<> struct hash<QString> {
    std::size_t operator()(const QString& s) const {
      return qHash(s);
    }
  };
}

static const int MOVE_TIMEOUT = 50;

struct QINotifyFileSystemWatcher::QINotifyFileSystemWatcherImpl {
    QString m_rootPath;
    QString m_source_move_event_path;
    QINotify m_inotify;
    bool m_recursive = false;
    bool m_move_pending = false;
    QINotify::Event m_move_event;
    QTimer m_move_timer;
    std::unordered_map<QString, int> m_pathToDescriptor;
    std::unordered_map<int, QString> m_descriptorToPath;
};

QINotifyFileSystemWatcher::QINotifyFileSystemWatcher(QObject *parent)
    : QObject(parent)
    , m_impl(new QINotifyFileSystemWatcher::QINotifyFileSystemWatcherImpl())
{
    QObject::connect(&m_impl->m_inotify, &QINotify::eventTriggered, this, &QINotifyFileSystemWatcher::onEventTriggered);
    QObject::connect(&m_impl->m_move_timer, &QTimer::timeout, this, &QINotifyFileSystemWatcher::onMoveTimerTimeout);
}

QINotifyFileSystemWatcher::~QINotifyFileSystemWatcher()
{
    delete m_impl;
}

bool QINotifyFileSystemWatcher::isRecursive() const
{
    return m_impl->m_recursive;
}

QString QINotifyFileSystemWatcher::rootPath() const
{
    return m_impl->m_rootPath;
}

void QINotifyFileSystemWatcher::setRootPath(const QString &rootPath, bool recursive)
{   
    m_impl->m_rootPath = rootPath;
    m_impl->m_recursive = recursive;
    addWatchToDir(rootPath);
}

QSet<QString> QINotifyFileSystemWatcher::watchedDirs() const
{    
    QSet<QString> result;
    result.reserve(m_impl->m_pathToDescriptor.size());
    for (auto it = m_impl->m_pathToDescriptor.begin(); it != m_impl->m_pathToDescriptor.end(); ++it) {
        result.insert(it->first);
    }
    return result;
}

void QINotifyFileSystemWatcher::addWatchToDir(const QString &path)
{
    int watch_descriptor = m_impl->m_inotify.AddWatch(path);
    m_impl->m_descriptorToPath.emplace(watch_descriptor, path);
    m_impl->m_pathToDescriptor.emplace(path, watch_descriptor);

    if (isRecursive()) {
        QDir dir(path);
        for (const QFileInfo& entry : dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs)) {
            addWatchToDir(entry.absoluteFilePath());
        }
    }
}

void QINotifyFileSystemWatcher::removeWatchFromDir(const QString &path)
{
    if (isRecursive()) {
        for (auto it = m_impl->m_pathToDescriptor.begin(); it != m_impl->m_pathToDescriptor.end();) {
            if (it->first != path && it->first.startsWith(path)) {
                m_impl->m_descriptorToPath.erase(it->second);
                it = m_impl->m_pathToDescriptor.erase(it);
                m_impl->m_inotify.RemoveWatch(it->second);
            } else {
                ++it;
            }
        }
    }

    auto wd_it = m_impl->m_pathToDescriptor.find(path);
    int wd = wd_it->second;
    m_impl->m_descriptorToPath.erase(wd);
    m_impl->m_pathToDescriptor.erase(wd_it);
    m_impl->m_inotify.RemoveWatch(wd);
}

void QINotifyFileSystemWatcher::onEventTriggered(const QINotify::Event &event)
{
    QString event_source_entity_path = m_impl->m_descriptorToPath[event.sourceWatchDescriptor];

    if (m_impl->m_move_pending) {
        m_impl->m_move_pending = false;
        m_impl->m_move_timer.stop();
        if (event.eventType.testFlag(QINotify::ChildMovedToEvent) &&
            event.eventCookie == m_impl->m_move_event.eventCookie) {
            Q_ASSERT(event.childEntityType == m_impl->m_move_event.childEntityType);

            if (event.childEntityType == QINotify::FileEntity) {
                emit fileMoved(m_impl->m_source_move_event_path,
                               m_impl->m_move_event.childEntityName,
                               event_source_entity_path,
                               event.childEntityName);
            } else {
                const QString from_path = m_impl->m_source_move_event_path
                                        + QDir::separator()
                                        + m_impl->m_move_event.childEntityName;

                const QString to_path = event_source_entity_path
                                      + QDir::separator()
                                      + event.childEntityName;

                std::vector<std::pair<QString, int>> to_insert;

                // Remove keys that changed name and add it later
                for (auto it = m_impl->m_pathToDescriptor.begin(); it != m_impl->m_pathToDescriptor.end(); ) {
                    if (it->first.startsWith(from_path)) {
                        QString new_dir_path = QString(it->first).replace(from_path, to_path);
                        int wd = it->second;
                        it = m_impl->m_pathToDescriptor.erase(it);
                        m_impl->m_descriptorToPath.erase(wd);
                        to_insert.emplace_back(std::move(new_dir_path), wd);
                    } else {
                        ++it;
                    }
                }

                // Add renamed items
                for (const auto& pair : to_insert) {
                    m_impl->m_pathToDescriptor.emplace(pair.first, pair.second);
                    m_impl->m_descriptorToPath.emplace(pair.second, pair.first);
                }

                emit directoryMoved(m_impl->m_source_move_event_path,
                                    m_impl->m_move_event.childEntityName,
                                    event_source_entity_path,
                                    event.childEntityName);
            }
            return;
        } else {
            onMoveTimerTimeout();
        }
    }

    if (event.eventType.testFlag(QINotify::DeleteEvent) || event.eventType.testFlag(QINotify::MoveEvent)) {
        if (event_source_entity_path == m_impl->m_rootPath) {
            emit rootPathRemoved(m_impl->m_rootPath);
            removeWatchFromDir(m_impl->m_rootPath);
            return;
        }
    }

    if (event.eventType.testFlag(QINotify::ChildMovedFromEvent)) {
        m_impl->m_move_pending = true;
        m_impl->m_move_event = event;
        m_impl->m_source_move_event_path = event_source_entity_path;
        m_impl->m_move_timer.start(MOVE_TIMEOUT);
        return;
    }

    if (event.childEntityType == QINotify::FileEntity) {
        if (event.eventType.testFlag(QINotify::ChildCreateEvent) ||
            event.eventType.testFlag(QINotify::ChildMovedToEvent)) {
            emit fileAdded(event_source_entity_path, event.childEntityName);
        } else if (event.eventType.testFlag(QINotify::ChildDeleteEvent)) {
            emit fileRemoved(event_source_entity_path, event.childEntityName);
        } else if (event.eventType.testFlag(QINotify::ModifyEvent)
                   || event.eventType.testFlag(QINotify::AttributeChangeEvent)) {
            emit fileModified(event_source_entity_path, event.childEntityName);
        }
    }
    else if (event.childEntityType == QINotify::DirectoryEntity) {
        if (event.eventType.testFlag(QINotify::ChildCreateEvent)
                || event.eventType.testFlag(QINotify::ChildMovedToEvent)) {
            if (m_impl->m_recursive)
                addWatchToDir(event_source_entity_path + QDir::separator() + event.childEntityName);
            emit directoryAdded(event_source_entity_path, event.childEntityName);
        } else if (event.eventType.testFlag(QINotify::ChildDeleteEvent)) {
            if (m_impl->m_recursive)
                removeWatchFromDir(event_source_entity_path + QDir::separator() + event.childEntityName);
            emit directoryRemoved(event_source_entity_path, event.childEntityName);
        }
    }
}

void QINotifyFileSystemWatcher::onMoveTimerTimeout() {
    if (!m_impl->m_move_pending)
        return;
    m_impl->m_move_pending = false;
    if (m_impl->m_move_event.childEntityType == QINotify::FileEntity) {
        emit fileRemoved(m_impl->m_source_move_event_path, m_impl->m_move_event.childEntityName);
    } else if (m_impl->m_move_event.childEntityType == QINotify::DirectoryEntity) {
        if (m_impl->m_recursive)
            removeWatchFromDir(m_impl->m_source_move_event_path + QDir::separator() + m_impl->m_move_event.childEntityName);
        emit directoryRemoved(m_impl->m_source_move_event_path, m_impl->m_move_event.childEntityName);
    }
}
