#pragma once

#include <QObject>
#include "PartDownload.h"
//#include "ProcessDatabaseOutput.h"
#include "qfile.h"
#include "qtimer.h"
#include "qdatetime.h"
#include "qurl.h"
#include "qlist.h"

class Download : public QObject
{
	Q_OBJECT


public:
	enum DownloadStatusEnum
	{
		NotStarted,
		Downloading,
		Pause,
		Completed
	};
	enum ResumeCapabilityEnum
	{
		UnKnown,
		Yes,
		No
	};

	struct Category
	{
		qint32 IdCategory;
		QString Name;
		QUrl BaseUrl;
	};


private:
	qint32 IdDownload;
	QString FileName;
	size_t MaxSpeed; //Kilobytes
	qint64 DownloadSize;
	qint64 SizeDownloaded;
	QFile* CompletedFile = nullptr;
	QString description;
	DownloadStatusEnum downloadStatus;
	QTime TimeLeft;
	QDateTime LastTryTime;
	QUrl Url;
	QString FullFileName;
	QUrl SaveTo;
	QString suffix;
	Category Category;
	QList<PartDownload*> DownloadParts;
	ResumeCapabilityEnum ResumeCapability;
	int Queue_id = -1;
	QString Username;
	QString Password;




	friend class ProcessDatabaseOutput;
	//friend class NewDownloadCreater;
	friend class DatabaseQueryPreparer;
	friend class TableViewRowCreater;
	friend class ShowDownloadWidget;
	friend class DownloadControl;

public:
	QUrl get_Url();
	size_t get_MaxSpeed();
	QList<PartDownload*> get_PartDownloads();
	QUrl get_SavaTo();
	size_t get_Id();
	DownloadStatusEnum get_Status();
	int get_QueueId();
	QDateTime get_LastTryTime();

	QString get_FullFileName();
	QString get_Description();
	qint64 get_SizeDownload();
	qint64 get_DownloadedSize();

	void AppendPartDownloadToPartDownloadListOfDownload(PartDownload* partDownload);


	//new
	QString get_Username();
	QString get_Password();
	QTime get_TimeLeft();
	QString get_FileName();
	QString get_Suffix();
	ResumeCapabilityEnum get_ResumeCapability();

	void Set_Username(QString);
	void Set_Password(QString);
	void Set_Url(QUrl );
	void Set_MaxSpeed(size_t);
	void Set_SavaTo(QUrl);
	void Set_Id(size_t);
	void Set_downloadStatus(Download::DownloadStatusEnum status);
	void Set_QueueId(int Queue_id);
	void Set_LastTryTime(QDateTime);
	void Set_FullFileName(QString);
	void Set_Description(QString);
	void Set_SizeDownload(qint64);
	void Set_DownloadedSize(qint64);
	void Set_TimeLeft(QTime);
	void Set_ResumeCapability(ResumeCapabilityEnum);
	void Set_FileName(QString);
	void Set_Suffix(QString);

signals:
	void DownloadStatusChanged(Download::DownloadStatusEnum NewStatus);

public:
	Download(QObject *parent=nullptr);
	~Download();
};
