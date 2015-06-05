#include "mainwindow.h"

mainWindow::mainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	//������ GUI ��������� ���������
	ui.setupUi(this);

	//��������� ���� � ����������� ��������� ��������
	if(!init_file_lastdiagram())
	{
		QMessageBox::warning(this, "������", "�� ������� ��������� ���������� � ��������� ����������");
		exit(0);
	}

	//������� �����
	scene = new GraphicScene();

	//����������� ����� � ������������� �����
	ui.canvas->setScene(scene);

	//������������� ������� ����������� �����
	ui.canvas->setSceneRect(0, 0, scene_width, scene_height);

	//���������� �������� � ������
	connect(ui.addObject, SIGNAL(clicked()), this, SLOT(addDiagramObject()));
	connect(ui.addComment, SIGNAL(clicked()), this, SLOT(addDiagramComment()));
	connect(ui.addMessage, SIGNAL(clicked()), this, SLOT(addMessage()));
	connect(ui.removeObject, SIGNAL(clicked()), this, SLOT(removeItem()));
	connect(ui.addCreationMessage, SIGNAL(clicked()), this, SLOT(addCreationMessage()));
	connect(ui.zoom_in, SIGNAL(clicked()), this, SLOT(zoomIn()));
	connect(ui.zoom_out, SIGNAL(clicked()), this, SLOT(zoomOut()));
	connect(ui.scaleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(scaleScene()));
	connect(ui.addStop, SIGNAL(clicked()), this, SLOT(addStopLine()));
	connect(ui.createFile, SIGNAL(triggered(bool)), this, SLOT(createNewDiagram()));
	connect(ui.openFile, SIGNAL(triggered(bool)), this, SLOT(loadDiagramFromFile()));
	connect(ui.saveFile, SIGNAL(triggered(bool)), this, SLOT(saveDiagramToFile()));
	connect(ui.saveFileAs, SIGNAL(triggered(bool)), this, SLOT(saveAsDiagramToFile()));
	connect(ui.saveImage, SIGNAL(triggered(bool)), this, SLOT(saveAsImage()));
	connect(ui.addLoop, SIGNAL(clicked()), this, SLOT(addLoop()));
	connect(ui.printAction, SIGNAL(triggered(bool)), this, SLOT(printAction()));
	connect(ui.exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
	connect(ui.cutAction, SIGNAL(triggered(bool)), this, SLOT(cutAction()));
	connect(ui.copyAction, SIGNAL(triggered(bool)), this, SLOT(copyAction()));
	connect(ui.pasteAction, SIGNAL(triggered(bool)), this, SLOT(pasteAction()));
	connect(ui.lastdiagram1, SIGNAL(triggered(bool)), this, SLOT(loadDiagramFromLastDiagram()));
	connect(ui.lastdiagram2, SIGNAL(triggered(bool)), this, SLOT(loadDiagramFromLastDiagram()));
	connect(ui.lastdiagram3, SIGNAL(triggered(bool)), this, SLOT(loadDiagramFromLastDiagram()));
	connect(ui.lastdiagram4, SIGNAL(triggered(bool)), this, SLOT(loadDiagramFromLastDiagram()));
	connect(ui.lastdiagram5, SIGNAL(triggered(bool)), this, SLOT(loadDiagramFromLastDiagram()));

	//������� ������ ��� ��������� ��������
	timer_lastdiagram = new QTimer();
	connect(timer_lastdiagram, SIGNAL(timeout()), this, SLOT(refresh_lastdiagram()));
	timer_lastdiagram->start(1000);
}

mainWindow::~mainWindow()
{
	delete scene;
	delete timer_lastdiagram;
	file_lastdiagram->close();
	delete file_lastdiagram;
}

//���� ���������� ������� ��������� ������������������
void mainWindow::addDiagramObject()
{
	//������ ��������� ����� - ����� ����� ��� �������
	scene->setNewState(EStateType::SELECT_PLACE_FOR_OBJECT);
}

