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

#ifndef QINOTIFYFILESYSTEMWATCHER_H
#define QINOTIFYFILESYSTEMWATCHER_H

#include <QObject>

#include <QINotify/QINotify.h>

/// Monitor a single directory for changes
class QINotifyFileSystemWatcher : public QObject
{
    Q_OBJECT

public:
    explicit QINotifyFileSystemWatcher(QObject *parent = nullptr);
    QINotifyFileSystemWatcher(const QINotifyFileSystemWatcher&) = delete;
    QINotifyFileSystemWatcher& operator=(const QINotifyFileSystemWatcher&) = delete;
    ~QINotifyFileSystemWatcher();

    /// Return true if the root path is monitored recursively, false otherwise
    bool isRecursive() const;

    /// Return the monitored root directory
    QString rootPath() const;

    /// Sets the root directory to monitor
    void setRootPath(const QString& rootPath, bool recursive = false);

    /// Return the watched directories
    QSet<QString> watchedDirs() const;

signals:
    /// Emitted when the root path is delete/moved
    void rootPathRemoved(const QString& rootPath);

    /// Emitted when a new files is added or moved
    void fileAdded(const QString& parentDirectory, const QString& fileName);

    /// Emitted when a file is removed
    void fileRemoved(const QString& parentDirectory, const QString& fileName);

    /// Emitted when a file is moved
    void fileMoved(const QString& fromParentDirectory, const QString& fromFilename,
                   const QString& toParentDirectory, const QString& toFilename);

    /// Emitted when a file is modified
    void fileModified(const QString& parentDirectory, const QString& fileName);

    /// Emitted when a new directory is added
    void directoryAdded(const QString& parent, const QString& name);

    /// Emitted when a directory is removed
    void directoryRemoved(const QString& parent, const QString& name);

    /// Emitted when a directory is moved
    void directoryMoved(const QString& fromParentDirectory, const QString& fromDirectoryName,
                        const QString& toParentDirectory, const QString& toDirectoryName);

private:
    /// Add a directory to inotify
    void addWatchToDir(const QString& path);

    /// Remove a directory from inotify
    void removeWatchFromDir(const QString& path);

    /// INotify callback
    void onEventTriggered(const QINotify::Event& event);

    /// Emitted when the move timer time out
    void onMoveTimerTimeout();

    struct QINotifyFileSystemWatcherImpl;
    QINotifyFileSystemWatcherImpl* m_impl = nullptr;
};

#endif // QINOTIFYFILESYSTEMWATCHER_H
