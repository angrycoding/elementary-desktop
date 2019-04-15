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

#include "QINotify.h"

#include <errno.h>
#include <poll.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <QFileInfo>
#include <QDir>
#include <QFile>

#include <memory>

#include <QSocketNotifier>

#include <unordered_map>

struct EntityData {
    int watchDescriptor;
};

struct QINotify::QINotifyImpl {
    int m_fd = -1;
    std::unique_ptr<QSocketNotifier> m_notifier;
};

QINotify::QINotify(QObject *parent)
    : QObject(parent)
    , m_impl(new QINotify::QINotifyImpl())
{
    m_impl->m_fd = inotify_init1(IN_NONBLOCK);
    m_impl->m_notifier.reset(new QSocketNotifier(m_impl->m_fd, QSocketNotifier::Read));
    QObject::connect(m_impl->m_notifier.get(), &QSocketNotifier::activated, this, &QINotify::onDataAvailable);
}

QINotify::~QINotify()
{
    if (m_impl->m_fd != -1)
        ::close(m_impl->m_fd);
    delete m_impl;
}

int QINotify::AddWatch(const QString &path)
{
    static const uint32_t dirMask = ModifyEvent | MoveEvent | DeleteEvent |
                                    ChildCreateEvent | ChildDeleteEvent | ChildMoveEvent;
    static const uint32_t fileMask = ModifyEvent | MoveEvent | DeleteEvent | AttributeChangeEvent;

    QFileInfo info(path);
    return inotify_add_watch(m_impl->m_fd, path.toStdString().c_str(), IN_ALL_EVENTS);
}

void QINotify::RemoveWatch(int wd)
{
    inotify_rm_watch(m_impl->m_fd, wd);
}

void QINotify::onDataAvailable()
{
    alignas(alignof (inotify_event)) std::array<uint8_t, 4096> m_inotify_buffer;

    for (;;) {
        ssize_t len = ::read(m_impl->m_fd, m_inotify_buffer.data(), m_inotify_buffer.size());
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (len <= 0)
            break;

        uint8_t* ptr = m_inotify_buffer.data();

        while (ptr < (m_inotify_buffer.data() + len)) {
            auto event = reinterpret_cast<const inotify_event *>(ptr);

            Event notify_event;
            notify_event.childEntityType = (event->mask & IN_ISDIR) ? DirectoryEntity : FileEntity;
            notify_event.childEntityName = QString::fromUtf8(event->name);
            notify_event.eventCookie = event->cookie;
            notify_event.eventType = static_cast<EventType>(event->mask);
            notify_event.sourceWatchDescriptor = event->wd;
            emit eventTriggered(notify_event);

            ptr += sizeof(inotify_event) + event->len;
        }
    }
}
