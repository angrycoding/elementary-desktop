#ifndef FSWATCHER_H
#define FSWATCHER_H
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <functional>
#include <QObject>
#include <QFileInfo>
#include <QFileSystemWatcher>

class FSWatcher: public QObject {

	Q_OBJECT

private:



	QString watchingDir;
	std::function<void (QMap<unsigned long, QFileInfo>)> callback;
	QFileSystemWatcher *fileSystemWatcher;
	void directoryChanged();
	unsigned long getInode(QString path);

public:

	explicit FSWatcher(const QString &watchDir, std::function<void(QMap<unsigned long, QFileInfo>)>callback, QObject *parent = nullptr);
	~FSWatcher();

};

#endif // FSWATCHER_H
