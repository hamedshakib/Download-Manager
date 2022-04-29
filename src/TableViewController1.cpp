#include "HeaderAndUi/TableViewController.h"

TableViewController::TableViewController(QTableView* tableView,QObject *parent)
	: QObject(parent)
{
	m_tableView = tableView;
	listOfColomns << "id" << tr("File Name") << tr("Size") << tr("Status") << tr("Speed") << tr("Time Left") << tr("Last Try Time") << tr("Description") << tr("Save To");
	model = new QStandardItemModel(this);

	//tableView->setDragDropMode(QAbstractItemView::DragDropMode::DragOnly);
	//tableView->setDragEnabled(true);
	//tableView->setDragEnabled(true);//somewhere in constructor
	
}

TableViewController::~TableViewController()
{
}

void TableViewController::Set_DownloadManager(DownloadManager* downloadManager)
{
	m_downloadManager = downloadManager;
	connect(m_downloadManager, &DownloadManager::CreatedNewDownload, this,&TableViewController::AddNewDownloadToTableView);
	connect(m_downloadManager, &DownloadManager::CreatedDownloader, this, &TableViewController::ConnectorDownloaderToTableUpdateInDownloading);
}

void TableViewController::ProcessSetupOfTableView()
{
	model->setHorizontalHeaderLabels(listOfColomns);
	m_tableView->setModel(model);

	


	
	//Set Header 
	horizontalHeader = m_tableView->horizontalHeader();
	horizontalHeader->setSectionsMovable(true);
	horizontalHeader->setContextMenuPolicy(Qt::CustomContextMenu);     //set contextmenu
	connect(horizontalHeader, &QHeaderView::customContextMenuRequested, this, &TableViewController::OnHeaderRightClicked);


	for (int columNumber = 1; columNumber < listOfColomns.count(); columNumber++)
	{
		QString SettingStringKey = "TableView/MainTableView/WidthColum" + QString::number(columNumber);
		int ColumWidth = SettingInteract::GetValue(SettingStringKey).toInt();
		m_tableView->setColumnWidth(columNumber, ColumWidth);
	}

	connect(horizontalHeader, &QHeaderView::sectionResized, this, [&](int numberOfColum, int oldsize, int newSize) {ChangeColumnWidth(numberOfColum, newSize); });

	
	//Set double click on rows 
	//connect(m_tableView, &QTableView::doubleClicked, this, &TableViewController::doubleClickedOnRow);
	connect(m_tableView, &QTableView::doubleClicked, this, [&](const QModelIndex& modelindex) {doubleClickedOnRow(modelindex); ClickedOnRow(modelindex); });



	
	//Set right click on rows
	m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_tableView, &QTableView::customContextMenuRequested, this, &TableViewController::ProcessCheckAndApply_RightClickOnTable);


	//Set Just Clicked on rows
	connect(m_tableView, &QTableView::clicked, this, &TableViewController::ClickedOnRow);

	
	AdjusteTableViewProperty();

	
	LoadAllDownloadsFromDatabaseForMainTableView();

}

void TableViewController::AdjusteTableViewProperty()
{
	m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_tableView->verticalHeader()->setVisible(false);
	m_tableView->hideColumn(0);
}

void TableViewController::OnHeaderRightClicked(const QPoint& pos)
{
	//ToDo Make Colomns Settings
	QAction ColomnsSetting(tr("Colomns..."), this);
	QMenu menu;
	menu.addAction(&ColomnsSetting);
	menu.exec(QCursor::pos());
}