//���� ���������� ���������� �����������
void mainWindow::addDiagramComment()
{
	//������ ��������� ����� - ����� ����� ��� ���������� �����������
	scene->setNewState(EStateType::SELECT_PLACE_FOR_COMMENT);
}

//���� ���������� ����������� ���������
void mainWindow::addMessage()
{
	//������ ��������� ����� - ����� ������� �������
	scene->setNewState(EStateType::SELECT_MESSAGE_BEGIN_STATE);
}

//������� ���������� ������
void mainWindow::removeItem()
{
	//������� ���������� ������
	scene->removeSelectedItem();
}

//�������� ��������� ��������
void mainWindow::addCreationMessage()
{
	//������ ��������� ����� - ����� ������� �������
	scene->setNewState(EStateType::SELECT_CREATION_MESSAGE_BEGIN_STATE);
}

//���������������
void mainWindow::scaleScene()
{
	//���������� ������� �������� ���������������
	int idx = ui.scaleCombo->currentIndex();
	int p = 100;
	if(idx == 0)
		p = 50;
	else if(idx == 1)
		p = 75;
	else if(idx == 2)
		p = 100;
	else if(idx == 3)
		p = 150;
	else
		p = 200;

	//�������� �������
	scene->scaleScene(p);

	//��������������� �����
	ui.canvas->setFocus();
}

//����������
void mainWindow::zoomIn()
{
	//�������� �������� ���������������
	int idx = ui.scaleCombo->currentIndex();
	ui.scaleCombo->setCurrentIndex(min(idx + 1, 4));

	//���������������
	scaleScene();

	//��������������� �����
	ui.canvas->setFocus();
}

//��������
void mainWindow::zoomOut()
{
	//�������� �������� ���������������
	int idx = ui.scaleCombo->currentIndex();
	ui.scaleCombo->setCurrentIndex(max(idx - 1, 0));

	//���������������
	scaleScene();

	//��������������� �����
	ui.canvas->setFocus();
}

//��������/������� ��������� ����� �����
void mainWindow::addStopLine()
{
	scene->changeStateOfLineEnd();
	
	//��������������� �����
	ui.canvas->setFocus();
}

//������� ����� ���������
void mainWindow::createNewDiagram()
{
	scene->newDiagram();

	//��������������� �����
	ui.canvas->setFocus();
}

//��������� ��������� �� �����
void mainWindow::loadDiagramFromFile()
{
	bool isOk = false;
	QString filename = QFileDialog::getOpenFileName(this, "������� ���� � ����������","/", "Diagram (*.sd)");
	if(filename != "")
	{
		isOk = scene->openDiagramFromFile(filename);

		if(!isOk)
		{
			QMessageBox::warning(this, "������", "�� ������� ��������� ���������");
			createNewDiagram();
		}
		else
		{
			refresh_file_lastdiagram(filename);
		}
	}

	//��������������� �����
	ui.canvas->setFocus();
}

//��������� ��������� � ����
void mainWindow::saveDiagramToFile()
{
	if(scene->getFilename() == "")
		saveAsDiagramToFile();
	else
	{
		bool isOk = false;
		isOk = scene->saveDiagramToFile("");

		if(!isOk)
			QMessageBox::warning(this, "������", "�� ������� ��������� ���������");
		else
			refresh_file_lastdiagram(scene->getFilename());
	}

	//��������������� �����
	ui.canvas->setFocus();
}

//��������� ��� - ��������� � ����
void mainWindow::saveAsDiagramToFile()
{
	bool isOk = false;
	QString filename = QFileDialog::getSaveFileName(this, "��������� ���� � ����������","/", "Diagram (*.sd)");
	if(filename == "")
		return;
	isOk = scene->saveDiagramToFile(filename);

	if(!isOk)
		QMessageBox::warning(this, "������", "�� ������� ��������� ���������");
	else
		refresh_file_lastdiagram(filename);

	//��������������� �����
	ui.canvas->setFocus();
}

