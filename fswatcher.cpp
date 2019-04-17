#include "fswatcher.h"
#include <QDir>
#include <QDebug>

unsigned long FSWatcher::getInode(QString path) {
	QByteArray ba = path.toLocal8Bit();
	const char *c_str2 = ba.data();
	struct ::stat buffer;
	int ret = stat(c_str2, &buffer);
	return buffer.st_ino;
}

FSWatcher::FSWatcher(const QString &watchDir, std::function<void (QMap<unsigned long, QFileInfo>)> callback, QObject *parent) {
	watchingDir = watchDir;
	this->callback = callback;
	fileSystemWatcher = new QFileSystemWatcher();
	QObject::connect(fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &FSWatcher::directoryChanged);
	fileSystemWatcher->addPath(watchDir);
	directoryChanged();
}

void FSWatcher::directoryChanged() {
	QFileInfoList list = QDir(watchingDir).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
	QMap<unsigned long, QFileInfo> entryInfoMap;
		foreach (QFileInfo info, list) {
		unsigned long inode = getInode(info.absoluteFilePath());
		entryInfoMap.insert(inode, info);
	}
	this->callback(entryInfoMap);
}

FSWatcher::~FSWatcher() {
	delete fileSystemWatcher;
}