void TableViewController::doubleClickedOnRow(const QModelIndex& modelindex)
{
	int Download_id=FindDownloadIdFromRow(modelindex);
	//TODO Load a Download From Database Or ListOfDownload
	Download* doubleClickedDownload = m_downloadManager->ProcessAchieveDownload(Download_id);
	if (doubleClickedDownload->get_Status() == Download::DownloadStatusEnum::Completed)
	{
		QString UrlOfFile=doubleClickedDownload->get_SavaTo().toString();
		OpenFileForUser::openFileForShowUser(UrlOfFile);
	}
	else
	{
		//Not Completed Download
		Downloader *downloader=m_downloadManager->ProcessAchieveDownloader(doubleClickedDownload);
		ShowDownloadWidget* showDownloadWidget;

		if (MapOfShowDownloadWidgets.find(downloader) != MapOfShowDownloadWidgets.end())
		{
			//found
			showDownloadWidget = MapOfShowDownloadWidgets.find(downloader).value();
			//MapOfShowDownloadWidgets.insert(downloader, showDownloadWidget);
		}
		else
		{
			//not found
			showDownloadWidget = CreaterShowDownloadWidget(downloader);
			MapOfShowDownloadWidgets.insert(downloader, showDownloadWidget);
		}
		showDownloadWidget->show();

	}
}

int TableViewController::FindDownloadIdFromRow(const QModelIndex& modelindex)
{
	int row = m_tableView->currentIndex().row();
	QModelIndex index = model->index(row, 0);
	return model->data(index).toInt();
}

void TableViewController::LoadAllDownloadsFromDatabaseForMainTableView()
{
	manager = new DatabaseManager(this);
	manager->LoadAllDownloadsForMainTable(model);
}

void TableViewController::ProcessCheckAndApply_RightClickOnTable(const QPoint& point)
{
	int row = m_tableView->indexAt(point).row();
	QModelIndex index = model->index(row, 0);
	if (!index.isValid())
		return;


	int Download_id = model->data(index).toInt();

	qDebug() << "Download id right:" << Download_id;

	QMenu* menu = CreaterRightClickMenuForRowRightClicked(Download_id);
	menu->popup(m_tableView->viewport()->mapToGlobal(point));

	ClickedOnRow(index);
}

void TableViewController::GetDownloaderOfDownloadId(int DownloadId)
{
	
}