//��������� ��������� � ���� ��������
void mainWindow::saveAsImage()
{
	QString filename = QFileDialog::getSaveFileName(this, "��������� �������� � ����������","/", "PNG (*.png)");
	if(filename == "")
		return;

	//���� ������ �� ������ - ��������
	if(filename.length() < 5 || filename.lastIndexOf(".png") == -1 || filename.lastIndexOf(".png") != filename.length() - 4)
		filename += ".png";

	QImage image(scene->width(), scene->height(), QImage::Format_RGB32);
	image.fill(QColor(Qt::white).rgb());
	QPainter painter(&image);
	scene->render(&painter);
	painter.end();
	if(!image.save(filename))
		QMessageBox::warning(this, "������", "�� ������� ��������� �����������");

	//��������������� �����
	ui.canvas->setFocus();
}

//�������� ����
void mainWindow::addLoop()
{
	//������ ��������� ����� - ��������� ������������� ������� �����
	scene->setNewState(EStateType::SELECT_LOOP_RECTANGLE_FIRST_VERTEX);
}

//��������� ��������� �� ������
void mainWindow::printAction()
{
	QPrinter printer;
	if(QPrintDialog(&printer).exec() == QDialog::Accepted)
	{
		QPainter painter(&printer);
		painter.setRenderHint(QPainter::Antialiasing);
		scene->render(&painter);
	}

	//��������������� �����
	ui.canvas->setFocus();
}

//��������� ���������
void mainWindow::cutAction()
{
	//��������� = ����������� + ��������
	scene->cutSelection();

	//��������������� �����
	ui.canvas->setFocus();

	//������ ��������� ����� - �� �� ���������
	scene->setNewState(EStateType::DEFAULT_STATE);
}

//����������� ���������
void mainWindow::copyAction()
{
	//����������� = ������������ + ����������� � ������������
	//����������� - �.�. � ������� �� ������������ �� �� ����� �����, ����� �� ����� �� �� ������������ �������� �� ����� ��� ����������� ����������� ���������
	//�� ������� ������ ���� ����� �������������� - ���� ������ 1 ������ ��������� � ���� �������� �� ����� ���������� ������ � ���
	scene->copySelection();

	//��������������� �����
	ui.canvas->setFocus();

	//������ ��������� ����� - �� �� ���������
	scene->setNewState(EStateType::DEFAULT_STATE);
}

//������� ���������
void mainWindow::pasteAction()
{
	//������� - �������� ���� ���������
	//�.�. ������� ��� �������� ����� �� ����� �����������, �� � ������ ��� ������� ����� ���� ������ ���� ������, �� ������� ����� ����� ��������� ��� ������ ������� ��� �������
	//�������� ����� ����� ������� �� ������� � �������� ����� ������� ������� �� ������� � ������������ ��� ���������
	scene->pasteFromBuffer();

	//��������������� �����
	ui.canvas->setFocus();

	//������ ��������� ����� - �� �� ���������
	scene->setNewState(EStateType::DEFAULT_STATE);
}

