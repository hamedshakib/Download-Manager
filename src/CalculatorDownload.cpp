#include "HeaderAndUi/CalculatorDownload.h"

CalculatorDownload::CalculatorDownload(QObject *parent)
	: QObject(parent)
{
}

CalculatorDownload::~CalculatorDownload()
{
}

/*
qint64 CalculatorDownloadSpeed::CalculateDownloadSpeed(qint64 NowDownloadedbyte,qint64 SpentedTime_Millisecond)
{
	//Bytes per Millisecond 

	qint64 DiffrencebytesDownloaded = NowDownloadedbyte - downloadedByteAtLastTime;
	downloadedByteAtLastTime = NowDownloadedbyte;
	if (SpentedTime_Millisecond == 0)
	{
		return 0;
	}
	LastSpeedBytesPerMillisecond = (DiffrencebytesDownloaded / SpentedTime_Millisecond);
	return LastSpeedBytesPerMillisecond;
}
*/

qint64 CalculatorDownload::CalculateDownloadSpeed(qint64 NumberOfBytesThatDownloadedInLastPeriod, qint64 SpentedTime_Millisecond)
{
	/*if (SpentedTime_Millisecond == 0)
	{
		return 0;
	}
	LastSpeedBytesPerMillisecond = (NumberOfBytesThatDownloadedInLastPeriod / SpentedTime_Millisecond);
	return LastSpeedBytesPerMillisecond;
	*/


	if (SpentedTime_Millisecond == 0)
	{
		return 0;
	}

	CurrentSpeedBytesPerMillisecond = (NumberOfBytesThatDownloadedInLastPeriod / SpentedTime_Millisecond);
	CalculateDownloadSpeedAccordingToLastSpeeds();
	PutLastSpeeds();


	return AvrageSpeedBytesPerMillisecond;
}


QString CalculatorDownload::GetSpeedOfDownloadInFormOfString()
{
	//bytes per second
	QString SpeedInString;
	qint64 Speed_BytesPerSecond = AvrageSpeedBytesPerMillisecond * 1000;
	if (Speed_BytesPerSecond < 1024)
	{
		SpeedInString = QString::number(Speed_BytesPerSecond) + " Bytes/sec";
	}
	else if (Speed_BytesPerSecond >= 1024 && Speed_BytesPerSecond < 1024 * 1024)
	{
		SpeedInString = QString::number(Speed_BytesPerSecond/1024) + " KBytes/sec";
	}
	else if (Speed_BytesPerSecond >= 1024 * 1024 && Speed_BytesPerSecond < 1024 * 1024 * 1024)
	{
		SpeedInString = QString::number(Speed_BytesPerSecond / 1024*1024) + " MBytes/sec";
	}
	else if (Speed_BytesPerSecond >= 1024 * 1024 * 1024 && Speed_BytesPerSecond < 1024 * 1024 * 1024 * 1024)
	{
		SpeedInString = QString::number(Speed_BytesPerSecond / 1024 * 1024 *1024) + " GBytes/sec";
	}
	return SpeedInString;
}

QString CalculatorDownload::GetTimeLeftOfDownloadInFormOfString(qint64 NumberRemainedBytes)
{
	qint64 Speed_BytesPerSecond = AvrageSpeedBytesPerMillisecond * 1000;
	if (Speed_BytesPerSecond > 0)
	{
		RemainedTimeToFinish = QTime(0, 0, 0);
		RemainedTimeToFinish =RemainedTimeToFinish.addSecs(NumberRemainedBytes / Speed_BytesPerSecond);
	}


	QString RemainedTimeString;
	if (RemainedTimeToFinish.hour() > 0)
	{
		RemainedTimeString= RemainedTimeToFinish.toString("hh") + " hour(s) " + RemainedTimeToFinish.toString("mm") + " min";
	}
	else if (RemainedTimeToFinish.minute() > 0)
	{
		RemainedTimeString = RemainedTimeToFinish.toString("mm")+" min "+ RemainedTimeToFinish.toString("ss")+" sec";
	}
	else
	{
		RemainedTimeString = RemainedTimeToFinish.toString("ss")+" sec";
	}
	return RemainedTimeString;
}

QString CalculatorDownload::getStatusForTable(qint64 DownloadedSize,qint64 SizeDownload)
{
	QString DownloadStatus;

	float Present = (long double)DownloadedSize / SizeDownload;
	DownloadStatus = QString::number(Present*100, 'f', 2) +"%";
	return DownloadStatus;
}

void CalculatorDownload::CalculateDownloadSpeedAccordingToLastSpeeds()
{
	qint64 SumOfSpeed;
	int numberOfEffectedSpeed;
	if (LastSpeeds[0] == 0)
	{
		//just Last speed
		numberOfEffectedSpeed = 1;
		SumOfSpeed = CurrentSpeedBytesPerMillisecond;
	}
	else if (LastSpeeds[1] == 0)
	{
		//Effected 2 speeds
		numberOfEffectedSpeed = 2;
		SumOfSpeed = LastSpeeds[2] +CurrentSpeedBytesPerMillisecond;
	}
	else if (LastSpeeds[2] == 0)
	{
		//Effected 3 speeds
		numberOfEffectedSpeed = 3;
		SumOfSpeed = LastSpeeds[1]+LastSpeeds[2] + CurrentSpeedBytesPerMillisecond;
	}
	else
	{
		//Effected 4 speeds
		numberOfEffectedSpeed = 4;
		SumOfSpeed = LastSpeeds[1] +LastSpeeds[1] + LastSpeeds[2] + CurrentSpeedBytesPerMillisecond;
	}
	AvrageSpeedBytesPerMillisecond = (SumOfSpeed / numberOfEffectedSpeed);
}

void CalculatorDownload::PutLastSpeeds()
{
	LastSpeeds[0] = LastSpeeds[1];
	LastSpeeds[1] = LastSpeeds[2];
	LastSpeeds[2] = CurrentSpeedBytesPerMillisecond;
}
