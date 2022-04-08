#include "HeaderAndUi/PartDownloader.h"

PartDownloader::PartDownloader(QObject *parent)
	: QObject(parent)
{
}

PartDownloader::~PartDownloader()
{
	qDebug() << "delete PartDownloader";
}

bool PartDownloader::Set_PartDownload(PartDownload* partDownload)
{
	this->partDownload = partDownload;
	return true;
}

qint64 PartDownloader::ReadReadybytes(qint64 bytes)
{
	/*
	QByteArray byteArray;
	mutex.lock();
	if ((partDownload->PartDownloadFile->size() + bytes) < partDownload->end_byte)
	{
		byteArray=reply->read(bytes);
		if (DownloadFileWriter::WriteDownloadToFile(byteArray, partDownload->PartDownloadFile))
		{
			
		}
		mutex.unlock();
		return true;
	}
	else if ((partDownload->PartDownloadFile->size() + bytes) > partDownload->end_byte)
	{
		byteArray=reply->read(partDownload->end_byte - partDownload->PartDownloadFile->size());
		if (DownloadFileWriter::WriteDownloadToFile(byteArray, partDownload->PartDownloadFile))
		{

		}
		mutex.unlock();
		return false;
	}
	else if ((partDownload->PartDownloadFile->size() + bytes) == partDownload->end_byte)
	{
		byteArray=reply->read(partDownload->PartDownloadFile->size() + bytes);
		if (DownloadFileWriter::WriteDownloadToFile(byteArray, partDownload->PartDownloadFile))
		{

		}
		mutex.unlock();
	}
	mutex.unlock();
	return false;
	*/


	qint64 ReadedBytes = 0;
	QByteArray byteArray;

	if (bytes == -1 || reply->bytesAvailable() <= bytes)
	{
		ReadedBytes = reply->bytesAvailable();
		byteArray = reply->read(ReadedBytes);
		qDebug() << "ReadedBytes:" << ReadedBytes;
	}
	else
	{
		byteArray = reply->read(bytes);
		ReadedBytes = bytes;
	}

	if (DownloadFileWriter::WriteDownloadToFile(byteArray, partDownload->PartDownloadFile))
	{
		return ReadedBytes;
	}

	/*
	if (reply->isFinished())
	{
		partDownload->PartDownloadFile->close();
		emit Finished();
	}
	*/

	return true;
}

/*
bool PartDownloader::StartDownload()
{
	is_Downloading = true;
	return true;
}

bool PartDownloader::PauseDownload()
{
	is_Downloading = false;

}

bool PartDownloader::StopDownload()
{
	is_Downloading = false;
	reply->abort();
	return true;
}
*/


bool PartDownloader::Set_NetworkReply(QNetworkReply* reply)
{
	this->reply = reply;
	connect(reply, &QNetworkReply::finished, this, [&]() {
		ReadReadybytes();
		this->partDownload->PartDownloadFile->close();
		emit Finished(); });

	return true;
}