QMenu* TableViewController::CreaterRightClickMenuForRowRightClicked(int Download_id)
{

	Download* RightClickedRow_Download=m_downloadManager->ProcessAchieveDownload(Download_id);
	Download::DownloadStatusEnum status = RightClickedRow_Download->get_Status();
	QMenu* menu = new QMenu(m_tableView);


	//Open Item For Menu
	if (status == Download::DownloadStatusEnum::Completed)
	{
		QAction* OpenAction = new QAction(tr("Open"), this);
		menu->addAction(OpenAction);
		connect(OpenAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {qDebug() << "test1"; OpenFileActionTriggered(RightClickedRow_Download); });



		QAction *OpenWithAction= new QAction(tr("Open with"), this);
		menu->addAction(OpenWithAction);
		connect(OpenWithAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {qDebug() << "test1"; OpenFileWithActionTriggered(RightClickedRow_Download); });

		QAction* OpenFolderAction = new QAction(tr("Open folder"), this);
		menu->addAction(OpenFolderAction);
		connect(OpenFolderAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {qDebug() << "test1"; OpenFolderActionTriggered(RightClickedRow_Download); });

	}
	





	//Resume/Pause Item For Menu
	QAction* ResumeOrPause;
	if ((status == Download::DownloadStatusEnum::Pause) || (status== Download::DownloadStatusEnum::NotStarted))
	{
		ResumeOrPause = new QAction(this);
		ResumeOrPause->setText(tr("Resume"));
		ResumeOrPause->setVisible(true);
		menu->addAction(ResumeOrPause);
		connect(ResumeOrPause, &QAction::triggered, this, [&, ResumeOrPause, RightClickedRow_Download](bool clicked) {qDebug() << "test2"; PauseOrResumeActionTriggered(ResumeOrPause, RightClickedRow_Download); });

	}
	else if (status == Download::DownloadStatusEnum::Downloading)
	{
		QAction* ResumeOrPause = new QAction(this);
		ResumeOrPause->setText(tr("Pause"));
		ResumeOrPause->setVisible(true);
		menu->addAction(ResumeOrPause);
		connect(ResumeOrPause, &QAction::triggered, this, [&, ResumeOrPause, RightClickedRow_Download](bool clicked) {qDebug() << "test2"; PauseOrResumeActionTriggered(ResumeOrPause, RightClickedRow_Download); });

	}


	//Add To Queue or Remove From Queue
	if (status != Download::DownloadStatusEnum::Completed)
	{
		if (RightClickedRow_Download->get_QueueId() > 0)
		{
			QAction* RemoveFromQueueAction = new QAction(this);
			RemoveFromQueueAction->setText(tr("Delete From Queue"));
			RemoveFromQueueAction->setVisible(true);
			menu->addAction(RemoveFromQueueAction);
			//Todo Edit Connect
			connect(RemoveFromQueueAction, &QAction::triggered, this, [&, RemoveFromQueueAction, RightClickedRow_Download](bool clicked) {qDebug() << "test2"; RemoveDownloadFromQueue(RightClickedRow_Download); sender()->deleteLater(); });
		}
		else
		{
			QMenu *AddToQueueMenu=new QMenu(tr("Add To Queue"));
			QList<QAction*> ListOfQueueAction;
			for (Queue* queue : queueManager->Get_ListOfQueues())
			{
				QAction* AddToQueue = new QAction(queue->Get_QueueName());
				connect(AddToQueue, &QAction::triggered, this, [&, queue, RightClickedRow_Download, ListOfQueueAction]() {AddDownloadToQueue(queue, RightClickedRow_Download); qDeleteAll(ListOfQueueAction); });
				ListOfQueueAction.append(AddToQueue);
				AddToQueueMenu->addAction(AddToQueue);
			}
			menu->addMenu(AddToQueueMenu);
		}


	}

	menu->addSeparator();



	//Remove Action
	QAction* RemoveDownloadAction = new QAction(tr("Remove"), this);
	RemoveDownloadAction->setVisible(true);
	menu->addAction(RemoveDownloadAction);
	connect(RemoveDownloadAction, &QAction::triggered, this, [&, RightClickedRow_Download](bool clicked) {qDebug() << "test1"; RemoveActionTriggered(RightClickedRow_Download); });

	menu->addSeparator();


	//Properties Item For Menu
	QAction* PropertiesAction = new QAction(tr("Properties"), this);
	PropertiesAction->setVisible(true);
	menu->addAction(PropertiesAction);

	connect(PropertiesAction, &QAction::triggered, this, [&,RightClickedRow_Download](bool clicked) {qDebug() << "test2"; PropertiesActionTriggered(RightClickedRow_Download); });


	return menu;
}

void TableViewController::ConnectorDownloaderToTableUpdateInDownloading(Downloader* downloader)
{
	size_t Download_id= downloader->Get_Download()->get_Id();
	size_t Row = 0;
	for (size_t i = 0; i < model->rowCount();i++)
	{
		qDebug() << model->index(i, 0).data().toInt();
		if (model->index(i, 0).data().toInt() == Download_id)
		{
			Row = i;
			break;
		}
	}
	if (Row >= 0)
	{
		connect(downloader, &Downloader::SignalForUpdateDownloading, this, [&,Row](QString Status,QString Speed, QString TimeLeft) {UpdateRowInDownloading(Row,Status,Speed,TimeLeft); });
		connect(downloader, &Downloader::Started, this, [&,Row, downloader]() {
			//Update LastStartedTime
			model->setData(model->index(Row, 6), DateTimeManager::ConvertDataTimeToString(downloader->Get_Download()->get_LastTryTime()));
			});
	}
}