//���������� ���� - ��������� ���������
void mainWindow::refresh_lastdiagram()
{
	mutex_lastdiagram.lock();

	//��������� ������ ������
	//���� ���� ������ � ���������� ��� �������� ������ - ����� � �������
	//������� ��� ��������� ������ � �������
	vector<QString> v_path(5);
	vector<QDateTime> v_time(5);
	file_lastdiagram->seek(0);
	QDataStream in(file_lastdiagram);
	int cnt = 0;
	for(int i = 0; i < 5; i++)
	{
		if(in.atEnd())
			break;
		in >> v_path[i];
		if(in.status() != QDataStream::Ok || in.atEnd())
		{
			QMessageBox::warning(this, "������", "������ ���������� ������� ��������� ���������");
			exit(0);
		}
		if(v_path[i] == "")
			break;

		in >> v_time[i];
		if(in.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "������", "������ ���������� ������� ��������� ���������");
			exit(0);
		}
		cnt++;
	}

	//��������� �� �������
	for(int i = 0; i < cnt; i++)
	{
		for(int j = i + 1; j < cnt; j++)
		{
			if(v_time[i].msecsTo(QDateTime::currentDateTime()) > v_time[j].msecsTo(QDateTime::currentDateTime()))
			{
				swap(v_time[i], v_time[j]);
				swap(v_path[i], v_path[j]);
			}
		}
	}

	//��������� ������� ������� � ��������� ����
	vector<QAction *> actions;
	actions.push_back(ui.lastdiagram1);
	actions.push_back(ui.lastdiagram2);
	actions.push_back(ui.lastdiagram3);
	actions.push_back(ui.lastdiagram4);
	actions.push_back(ui.lastdiagram5);
	for(int i = 0; i < cnt; i++)
	{
		/*
		������ ��� (���� ��������� ���������� ������ 10 ������ �����)
		������ ������ (���� ��������� ���������� �� 10 �� 59 ������)
		��������� ����� ����� (���� ��������� ���������� �� 1 �� 5 ����� �����)
		N ����� ����� (�� 59 �����)
		������ ���� ����� (������ 60 �����)
		*/
		QString res = "";
		if(QDateTime::currentDateTime() < v_time[i])
			res += "� �������";
		else
		{
			QDateTime tmp = v_time[i];
			QDateTime cur = QDateTime::currentDateTime();
			if(tmp.daysTo(cur) > 0)
			{
				res += "������ ���� �����";
			}
			else if(tmp.secsTo(cur) < 60)
			{
				if(tmp.secsTo(cur) < 10)
					res += "������ ���";
				else
					res += "������ ������";
			}
			else if(tmp.secsTo(cur) < 60 * 60)
			{
				int val = tmp.secsTo(cur) / 60;
				if(val <= 5)
					res += "��������� ����� �����";
				else
					res += QString::number(val) + " ����� �����";
			}
			else
				res += "������ ���� �����";
		}

		res += "\t";

		res += v_path[i];

		actions[i]->setText(res);
	}

	//���� ������ ������ 5, �� ��������� ����������� �������� �����
	for(int i = cnt; i < 5; i++)
	{
		actions[i]->setText("<�����>");
	}

	mutex_lastdiagram.unlock();
	timer_lastdiagram->start(1000);
}

//������������� ����� ��������� ��������
bool mainWindow::init_file_lastdiagram()
{
	filename_lastdiagram = ".//lastdiagram_info.bin";
	file_lastdiagram = new QFile(filename_lastdiagram);
	if (!file_lastdiagram->open(QIODevice::ReadWrite))
		return false;
	return true;
}

