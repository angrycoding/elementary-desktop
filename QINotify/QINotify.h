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

#ifndef QINOTIFY_H
#define QINOTIFY_H

#include <QObject>
#include <QString>

/// Qt inotify convenience wrapper
class QINotify : public QObject
{
    Q_OBJECT

public:
    enum EventType : uint32_t {
        AccessEvent = 0x1,
        ModifyEvent = 0x2,
        AttributeChangeEvent = 0x4,
        WritableFileCloseEvent = 0x8,
        UnwritableFileCloseEvent = 0x10,
        CloseEvent = WritableFileCloseEvent | UnwritableFileCloseEvent,
        OpenEvent = 0x20,
        ChildMovedFromEvent = 0x40,
        ChildMovedToEvent = 0x80,
        ChildMoveEvent = (ChildMovedFromEvent | ChildMovedToEvent),
        ChildCreateEvent = 0x100,
        ChildDeleteEvent = 0x200,
        DeleteEvent = 0x400,
        MoveEvent = 0x800,
    };

    Q_DECLARE_FLAGS(EventTypes, EventType)
    Q_FLAG(EventTypes)

    enum EntityType {
        DirectoryEntity,
        FileEntity
    };

    struct Event {
        EntityType childEntityType;
        EventTypes eventType;
        uint32_t eventCookie;
        QString childEntityName;
        int32_t sourceWatchDescriptor;
    };

    QINotify(QObject* parent = nullptr);
    QINotify(const QINotify&) = delete;
    QINotify& operator=(const QINotify&) = delete;
    ~QINotify();

    /// Watch the given path for changes
    int AddWatch(const QString& path);

    /// Remove the watch from the given path
    void RemoveWatch(int wd);

signals:
    /// Emitted when an inotify event occur
    void eventTriggered(const Event& event);

private:
    /// inotify callback
    void onDataAvailable();

    struct QINotifyImpl;
    QINotifyImpl* m_impl = nullptr;
};

#endif // QINOTIFY_H
