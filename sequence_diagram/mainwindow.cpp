#include "mainwindow.h"

mainWindow::mainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	//Задаем GUI интерфейс программе
	ui.setupUi(this);

	//Открываем файл с параметрами последних диаграмм
	if(!init_file_lastdiagram())
	{
		QMessageBox::warning(this, "Ошибка", "Не удалось загрузить информацию о последних диаграммах");
		exit(0);
	}

	//Создаем сцену
	scene = new GraphicScene();

	//Привязываем сцену к представлению сцены
	ui.canvas->setScene(scene);

	//Устанавливаем размеры графической сцены
	ui.canvas->setSceneRect(0, 0, scene_width, scene_height);

	//Связывание сигналов и слотов
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

	//Создаем таймер для последних диаграмм
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

//Слот добавления объекта диаграммы последовательности
void mainWindow::addDiagramObject()
{
	//Меняем состояние сцены - Выбор места для объекта
	scene->setNewState(EStateType::SELECT_PLACE_FOR_OBJECT);
}

//Слот добавления свободного комментария
void mainWindow::addDiagramComment()
{
	//Меняем состояние сцены - Выбор места для свободного комментария
	scene->setNewState(EStateType::SELECT_PLACE_FOR_COMMENT);
}

//Слот добавления синхронного сообщения
void mainWindow::addMessage()
{
	//Меняем состояние сцены - Выбор первого объекта
	scene->setNewState(EStateType::SELECT_MESSAGE_BEGIN_STATE);
}

//Удалить выделенный объект
void mainWindow::removeItem()
{
	//Удалить выделенный объект
	scene->removeSelectedItem();
}

//Добавить сообщение создания
void mainWindow::addCreationMessage()
{
	//Меняем состояние сцены - Выбор первого объекта
	scene->setNewState(EStateType::SELECT_CREATION_MESSAGE_BEGIN_STATE);
}