bool TableViewController::UpdateRowInDownloading(size_t row,QString Status,QString Speed, QString TimeLeft)
{
	Downloader *downloader=static_cast<Downloader*>(sender());

	QModelIndex Status_index = model->index(row, 3);
	QModelIndex Speed_index = model->index(row, 4);
	QModelIndex TimeLeft_index = model->index(row, 5);

	if (downloader->Get_Download()->get_Status() == Download::DownloadStatusEnum::Completed)
	{
		Status = tr("Complete");
		Speed = "";
		TimeLeft = "";
	}
	model->setData(Status_index, Status);
	model->setData(Speed_index, Speed);
	model->setData(TimeLeft_index, TimeLeft);

	
	return true;
}

void TableViewController::AddNewDownloadToTableView(Download* download)
{
	model->appendRow(TableViewRowCreater::PrepareDataForRow(download));
}

ShowDownloadWidget* TableViewController::CreaterShowDownloadWidget(Downloader* downloader)
{
	ShowDownloadWidget* showDownload = new ShowDownloadWidget(downloader);
	showDownload->ProcessSetup();
	return showDownload;
}

void TableViewController::PauseOrResumeActionTriggered(QAction* pauseOrResumeAction,Download* download)
{

	if (pauseOrResumeAction->text() == tr("Resume"))
	{
		m_downloadManager->ProcessAchieveDownloader(download)->StartDownload();
	}
	else if (pauseOrResumeAction->text() == tr("Pause"))
	{
		m_downloadManager->ProcessAchieveDownloader(download)->PauseDownload();
	}
}

void TableViewController::OpenFileActionTriggered(Download* download)
{
	QString UrlOfFile = download->get_SavaTo().toString();
	OpenFileForUser::openFileForShowUser(UrlOfFile);
}

void TableViewController::OpenFileWithActionTriggered(Download* download)
{
	QString UrlOfFile = download->get_SavaTo().toString();
	OpenFileForUser::openWithFileForShowUser(UrlOfFile);
}

void TableViewController::OpenFolderActionTriggered(Download* download)
{
	QString UrlOfFile = download->get_SavaTo().toString();
	OpenFileForUser::OpenFolderForShowUser(UrlOfFile);
}

void TableViewController::RemoveActionTriggered(Download* download)
{
	m_downloadManager->ProcessRemoveDownload(download);
	int row = m_tableView->currentIndex().row();
	model->removeRow(row);
}

void TableViewController::PropertiesActionTriggered(Download* download)
{
	qDebug() << "ddd1";
	ShowDownloadProperties *showProperties=new ShowDownloadProperties(download);
	showProperties->ShowPropertiesOfDownload();
}

void TableViewController::Set_QueueManager(QueueManager* queueManager)
{
	this->queueManager = queueManager;
}

void TableViewController::AddDownloadToQueue(Queue* queue, Download* download)
{
	queueManager->AddDownloadToQueue(download, queue);
}

void TableViewController::RemoveDownloadFromQueue(Download* download)
{
	//queueManager->RemoveDownloadFromQueue(download);
	queueManager->ProcessRemoveADownloadFromQueue(download);
}

void TableViewController::ClickedOnRow(const QModelIndex& modelindex)
{
	int Download_id=FindDownloadIdFromRow(modelindex);

	int row = m_tableView->currentIndex().row();
	QModelIndex Statusindex = model->index(row, 3);
	QString Status = model->data(Statusindex).toString();
	if (Status == tr("Complete"))
	{
		SelectedFinishedDownload_id = Download_id;
		emit SelectedDownloadChanged(Download_id,true);
	}
	else
	{
		emit SelectedDownloadChanged(Download_id,false);
	}
}

int TableViewController::Get_SeletedFinisedDownloadId()
{
	return SelectedFinishedDownload_id;
}

void TableViewController::ChangeColumnWidth(int numberOfColumn, int NewColumnWidth)
{
	QString SettingStringKey = "TableView/MainTableView/WidthColum" + QString::number(numberOfColumn);
	SettingInteract::SetValue(SettingStringKey, NewColumnWidth);
}