//���������� ����� � ���������� �����������
void mainWindow::refresh_file_lastdiagram(QString path)
{
	mutex_lastdiagram.lock();

	//��������� �� ����� ������ �� ��� ��� ���� �� ���������� ����� �����, ���� �� ��� ��� ���� �� ������� 5 ��������� ���� ���� �� ������ ������ � �����
	vector<QString> v_path(6);				//������ �����
	vector<QDateTime> v_time(6);			//������ ������ �������������
	file_lastdiagram->seek(0);				//��������� � ������ �����
	QDataStream in(file_lastdiagram);		//������� ����� ��� ������
	int cnt = 0;							//����������, ��� �������� ���������
	for(int i = 0; i < 5; i++)
	{
		//���� ����� �� ����� ����� - ���������
		if(in.atEnd())
			break;

		//��������� ���� � ���� ����� ����� �� ��������� � ����� ����� ��� ���� �����-�� ������ - ��������� ����������
		in >> v_path[i];
		if(in.status() != QDataStream::Ok || in.atEnd())
		{
			QMessageBox::warning(this, "������", "������ ���������� ������� ��������� ���������");
			exit(0);
		}
		//���� ����������� ����� ������ - ���������
		if(v_path[i] == "")
			break;

		//��������� �����
		in >> v_time[i];
		if(in.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "������", "������ ���������� ������� ��������� ���������");
			exit(0);
		}

		//������������ ���-�� ���������
		cnt++;
	}
	//������� ����� path � ����� ����������� ��������
	bool isFind = false;
	for(int i = 0; i < cnt; i++)
	{
		if(v_path[i] == path)
		{
			isFind = true;
			v_time[i] = QDateTime::currentDateTime();
			break;
		}
	}
	if(!isFind)
	{
		v_path[cnt] = path;
		v_time[cnt] = QDateTime::currentDateTime();
		cnt++;
	}

	//��������� �� �������
	for(int i = 0; i < cnt; i++)
	{
		for(int j = i + 1; j < cnt; j++)
		{
			if(v_time[i].msecsTo(QDateTime::currentDateTime()) > v_time[j].msecsTo(QDateTime::currentDateTime()))
			{
				swap(v_time[i], v_time[j]);
				swap(v_path[i], v_path[j]);
			}
		}
	}

	//������� ������ 5 �������� � ���� (��� ������ - ������� �� cnt)
	file_lastdiagram->seek(0);
	QDataStream out(file_lastdiagram);
	for(int i = 0; i < min(cnt, 5); i++)
	{
		out << v_path[i];
		if(out.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "������", "������ ���������� ������� ��������� ���������");
			exit(0);
		}

		out << v_time[i];
		if(out.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "������", "������ ���������� ������� ��������� ���������");
			exit(0);
		}
	}

	mutex_lastdiagram.unlock();
}

//��������� ��������� �� ��������� ������
void mainWindow::loadDiagramFromLastDiagram()
{
	QAction *action = static_cast<QAction *>(QObject::sender());
	if(action->text() == "<�����>")
		return;
	int idx = -1;
	if(QObject::sender()->objectName() == "lastdiagram1")
		idx = 0;
	else if(QObject::sender()->objectName() == "lastdiagram2")
		idx = 1;
	else if(QObject::sender()->objectName() == "lastdiagram3")
		idx = 2;
	else if(QObject::sender()->objectName() == "lastdiagram4")
		idx = 3;
	else if(QObject::sender()->objectName() == "lastdiagram5")
		idx = 4;

	if(idx == -1)
		return;

	mutex_lastdiagram.lock();

	//��������� ������ ������
	//���� ���� ������ � ���������� ��� �������� ������ - ����� � �������
	//������� ��� ��������� ������ � �������
	vector<QString> v_path(5);
	vector<QDateTime> v_time(5);
	file_lastdiagram->seek(0);
	QDataStream in(file_lastdiagram);
	int cnt = 0;
	for(int i = 0; i < 5; i++)
	{
		if(in.atEnd())
			break;
		in >> v_path[i];
		if(in.status() != QDataStream::Ok || in.atEnd())
		{
			QMessageBox::warning(this, "������", "������ ���������� ������� ��������� ���������");
			exit(0);
		}
		if(v_path[i] == "")
			break;

		in >> v_time[i];
		if(in.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "������", "������ ���������� ������� ��������� ���������");
			exit(0);
		}
		cnt++;
	}

	mutex_lastdiagram.unlock();

	//��������� �� �������
	for(int i = 0; i < cnt; i++)
	{
		for(int j = i + 1; j < cnt; j++)
		{
			if(v_time[i].msecsTo(QDateTime::currentDateTime()) > v_time[j].msecsTo(QDateTime::currentDateTime()))
			{
				swap(v_time[i], v_time[j]);
				swap(v_path[i], v_path[j]);
			}
		}
	}

	//����� ����
	QString filename = v_path[idx];

	bool isOk = scene->openDiagramFromFile(filename);

	if(!isOk)
	{
		QMessageBox::warning(this, "������", "�� ������� ��������� ���������");
		createNewDiagram();
	}
	else
	{
		refresh_file_lastdiagram(filename);
	}

	//��������������� �����
	ui.canvas->setFocus();
}