//Масштабирование
void mainWindow::scaleScene()
{
	//Определить текущее значение масштабирования
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

	//Изменить масштаб
	scene->scaleScene(p);

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Приблизить
void mainWindow::zoomIn()
{
	//Изменяем значение масштабирования
	int idx = ui.scaleCombo->currentIndex();
	ui.scaleCombo->setCurrentIndex(min(idx + 1, 4));

	//Масштабирование
	scaleScene();

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Отдалить
void mainWindow::zoomOut()
{
	//Изменяем значение масштабирования
	int idx = ui.scaleCombo->currentIndex();
	ui.scaleCombo->setCurrentIndex(max(idx - 1, 0));

	//Масштабирование
	scaleScene();

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Добавить/Удалить остановку линии жизни
void mainWindow::addStopLine()
{
	scene->changeStateOfLineEnd();
	
	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Создать новую диаграмму
void mainWindow::createNewDiagram()
{
	scene->newDiagram();

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Загрузить диаграмму из файла
void mainWindow::loadDiagramFromFile()
{
	bool isOk = false;
	QString filename = QFileDialog::getOpenFileName(this, "Открыть файл с диаграммой","/", "Diagram (*.sd)");
	if(filename != "")
	{
		isOk = scene->openDiagramFromFile(filename);

		if(!isOk)
		{
			QMessageBox::warning(this, "Ошибка", "Не удалось загрузить диаграмму");
			createNewDiagram();
		}
		else
		{
			refresh_file_lastdiagram(filename);
		}
	}

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Сохранить диаграмму в файл
void mainWindow::saveDiagramToFile()
{
	if(scene->getFilename() == "")
		saveAsDiagramToFile();
	else
	{
		bool isOk = false;
		isOk = scene->saveDiagramToFile("");

		if(!isOk)
			QMessageBox::warning(this, "Ошибка", "Не удалось сохранить диаграмму");
		else
			refresh_file_lastdiagram(scene->getFilename());
	}

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Сохранить как - диаграмму в файл
void mainWindow::saveAsDiagramToFile()
{
	bool isOk = false;
	QString filename = QFileDialog::getSaveFileName(this, "Сохранить файл с диаграммой","/", "Diagram (*.sd)");
	if(filename == "")
		return;
	isOk = scene->saveDiagramToFile(filename);

	if(!isOk)
		QMessageBox::warning(this, "Ошибка", "Не удалось сохранить диаграмму");
	else
		refresh_file_lastdiagram(filename);

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Сохранить диаграмму в виде картинки
void mainWindow::saveAsImage()
{
	QString filename = QFileDialog::getSaveFileName(this, "Сохранить картинку с диаграммой","/", "PNG (*.png)");
	if(filename == "")
		return;

	//Если формат не указан - добавить
	if(filename.length() < 5 || filename.lastIndexOf(".png") == -1 || filename.lastIndexOf(".png") != filename.length() - 4)
		filename += ".png";

	QImage image(scene->width(), scene->height(), QImage::Format_RGB32);
	image.fill(QColor(Qt::white).rgb());
	QPainter painter(&image);
	scene->render(&painter);
	painter.end();
	if(!image.save(filename))
		QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изображение");

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Добавить цикл
void mainWindow::addLoop()
{
	//Меняем состояние сцены - выделение прямоугольной области цикла
	scene->setNewState(EStateType::SELECT_LOOP_RECTANGLE_FIRST_VERTEX);
}

//Отправить диаграмму на печать
void mainWindow::printAction()
{
	QPrinter printer;
	if(QPrintDialog(&printer).exec() == QDialog::Accepted)
	{
		QPainter painter(&printer);
		painter.setRenderHint(QPainter::Antialiasing);
		scene->render(&painter);
	}

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}

//Вырезание элементов
void mainWindow::cutAction()
{
	//Вырезание = копирование + удаление
	scene->cutSelection();

	//Восстанавливаем фокус
	ui.canvas->setFocus();

	//Меняем состояние сцены - на по умолчанию
	scene->setNewState(EStateType::DEFAULT_STATE);
}

//Копирование элементов
void mainWindow::copyAction()
{
	//Копирование = дублирование + модификация к дублированию
	//Модификация - т.к. в отличии от дублирования мы не можем знать, будет ли какой то из некопируемых объектов на сцене для копируемого синхронного сообщения
	//то стрелки любого вида будут игнорироваться - если хотябы 1 объект связанный с этой стрелкой не будет скопирован вместе с ней
	scene->copySelection();

	//Восстанавливаем фокус
	ui.canvas->setFocus();

	//Меняем состояние сцены - на по умолчанию
	scene->setNewState(EStateType::DEFAULT_STATE);
}

//Вставка элементов
void mainWindow::pasteAction()
{
	//Вставка - добавить кучу элементов
	//Т.к. стрелки без объектов точно не будут скопированы, то в наборе как минимум будет либо ничего либо объект, на который можно будет опереться при выборе позиции для вставки
	//Выбираем самый левый элемент не стрелку и выбираем самый верхний элемент не стрелку и относительно них вставляем
	scene->pasteFromBuffer();

	//Восстанавливаем фокус
	ui.canvas->setFocus();

	//Меняем состояние сцены - на по умолчанию
	scene->setNewState(EStateType::DEFAULT_STATE);
}

//Обновление меню - последние диаграммы
void mainWindow::refresh_lastdiagram()
{
	mutex_lastdiagram.lock();

	//Считываем список данных
	//Если есть ошибка в считывании или неверные данные - выйти с ошибкой
	//Заносим все считанные данные в вектора
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
			QMessageBox::warning(this, "Ошибка", "Ошибка выполнения функции последние диаграммы");
			exit(0);
		}
		if(v_path[i] == "")
			break;

		in >> v_time[i];
		if(in.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "Ошибка", "Ошибка выполнения функции последние диаграммы");
			exit(0);
		}
		cnt++;
	}

	//Сортируем по времени
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

	//Вычисляем разницу времени и обновляем меню
	vector<QAction *> actions;
	actions.push_back(ui.lastdiagram1);
	actions.push_back(ui.lastdiagram2);
	actions.push_back(ui.lastdiagram3);
	actions.push_back(ui.lastdiagram4);
	actions.push_back(ui.lastdiagram5);
	for(int i = 0; i < cnt; i++)
	{
		/*
		Только что (если диаграмма изменялась меньше 10 секунд назад)
		Меньше минуты (если диаграмма изменялась от 10 до 59 секунд)
		Несколько минут назад (если диаграмма изменялась от 1 до 5 минут назад)
		N минут назад (до 59 минут)
		Больше часа назад (больше 60 минут)
		*/
		QString res = "";
		if(QDateTime::currentDateTime() < v_time[i])
			res += "В будущем";
		else
		{
			QDateTime tmp = v_time[i];
			QDateTime cur = QDateTime::currentDateTime();
			if(tmp.daysTo(cur) > 0)
			{
				res += "Больше часа назад";
			}
			else if(tmp.secsTo(cur) < 60)
			{
				if(tmp.secsTo(cur) < 10)
					res += "Только что";
				else
					res += "Меньше минуты";
			}
			else if(tmp.secsTo(cur) < 60 * 60)
			{
				int val = tmp.secsTo(cur) / 60;
				if(val <= 5)
					res += "Несколько минут назад";
				else
					res += QString::number(val) + " минут назад";
			}
			else
				res += "Больше часа назад";
		}

		res += "\t";

		res += v_path[i];

		actions[i]->setText(res);
	}

	//если данных меньше 5, то остальные заполняются надписью пусто
	for(int i = cnt; i < 5; i++)
	{
		actions[i]->setText("<Пусто>");
	}

	mutex_lastdiagram.unlock();
	timer_lastdiagram->start(1000);
}

//Инициализация файла последних диаграмм
bool mainWindow::init_file_lastdiagram()
{
	filename_lastdiagram = ".//lastdiagram_info.bin";
	file_lastdiagram = new QFile(filename_lastdiagram);
	if (!file_lastdiagram->open(QIODevice::ReadWrite))
		return false;
	return true;
}

//Обновление файла с последними диаграммами
void mainWindow::refresh_file_lastdiagram(QString path)
{
	mutex_lastdiagram.lock();

	//Считываем из файла данные до тех пор пока не встретится конец файла, либо до тех пор пока не наберем 5 элементов либо пока не найдем ошибку в файле
	vector<QString> v_path(6);				//Список путей
	vector<QDateTime> v_time(6);			//Список времен использования
	file_lastdiagram->seek(0);				//Переходим в начало файла
	QDataStream in(file_lastdiagram);		//Создаем поток для записи
	int cnt = 0;							//Переменная, для подсчета элементов
	for(int i = 0; i < 5; i++)
	{
		//Если дошли до конца файла - завершить
		if(in.atEnd())
			break;

		//Считываем путь и если после этого мы оказались в конце файла или были какие-то ошибки - завершить приложение
		in >> v_path[i];
		if(in.status() != QDataStream::Ok || in.atEnd())
		{
			QMessageBox::warning(this, "Ошибка", "Ошибка выполнения функции последние диаграммы");
			exit(0);
		}
		//Если считываются левые данные - завершить
		if(v_path[i] == "")
			break;

		//Считываем время
		in >> v_time[i];
		if(in.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "Ошибка", "Ошибка выполнения функции последние диаграммы");
			exit(0);
		}

		//Подсчитываем кол-во элементов
		cnt++;
	}
	//Заносим новый path с новым вычисленным временем
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

	//Сортируем по времени
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

	//Заносим первые 5 объектов в файл (или меньше - зависит от cnt)
	file_lastdiagram->seek(0);
	QDataStream out(file_lastdiagram);
	for(int i = 0; i < min(cnt, 5); i++)
	{
		out << v_path[i];
		if(out.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "Ошибка", "Ошибка выполнения функции последние диаграммы");
			exit(0);
		}

		out << v_time[i];
		if(out.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "Ошибка", "Ошибка выполнения функции последние диаграммы");
			exit(0);
		}
	}

	mutex_lastdiagram.unlock();
}

//Загрузить диаграмму из последних файлов
void mainWindow::loadDiagramFromLastDiagram()
{
	QAction *action = static_cast<QAction *>(QObject::sender());
	if(action->text() == "<Пусто>")
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

	//Считываем список данных
	//Если есть ошибка в считывании или неверные данные - выйти с ошибкой
	//Заносим все считанные данные в вектора
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
			QMessageBox::warning(this, "Ошибка", "Ошибка выполнения функции последние диаграммы");
			exit(0);
		}
		if(v_path[i] == "")
			break;

		in >> v_time[i];
		if(in.status() != QDataStream::Ok)
		{
			QMessageBox::warning(this, "Ошибка", "Ошибка выполнения функции последние диаграммы");
			exit(0);
		}
		cnt++;
	}

	mutex_lastdiagram.unlock();

	//Сортируем по времени
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

	//Берем путь
	QString filename = v_path[idx];

	bool isOk = scene->openDiagramFromFile(filename);

	if(!isOk)
	{
		QMessageBox::warning(this, "Ошибка", "Не удалось загрузить диаграмму");
		createNewDiagram();
	}
	else
	{
		refresh_file_lastdiagram(filename);
	}

	//Восстанавливаем фокус
	ui.canvas->setFocus();
}