#include "graphicscene.h"

GraphicScene::GraphicScene(QObject *parent)
	: QGraphicsScene(parent)
{
	//Последний id по умолчанию
	last_id = -1;
	current_state = DEFAULT_STATE;
	diagram_filename = "";

	//Вспомогательные средства для определения состояния по умолчанию
	is_drag = false;
	is_ctrl = false;
	is_shift = false;
	is_move = false;
	is_duplicate = false;

	//Устанавливаем вспомогательные средства drag and drop значениями по умолчанию
	mouse_drag_pos_x = 0;
	mouse_drag_pos_y = 0;

	//Убираем использование BSP дерева из-за бага с удалением
	setItemIndexMethod(QGraphicsScene::NoIndex);

	//Создаем место для хранения данных, для буфера
	mimeData = new QMimeData();
}

GraphicScene::~GraphicScene()
{
	//Удаляем все элементы
	for(int i = 0; i < items.size(); i++)
		delete items[i];
}

//Добавление элемента на сцену
void GraphicScene::addItem(QGraphicsItem *item)
{
	//Присваиваем id элементу
	DiagramItem* it = static_cast<DiagramItem *>(item);
	it->id = ++last_id;

	//Добавляем новый элемент в вектор
	items.push_back(item);

	//Перенаправляем функцию на стандартную
	QGraphicsScene::addItem(item);
}

//Нажата кнопка мыши
/*virtual*/ void GraphicScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	//Выполняем действие в зависимости от текущего состояния
	if(current_state == EStateType::DEFAULT_STATE)
		mousePressEventDefault(event);
	else if(current_state == EStateType::SELECT_PLACE_FOR_OBJECT)
		mousePressEventSelectPlaceForObject(event);
	else if(current_state == EStateType::SELECT_PLACE_FOR_COMMENT)
		mousePressEventSelectPlaceForComment(event);
	else if(current_state == EStateType::SELECT_MESSAGE_BEGIN_STATE)
		mousePressEventSelectMessageBegin(event);
	else if(current_state == EStateType::SELECT_MESSAGE_END_STATE)
		mousePressEventSelectMessageEnd(event);
	else if(current_state == EStateType::SELECT_CREATION_MESSAGE_BEGIN_STATE)
		mousePressEventSelectCreationMessageBegin(event);
	else if(current_state == EStateType::SELECT_CREATION_MESSAGE_END_STATE)
		mousePressEventSelectCreationMessageEnd(event);
	else if(current_state == EStateType::SELECT_LOOP_RECTANGLE_FIRST_VERTEX)
		mousePressEventSelectLoopRectangleFirstVertex(event);
	//Обновить линии жизни
	update_lifelines();
}

//Установить новое состояние для сцены
void GraphicScene::setNewState(EStateType s)
{
	current_state = s;
}

//Удалить выделенный элемент
void GraphicScene::removeSelectedItem()
{
	//Проходим по всем элементам в поисках выделенных элементов - стрелок и комментариев и циклов
	for(int i = 0; i < items.size(); i++)
	{
		//Если элемент не выделен - пропустить
		if(!items[i]->isSelected())
			continue;

		//Если элемент не является стрелкой или свободным комментарием или циклом - пропустить
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::COMMENT_ITEM && it->item_type != EItemsType::MESSAGE_ITEM && it->item_type != EItemsType::LOOP_ITEM)
			continue;

		//Остались только выделенные элементы, которые являются или стрелкой или комментарием

		//Делаем действие в зависимости от типа объекта
		if(it->item_type == EItemsType::COMMENT_ITEM)
		{
			//Свободный комментарий

			//Удаляем из вектора
			items.erase(items.begin() + i);

			//Удаляем со сцены
			this->removeItem(it);

			//Очищаем память
			delete it;
		}
		else if(it->item_type == EItemsType::LOOP_ITEM)
		{
			//Цикл

			//Удаляем из вектора
			items.erase(items.begin() + i);

			//Удаляем со сцены
			this->removeItem(it);

			//Очищаем память
			delete it;
		}
		else if(it->item_type == EItemsType::MESSAGE_ITEM)
		{
			//Стрелка

			//Получаем стрелку
			DiagramArrow *cur_arr = static_cast<DiagramArrow *>(it);
			DiagramObject *target = static_cast<DiagramObject *>(cur_arr->targetItem);

			//Если сообщение создания
			if(cur_arr->isCreationMessageArrow)
			{
				//Поднимаем объект который был создан
				target->setVerticalSpace(vertical_space_default);
			}

			//Удаляем из вектора
			items.erase(items.begin() + i);

			//Удаляем со сцены
			this->removeItem(it);

			//Очищаем память
			delete it;
		}

		//После удаления, остаемся на том же месте в массиве
		i--;
	}

	//Теперь удаляем все выделенные объекты

	//Проходим по всем элементам в поисках выделенных элементов - объектов
	for(int i = 0; i < items.size(); i++)
	{
		//Если элемент не выделен - пропустить
		if(!items[i]->isSelected())
			continue;

		//Если элемент не является объектом - пропустить
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//Остались только выделенные элементы, которые являются объектами

		//Удаляем все сообщения, связанные с этим объектом
		for(int j = 0; j < items.size(); j++)
		{
			//Определяем стрелка или нет
			DiagramItem *cur_obj = static_cast<DiagramItem *>(items[j]);
			if(cur_obj->item_type != EItemsType::MESSAGE_ITEM)
				continue;

			//Получаем стрелку
			DiagramArrow *cur_arr = static_cast<DiagramArrow *>(items[j]);
			DiagramObject *target = static_cast<DiagramObject *>(cur_arr->targetItem);
			DiagramObject *source = static_cast<DiagramObject *>(cur_arr->sourceItem);

			//Опредеяем, связана ли эта стрелка с текущим объектом
			if(source->id != it->id && target->id != it->id)
				continue;

			//Если сообщение создания
			if(cur_arr->isCreationMessageArrow)
			{
				//Стрелка идет из текущего объекта
				if(source->id == it->id)
				{
					//Поднимаем другой объект
					target->setVerticalSpace(vertical_space_default);
				}

				//Удалям стрелку

				//Удаляем со сцены
				this->removeItem(items[j]);

				//Очищаем память
				delete items[j];

				//Удаляем из вектора
				items.erase(items.begin() + j);
			}
			else	//Если синхронное сообщение
			{
				//Удаляем со сцены
				this->removeItem(items[j]);

				//Очищаем память
				delete items[j];

				//Удаляем из вектора
				items.erase(items.begin() + j);
			}
			//Возвращаемся
			j = -1;
		}

		//Удаляем объект из вектора
		//(index не должен был измениться после удаления стрелок
		//т.к. сообщения к объекту привязать можно только после создания объекта)
		items.erase(items.begin() + i);

		//Удаляем со сцены
		this->removeItem(it);

		//Очищаем память
		delete it;

		//Возвращаемся
		i = -1;
	}

	//Обновить линии жизни
	update_lifelines();

	//Обновляем сцену
	QGraphicsScene::update();
}

//Перемещение цикла
void GraphicScene::move_loop(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//Перемещаем цикл следя за тем, чтобы тот не вышел за пределы рабочей области
	
	//Получаем цикл
	DiagramLoop *loop = static_cast<DiagramLoop *>(it);

	//Если уже переместили - то ничего не делать
	if(used.count(loop->id))
		return;

	//Указываем, что сместили элемент
	used.insert(loop->id);

	//Получаем позицию элемента на сцене
	qreal scene_object_pos_x = loop->scenePos().x();
	qreal scene_object_pos_y = loop->scenePos().y();
	loop->leftVertex.setX(scene_object_pos_x);
	loop->leftVertex.setY(scene_object_pos_y);
	loop->rightVertex.setX(scene_object_pos_x + loop->getWidth());
	loop->rightVertex.setY(scene_object_pos_y + loop->getHeight());

	//Получаем новые координаты с учетом смещения
	qreal old_scene_object_pos_x = loop->scenePos().x();
	qreal old_scene_object_pos_y = loop->scenePos().y();
	scene_object_pos_x += dx;
	scene_object_pos_y += dy;

	//Следим, чтобы элемент не вышел за пределы поля
	scene_object_pos_x = max(0, (int)scene_object_pos_x);
	scene_object_pos_x = min(scene_width - loop->getWidth(), (int)scene_object_pos_x);
	scene_object_pos_y = max(0, (int)scene_object_pos_y);
	scene_object_pos_y = min(scene_height - loop->getHeight(), (int)scene_object_pos_y);

	//Присваиваем новые координаты
	loop->setPos(scene_object_pos_x, scene_object_pos_y);

	//Реальное смещенеи
	int r_dx = scene_object_pos_x - old_scene_object_pos_x;
	int r_dy = scene_object_pos_y - old_scene_object_pos_y;

	//Смещаем
	loop->move_loop(r_dx, r_dy, used);

	/*
	bool hasParent = false; 
	//переопределяем наличие "родителя" у цикла
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *tmpIt = static_cast<DiagramItem *>(items[i]);
		if(tmpIt->item_type == EItemsType::LOOP_ITEM)
		{
			DiagramLoop *tmpLoop = static_cast<DiagramLoop *>(items[i]);
			if(tmpLoop != loop)
			{
				if(tmpLoop->leftVertex.x() <= loop->leftVertex.x() && tmpLoop->leftVertex.y() <= loop->leftVertex.y() && 
					tmpLoop->rightVertex.x() >= loop->rightVertex.x() && tmpLoop->rightVertex.y() >= loop->rightVertex.y())
				{
					loop->parentLoop = tmpLoop;
					hasParent = true;
				}
			}
		}
	}
	if(!hasParent)
	{
		loop->parentLoop = NULL;
	}

	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *tmpIt = static_cast<DiagramItem *>(items[i]);
		if(tmpIt->item_type == EItemsType::LOOP_ITEM)
		{
			DiagramLoop *tmpLoop = static_cast<DiagramLoop *>(items[i]);
			if(tmpLoop != loop)
			{
				if(tmpLoop->parentLoop == loop)
				{
					move_loop(dx, dy, tmpIt, used);
				}
			}
		}
	}*/
}

void GraphicScene::moveChildLoop(int dx, int dy, DiagramItem *it, set<int> &used)
{

}

//Отжата кнопка мыши
/*virtual*/ void GraphicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if(current_state == EStateType::SELECT_LOOP_RECTANGLE_AREA)
	{
		mouseReleaseEventSelectLoopRectangleLastVertex(event);
	}
	else
	{
		//Если отжата левая кнопка мыши
		if(event->button() == Qt::LeftButton)
		{
			//Указать, что событие перетаскивания закончилось
			is_drag = false;

			//Если движения небыло - определяем это как обычный клик на рабочей области
			if(!is_move && !is_ctrl)
			{
				//Развыделяем все
				unselectAll();

				//Пытаемся получить элемент под курсором
				QGraphicsItem *it = itemAt(event->scenePos());

				//Если под курсором есть элемент
				if(it != NULL)
				{
					//Получить родительский элемент
					while(it->parentItem() != NULL)
						it = it->parentItem();

					//Выделить
					it->setSelected(true);
				}
			}

			//Обновить линии жизни
			update_lifelines();
		}

		//Обнулить флаг передвижения
		is_move = false;


		//Обнулить флаг дублирование
		is_duplicate = false;

		//Обновляем сцену
		QGraphicsScene::update();
	}
}

//Обновить линии жизни
void GraphicScene::update_lifelines()
{
	int max_h = 0;		//Максимальная высота сообщения

	//Проходим по всем объектам сцены
	for(int i = 0; i < items.size(); i++)
	{
		//Определяем - синхронное сообщение или нет
		DiagramItem *cur_obj = static_cast<DiagramItem *>(items[i]);
		if(cur_obj->item_type == EItemsType::MESSAGE_ITEM)
		{
			//Находим максимальную высоту сообщения
			DiagramArrow *diag_arrow = static_cast<DiagramArrow *>(items[i]);
			int cur_h = diag_arrow->getVerticalSpace();
			if(cur_h > max_h)
				max_h = cur_h;
		}
		else if(cur_obj->item_type == EItemsType::OBJECT_ITEM)
		{
			//Если объект, то смотрим на его высоту
			DiagramObject *diag_obj = static_cast<DiagramObject *>(items[i]);
			int cur_h = diag_obj->getVerticalSpace() + diag_obj->getHeight();
			if(cur_h > max_h)
				max_h = cur_h;
		}
	}

	//Выравниваем линии жизни
	int const_h = life_line_len_default;	//Дополнительная длина линии жизни (чтобы линия жизни не заканчивалась прямо на стрелке)
	for(int i = 0; i < items.size(); i++)
	{
		//Определяем - объект или нет
		DiagramItem *cur_obj = static_cast<DiagramItem *>(items[i]);
		if(cur_obj->item_type == EItemsType::OBJECT_ITEM)
		{
			//Выравниваем если объект не остановлен
			DiagramObject *diag_obj = static_cast<DiagramObject *>(items[i]);
			if(diag_obj->isLifeLineStop())	
			{
				//Получаем последнюю стрелку, связанную с объектом
				int max_arrow_h = getMaxArrowHeight(diag_obj);
				int new_life_line_len = const_h;
				if(max_arrow_h != 0)
					new_life_line_len = max_arrow_h - diag_obj->getHeight() + const_h;
				diag_obj->setLifeLineLen(new_life_line_len);
			}
			else
				diag_obj->setLifeLineLen(max_h + const_h - diag_obj->getVerticalSpace() + vertical_space_default);
		}
	}

	//Переопределяем активности объектов
	update_activity();

	//Обновляем сцену
	QGraphicsScene::update();
}

//Перемещение мыши
/*virtual*/ void GraphicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if(current_state == EStateType::SELECT_LOOP_RECTANGLE_AREA)
	{
		mouseMoveEventSelectLoopRectangleArea(event);
	}
	else
	{
		//Если происходит событие перетаскивания
		if(is_drag)
		{
			//Если происходит событие дублирования выделенных элементов - продублировать
			if(is_shift && !is_duplicate)
			{
				is_duplicate = true;
				duplicateSelection();
			}

			//Создаем множество использованных элементов
			set<int> used;

			//Вычисляем смещение, как объекты должны быть сдвинуты

			//Новые позиции мыши
			qreal new_mouse_drag_pos_x = event->scenePos().x();
			qreal new_mouse_drag_pos_y = event->scenePos().y();

			//Вычисляем смещение
			int dx = new_mouse_drag_pos_x - mouse_drag_pos_x;
			int dy = new_mouse_drag_pos_y - mouse_drag_pos_y;

			//Обновляем позицию мыши
			mouse_drag_pos_x = new_mouse_drag_pos_x;
			mouse_drag_pos_y = new_mouse_drag_pos_y;

			//Если движения произошло - указываем
			if(dx != 0 || dy != 0)
				is_move = true;

			//Получаем список элементов, которые нужно сместить
			vector<QGraphicsItem *> items_for_drag;
			for(int i = 0; i < items.size(); i++)
				if(items[i]->isSelected())
					items_for_drag.push_back(items[i]);

			//Сортируем их по высоте - vert_space
			for(int i = 0; i < items_for_drag.size(); i++)
				for(int j = i + 1; j < items_for_drag.size(); j++)
					if(((DiagramItem *)items_for_drag[i])->getVerticalSpace() > ((DiagramItem *)items_for_drag[j])->getVerticalSpace())
						swap(items_for_drag[i], items_for_drag[j]);

			//Проходим по всем выделенным элементам и двигаем их
			for(int i = 0; i < items_for_drag.size(); i++)
			{
				DiagramItem *it = static_cast<DiagramItem *>(items_for_drag[i]);
				if(it->item_type == EItemsType::OBJECT_ITEM)
					move_object(dx, dy, it, used);
				else if(it->item_type == EItemsType::COMMENT_ITEM)
					move_comment(dx, dy, it, used);
				else if(it->item_type == EItemsType::MESSAGE_ITEM && !((DiagramArrow *)items_for_drag[i])->isCreationMessageArrow)
					move_message(dx, dy, it, used);
				else if(it->item_type == EItemsType::MESSAGE_ITEM && ((DiagramArrow *)items_for_drag[i])->isCreationMessageArrow)
					move_creation_message(dx, dy, it, used);
				else if(it->item_type == EItemsType::LOOP_ITEM)
					move_loop(dx, dy, it, used);
			}
		}

		//Обновить линии жизни
		update_lifelines();

		//Обновляем сцену
		QGraphicsScene::update();
	}
}

//Масштабирование сцены
void GraphicScene::scaleScene(int p)
{
	//Пытаемся получить представление сцены и если не удается - выходим
	QList<QGraphicsView *> list = QGraphicsScene::views();
	if(list.size() == 0)
		return;
	QGraphicsView *v = list[0];

	//Масштабирование (в коэффициентах - переводим из процентов)
	v->resetMatrix();
	v->scale(qreal(p) / 100.0, qreal(p) / 100);
}


//Реакция на нажатие кнопки мыши в обычном состоянии
void GraphicScene::mousePressEventDefault(QGraphicsSceneMouseEvent *event)
{
	//Если нажата левая кнопка мыши
	if(event->button() == Qt::LeftButton)
	{
		//Пытаемся получить элемент под курсором
		QGraphicsItem *it = itemAt(event->scenePos());

		//Если под курсором есть элемент
		if(it != NULL)
		{
			//Получить родительский элемент
			while(it->parentItem() != NULL)
				it = it->parentItem();

			//Если не зажата клавиша ctrl - выделить текущий и снять выделение с остальных
			if(!is_ctrl)
			{
				if(!it->isSelected())
					unselectAll();
				it->setSelected(true);
			}
			else	//Иначе инвертировать выделение
				it->setSelected(!it->isSelected());

			//Указываем, что началось событие перетаскивания
			is_drag = true;

			//Запоминаем координаты начала перетаскивания
			mouse_drag_pos_x = event->scenePos().x();
			mouse_drag_pos_y = event->scenePos().y();

			//Если это цикл - запомнить для него все вложенные циклы
			DiagramItem *cur_it = static_cast<DiagramItem *>(it);
			if(cur_it->item_type == EItemsType::LOOP_ITEM)
			{
				DiagramLoop *loop = static_cast<DiagramLoop *>(it);
				//Получаем все вложенные циклы
				loop->stacked_loops = loop->getLoopInVector(items);
			}
		}
		else
		{
			//Если клавиша ctrl не зажата - снять выделения со всех объектов
			if(!is_ctrl)
				unselectAll();
		}
	}
}

//Реакция на нажатие кнопки мыши в состоянии добавления объекта
void GraphicScene::mousePressEventSelectPlaceForObject(QGraphicsSceneMouseEvent *event)
{
	//Если нажата левая кнопка мыши
	if(event->button() == Qt::LeftButton)
	{
		//Определяем координату на оси X - куда будет помещен объект
		qreal x_pos = event->scenePos().x();

		//Создаем новый объект
		DiagramObject *new_obj = new DiagramObject();

		//Задаем ему координату
		new_obj->setPosX(x_pos);

		//Добавляем на форму
		addItem(new_obj);
	}

	//Переходим в состояние по умолчанию
	setNewState(EStateType::DEFAULT_STATE);
}

//Реакция на нажатие кнопки мыши в состоянии добавления комментария
void GraphicScene::mousePressEventSelectPlaceForComment(QGraphicsSceneMouseEvent *event)
{
	//Если нажата левая кнопка мыши
	if(event->button() == Qt::LeftButton)
	{
		//Определяем координаты - куда будет помещен объект
		qreal x_pos = event->scenePos().x();
		qreal y_pos = event->scenePos().y();

		//Создаем новый объект
		DiagramComment *new_obj = new DiagramComment();

		//Задаем ему координаты
		new_obj->setPos(x_pos, y_pos);
		new_obj->setVerticalSpace(y_pos);

		//Добавляем на форму
		addItem(new_obj);
	}

	//Переходим в состояние по умолчанию
	setNewState(EStateType::DEFAULT_STATE);
}

//Реакция на нажатие кнопки мыши в состоянии добавления начала синх. сообщения
void GraphicScene::mousePressEventSelectMessageBegin(QGraphicsSceneMouseEvent *event)
{
	//Если нажата левая кнопка мыши
	if(event->button() == Qt::LeftButton)
	{
		//Убираем выделение со всех объектов
		unselectAll();

		//Получаем элемент под курсором
		QGraphicsItem *it = itemAt(event->scenePos());

		//Поиск самого верхнего элемента (родителя)
		if(it != NULL)
		{
			while(it->parentItem() != NULL)
				it = it->parentItem();
			it->setSelected(true);
		}

		//Если элемент есть и он является объектом
		if(it != NULL && ((DiagramItem *)it)->item_type == EItemsType::OBJECT_ITEM)
		{
			//Выделяем
			it->setSelected(true);

			//Создаем новую стрелку
			new_arrow = new DiagramArrow();

			//Указываем, откуда идет стрелка
			new_arrow->sourceItem = it;

			//Указываем изначальную высоту стрелки по первому клику
			DiagramObject *source_obj = static_cast<DiagramObject *>(it);
			int min_height = source_obj->getVerticalSpace() + source_obj->getHeight();
			int mouse_pos_y = event->scenePos().y() - new_arrow->getHeight() / 2;
			int init_height = max(min_height, mouse_pos_y);
			new_arrow->setVerticalSpace(init_height);

			//Меняем текущее состояние
			setNewState(EStateType::SELECT_MESSAGE_END_STATE);
		}
		else
			setNewState(EStateType::DEFAULT_STATE);	//Иначе переходим в состояние по умолчанию
	}
	else
		setNewState(EStateType::DEFAULT_STATE);	//Иначе переходим в состояние по умолчанию
}

//Реакция на нажатие кнопки мыши в состоянии добавления конца синх. сообщения
void GraphicScene::mousePressEventSelectMessageEnd(QGraphicsSceneMouseEvent *event)
{
	//Если нажата левая кнопка мыши
	if(event->button() == Qt::LeftButton)
	{
		//Убираем выделение со всех объектов
		unselectAll();

		//Получаем элемент под курсором
		QGraphicsItem *it = itemAt(event->scenePos());

		//Поиск самого верхнего элемента (родителя)
		if(it != NULL)
		{
			while(it->parentItem() != NULL)
				it = it->parentItem();
			it->setSelected(true);
		}

		//Если элемент есть и он является объектом и стрелка не идет в тот же объект из которого вышла
		if(it != NULL && ((DiagramItem *)it)->item_type == EItemsType::OBJECT_ITEM
			&& ((DiagramItem *)it)->id != ((DiagramItem *)new_arrow->sourceItem)->id)
		{
			//Выделяем
			it->setSelected(true);

			//Указываем куда идет стрелка
			new_arrow->targetItem = it;

			//Добавляем стрелку на сцену
			this->addItem(new_arrow);

			//Обновить значения стрелки
			new_arrow->refreshValues();
		}
		else
			delete new_arrow;		//Удаляем созданный объект синхронного сообщения
	}
	else
		delete new_arrow;			//Удаляем созданный объект синхронного сообщения

	//Ставим состояние по умолчанию
	setNewState(DEFAULT_STATE);
}

//Реакция на нажатие кнопки мыши в состоянии добавления начала сообщения создания
void GraphicScene::mousePressEventSelectCreationMessageBegin(QGraphicsSceneMouseEvent *event)
{
	//Если нажата левая кнопка мыши
	if(event->button() == Qt::LeftButton)
	{
		//Убираем выделение со всех объектов
		unselectAll();

		//Получаем элемент под курсором
		QGraphicsItem *it = itemAt(event->scenePos());

		//Поиск самого верхнего элемента (родителя)
		if(it != NULL)
		{
			while(it->parentItem() != NULL)
				it = it->parentItem();
			it->setSelected(true);
		}

		//Если элемент есть и он является объектом
		if(it != NULL && ((DiagramItem *)it)->item_type == EItemsType::OBJECT_ITEM)
		{
			//Выделяем
			it->setSelected(true);

			//Создаем новую стрелку
			new_arrow = new DiagramArrow();
			new_arrow->isCreationMessageArrow = true;

			//Указываем, откуда идет стрелка
			new_arrow->sourceItem = it;

			//Указываем изначальную высоту стрелки по первому клику
			DiagramObject *source_obj = static_cast<DiagramObject *>(it);
			int min_height = source_obj->getVerticalSpace() + source_obj->getHeight();
			int mouse_pos_y = event->scenePos().y() - new_arrow->getHeight() / 2;
			int init_height = max(min_height, mouse_pos_y);
			new_arrow->setVerticalSpace(init_height);

			//Меняем текущее состояние
			setNewState(EStateType::SELECT_CREATION_MESSAGE_END_STATE);
		}
		else
			setNewState(EStateType::DEFAULT_STATE);	//Иначе переходим в состояние по умолчанию
	}
	else
		setNewState(EStateType::DEFAULT_STATE);	//Иначе переходим в состояние по умолчанию
}

//Реакция на нажатие кнопки мыши в состоянии добавления конца сообщения создания
void GraphicScene::mousePressEventSelectCreationMessageEnd(QGraphicsSceneMouseEvent *event)
{
	//Ставим состояние по умолчанию
	setNewState(DEFAULT_STATE);

	//Если нажата не левая кнопка мыши
	if(event->button() != Qt::LeftButton)
	{
		delete new_arrow;			//Удаляем созданный объект синхронного сообщения
		return;
	}

	//Убираем выделение со всех объектов
	unselectAll();

	//Получаем элемент под курсором
	QGraphicsItem *it = itemAt(event->scenePos());

	//Поиск самого верхнего элемента (родителя)
	if(it != NULL)
	{
		while(it->parentItem() != NULL)
			it = it->parentItem();
		it->setSelected(true);
	}

	//Если элемент есть и он является объектом и стрелка не идет в тот же объект из которого вышла и такая стрелка возможна
	if(it != NULL && 
		((DiagramItem *)it)->item_type == EItemsType::OBJECT_ITEM && 
		((DiagramItem *)it)->id != ((DiagramItem *)new_arrow->sourceItem)->id &&
		isCreationMessageCanBeCreated((DiagramObject *)new_arrow->sourceItem, (DiagramObject *)it))
	{
		//Определяем минимальное возможное расстояние для текущей стрелки
		int possible_height = dfs_get_possible_height((DiagramObject *)it, items);

		//Если текущая стрелка будет создаваться слишком низко (смещения связанных объектов будет выходить за пределы рабочей области)
		if(new_arrow->getVerticalSpace() + new_arrow->getHeight() / 2 > scene_height - possible_height)
		{
			QMessageBox::warning(NULL, "Ошибка", "Нельзя создать сообщение создания, так как смещение, после связывания объектов, будет выходить за пределы рабочей области");
			delete new_arrow;
			return;
		}

		//Выделяем
		it->setSelected(true);

		//Указываем куда идет стрелка
		new_arrow->targetItem = it;

		//Определяем, на какой высоте находился объект
		int old_height = ((DiagramObject *)new_arrow->targetItem)->getVerticalSpace() + ((DiagramObject *)new_arrow->targetItem)->getHeight() / 2;

		//Определяем новую высоту
		int new_height = new_arrow->getVerticalSpace() + new_arrow->getHeight();

		//Определяем смещение
		int dy = new_height - old_height;

		//Добавляем стрелку на сцену
		this->addItem(new_arrow);

		//Обновить значения стрелки
		new_arrow->refreshValues();

		//Переместить все связные элементы
		move_items((DiagramObject *)it, items, dy);
	}
	else
		delete new_arrow;		//Удаляем созданный объект синхронного сообщения
}

//Реакция на нажатие кнопки мыши в состоянии выбора первой вершины прямоугольника цикла
void GraphicScene::mousePressEventSelectLoopRectangleFirstVertex(QGraphicsSceneMouseEvent *event)
{
	new_loop = new DiagramLoop();
	new_loop->leftVertex = event->scenePos();
	qreal posX = event->scenePos().x();
	qreal posY = event->scenePos().y();
	new_loop->setPos(posX, posY);
	new_loop->beginVertex = new_loop->leftVertex;
	new_loop->setWidth(0);
	new_loop->setHeight(0);
	this->addItem(new_loop);
	setNewState(SELECT_LOOP_RECTANGLE_AREA);

	unselectAll();
}

//Реакция на передвижение мыши в состоянии выделении прямоугольной области для цикла
void GraphicScene::mouseMoveEventSelectLoopRectangleArea(QGraphicsSceneMouseEvent *event)
{
	if(current_state == SELECT_LOOP_RECTANGLE_AREA)
	{
		new_loop->rightVertex.setX(event->scenePos().x());
		new_loop->rightVertex.setY(event->scenePos().y());
		if(new_loop->beginVertex.x() > event->scenePos().x())
		{
			new_loop->leftVertex.setX(event->scenePos().x());
			new_loop->rightVertex.setX(new_loop->beginVertex.x());
		}
		if(new_loop->beginVertex.y() > event->scenePos().y())
		{
			new_loop->leftVertex.setY(event->scenePos().y());
			new_loop->rightVertex.setY(new_loop->beginVertex.y());
		}
		new_loop->setPos(new_loop->leftVertex.x(), new_loop->leftVertex.y());
		new_loop->setWidth(abs(event->scenePos().x() - new_loop->beginVertex.x()));
		new_loop->setHeight(abs(event->scenePos().y() - new_loop->beginVertex.y()));
		QGraphicsScene::update();
	}
}

//Реакция на нажатие кнопки мыши в состоянии выбора последней вершины прямоугольника цикла
void GraphicScene::mouseReleaseEventSelectLoopRectangleLastVertex(QGraphicsSceneMouseEvent *event)
{
	if(current_state == EStateType::SELECT_LOOP_RECTANGLE_AREA)
	{
		if(new_loop->beginVertex.x() > event->scenePos().x())
		{
			new_loop->leftVertex.setX(event->scenePos().x());
			new_loop->rightVertex.setX(new_loop->beginVertex.x());
		}
		if(new_loop->beginVertex.y() > event->scenePos().y())
		{
			new_loop->leftVertex.setY(event->scenePos().y());
			new_loop->rightVertex.setY(new_loop->beginVertex.y());
		}
		new_loop->setWidth(abs(event->scenePos().x() - new_loop->beginVertex.x()));
		new_loop->setHeight(abs(event->scenePos().y() - new_loop->beginVertex.y()));
		new_loop->setPos(new_loop->leftVertex.x(), new_loop->leftVertex.y());
		if(new_loop->getWidth() < 150 || new_loop->getHeight() < 100)
		{
			QMessageBox::warning(NULL, "Ошибка", "Слишком маленький размер области для цикла");
			int index = 0;
			for(int i = 0; i < items.size(); i++)
				if(items[i] == new_loop)
					index = i;
			items.erase(items.begin() + index);
			this->removeItem(new_loop);
			delete(new_loop);
		}
		else
		{
			new_loop->sizeDetermined = true;
			//определяем наличие "родителя" у цикла и смотрим, является ли цикл родителем
			for(int i = 0; i < items.size(); i++)
			{
				DiagramItem *it = static_cast<DiagramItem *>(items[i]);
				if(it->item_type == EItemsType::LOOP_ITEM)
				{
					DiagramLoop *loop = static_cast<DiagramLoop *>(items[i]);
					// если у цикла есть родитель
					if(loop->leftVertex.x() <= new_loop->leftVertex.x() && loop->leftVertex.y() <= new_loop->leftVertex.y() && 
						loop->rightVertex.x() > new_loop->rightVertex.x() && loop->rightVertex.y() > new_loop->rightVertex.y())
					{
						new_loop->parentLoop = loop;
					}
					// если цикл является родителем
					if(loop->leftVertex.x() >= new_loop->leftVertex.x() && loop->leftVertex.y() >= new_loop->leftVertex.y() && 
						loop->rightVertex.x() < new_loop->rightVertex.x() && loop->rightVertex.y() < new_loop->rightVertex.y())
					{
						loop->parentLoop = new_loop;
					}
				}
			}
		}
		QGraphicsScene::update();
		setNewState(DEFAULT_STATE); 
	}
	else
	{
		setNewState(DEFAULT_STATE); 
		delete(new_loop);
	}
}

//Убрать выделение со всех объектов
void GraphicScene::unselectAll()
{
	for(int i = 0; i < items.size(); i++)
		items[i]->setSelected(false);
}

//Обновить активности
void GraphicScene::update_activity()
{
	//Перебор объектов
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *cur_it = static_cast<DiagramItem *>(items[i]);
		if(cur_it->item_type == EItemsType::OBJECT_ITEM)
		{
			//Поиск всех стрелок, связанных с объектом и занесение их в список
			vector<DiagramArrow *> cur_arr;
			for(int j = 0; j < items.size(); j++)
			{
				DiagramItem *exp_arr = static_cast<DiagramItem *>(items[j]);
				if(exp_arr->item_type == EItemsType::MESSAGE_ITEM)
				{
					DiagramArrow *arr = static_cast<DiagramArrow *>(items[j]);
					DiagramItem *source = static_cast<DiagramItem *>(arr->sourceItem);
					DiagramItem *target = static_cast<DiagramItem *>(arr->targetItem);
					//Определяем - связана ли стрелка с текущим объектом и если да, то добавляем в список
					if(source->id == cur_it->id || target->id == cur_it->id)
						cur_arr.push_back(arr);
				}
			}

			//Сортируем список стрелок для текущего объекта по высоте
			for(int j = 0; j < cur_arr.size(); j++)
				for(int k = j + 1; k < cur_arr.size(); k++)
					if(cur_arr[j]->getVerticalSpace() > cur_arr[k]->getVerticalSpace())
						swap(cur_arr[j], cur_arr[k]);

			//Строим отрезки активностей
			vector<pair<int, int> > activity;									//Отрезки активностей
			DiagramObject *cur_obj = static_cast<DiagramObject *>(items[i]);	//Объект текущий
			int balance = 0;													//Баланс
			int cur_start = -1;													//Начало отрезка (для поддержания)
			for(int i = 0; i < cur_arr.size(); i++)
			{
				//Определяем объекты источник и цель для стрелки
				DiagramItem *source = static_cast<DiagramItem *>(cur_arr[i]->sourceItem);
				DiagramItem *target = static_cast<DiagramItem *>(cur_arr[i]->targetItem);

				//Если есть сообщение создания, которое входит в текущий объект - не учитывать эту стрелку
				if(cur_arr[i]->isCreationMessageArrow && cur_obj->id == target->id)
					continue;

				//Если сообщение создания выходит из объекта
				if(cur_arr[i]->isCreationMessageArrow && source->id == cur_obj->id)
				{
					int s = cur_arr[i]->getVerticalSpace() - cur_obj->getVerticalSpace();
					int f = s + 10;
					activity.push_back(make_pair(s, f));
					continue;
				}

				//Если стрелка выходит из объекта
				if(source->id == cur_obj->id)
				{
					balance--;

					//Если активность объекта еще не закончилась или закончилась только что
					if(balance >= 0)
					{
						//Если есть начало активности
						if(cur_start != -1)
						{
							int s = cur_arr[cur_start]->getVerticalSpace() - cur_obj->getVerticalSpace();
							int f = cur_arr[i]->getVerticalSpace() - cur_obj->getVerticalSpace();
							activity.push_back(make_pair(s, f));
							if(balance == 0)
								cur_start = -1;
						}
					}
					else	//Если активности нет и небыло
					{
						int s = cur_arr[i]->getVerticalSpace() - cur_obj->getVerticalSpace();
						int f = s + 10;
						activity.push_back(make_pair(s, f));
					}
				}
				else	//Если входит
				{
					balance++;

					//Если стрелка означает начало активации - запомнить индекс начала
					if(balance == 1)
						cur_start = i;

					//Всегда ставим обычный отрезок активности
					int s = cur_arr[i]->getVerticalSpace() - cur_obj->getVerticalSpace();
					int f = s + 10;
					activity.push_back(make_pair(s, f));
				}
			}

			//Объединяем отрезки с пересечениями
			for(int i = 1; i < activity.size(); i++)
			{
				int left1 = activity[i - 1].first;
				int right1 = activity[i - 1].second;
				int left2 = activity[i].first;
				int right2 = activity[i].second;

				//Если есть пересечение
				if(min(right1, right2) >= max(left1, left2))
				{
					activity[i - 1].first = min(left1, left2);
					activity[i - 1].second = max(right1, right2);
					activity.erase(activity.begin() + i);
					i--;
				}
			}

			//Ставим активность
			cur_obj->setActivity(activity);
		}
	}
}

//Может ли стрелка создания быть создана
bool GraphicScene::isCreationMessageCanBeCreated(DiagramObject *from, DiagramObject *to)
{
	//Кол-во входящих стрелок создания должно быть не более 1
	
	//Поиск всех стрелок, входящих в объект to
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *cur_it = static_cast<DiagramItem *>(items[i]);

		//Отбрасываем все стрелки
		if(cur_it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		DiagramArrow *cur_arr = static_cast<DiagramArrow *>(items[i]);
		DiagramObject *source = static_cast<DiagramObject *>(cur_arr->sourceItem);
		DiagramObject *target = static_cast<DiagramObject *>(cur_arr->targetItem);

		//Если стрелка не является сообщением создания - пропустить
		if(!cur_arr->isCreationMessageArrow)
			continue;

		//Если стрелка не является входящей - пропустить
		if(target->id != to->id)
			continue;

		return false;

	}

	//Стрелка не должна попасть в любого из родителей (а точнее в корень дерева)

	//Поиск корня
	DiagramObject *root = from;		//Корень дерева
	bool isEnd = true;				//Критерий остановки поиска
	while(isEnd)
	{
		isEnd = false;

		//Поиск сообщения создания, входящего в текущий объект
		for(int i = 0; i < items.size(); i++)
		{
			//Отбрасываем все элементы - не стрелки
			DiagramItem *cur_it = static_cast<DiagramItem *>(items[i]);
			if(cur_it->item_type != EItemsType::MESSAGE_ITEM)
				continue;

			DiagramArrow *cur_arr = static_cast<DiagramArrow *>(items[i]);
			DiagramObject *source = static_cast<DiagramObject *>(cur_arr->sourceItem);
			DiagramObject *target = static_cast<DiagramObject *>(cur_arr->targetItem);

			//Если стрелка не является сообщением создания - пропустить
			if(!cur_arr->isCreationMessageArrow)
				continue;

			//Если стрелка не входит в текущий корень - пропустить
			if(target->id != root->id)
				continue;

			//Поднимаемся выше по дереву
			root = source;
			isEnd = true;
			break;
		}
	}

	//Проверяем - идет ли новая стрелка в корень
	if(to->id == root->id)
		return false;
	
	return true;
}

//Изменить состояние выделенного объекта (остановлена линия жизни или нет)
void GraphicScene::changeStateOfLineEnd()
{
	//Получаем все выделенные объекты на данный момент
	for(int i = 0; i < items.size(); i++)
	{
		//Если элемент не выделен - пропустить
		if(!items[i]->isSelected())
			continue;

		//Если не объект - пропустить
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//Получаем объект
		DiagramObject *obj = static_cast<DiagramObject *>(it);

		//Получаем последнюю стрелку, связанную с объектом
		int max_arrow_h = getMaxArrowHeight(obj);

		//Изменяем состояние линии жизни объекта
		obj->changeStateLifeLineStop(max_arrow_h);
	}

	//Обновляем линии жизни
	update_lifelines();
}

//Получить высоту последней стрелки, связанной с объектом
int GraphicScene::getMaxArrowHeight(DiagramObject *obj)
{
	//Получаем последнюю стрелку, связанную с объектом
	int max_arrow_h = 0;
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *cur_it = static_cast<DiagramItem *>(items[i]);
		if(cur_it->item_type != EItemsType::MESSAGE_ITEM)
			continue;
		DiagramArrow *cur_arr = static_cast<DiagramArrow *>(items[i]);
		DiagramItem *source = static_cast<DiagramItem *>(cur_arr->sourceItem);
		DiagramItem *target = static_cast<DiagramItem *>(cur_arr->targetItem);
		if(cur_arr->isCreationMessageArrow && target->id == obj->id)
			continue;
		if(source->id != obj->id && target->id != obj->id)
			continue;
		if(max_arrow_h < cur_arr->getVerticalSpace() - obj->getVerticalSpace())
			max_arrow_h = cur_arr->getVerticalSpace() - obj->getVerticalSpace();
	}
	return max_arrow_h;
}

//Очистить диаграмму
void GraphicScene::newDiagram()
{
	//Удаляем все элементы
	for(int i = 0; i < items.size(); i++)
		delete items[i];
	items.clear();

	//Последний id по умолчанию
	last_id = -1;
	current_state = DEFAULT_STATE;
	diagram_filename = "";

	//Обновляем сцену
	update();
}

//Открыть диаграмму из файла
bool GraphicScene::openDiagramFromFile(QString filename)
{
	newDiagram();

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;

	QDataStream in(&file);

	in >> last_id;
	if(in.status() != QDataStream::Ok)
		return false;

	int size;
	in >> size;
	if(in.status() != QDataStream::Ok)
		return false;

	for(int i = 0; i < size; i++)
	{
		DiagramObject *it = new DiagramObject();
		EItemsType type = EItemsType::DEFAULT_ITEM;
		if(!it->loadItemType(in, type))
		{
			file.close();
			delete it;
			return false;
		}
		delete it;
		
		if(type == EItemsType::OBJECT_ITEM)
		{
			DiagramObject *new_obj = new DiagramObject();
			new_obj->item_type = type;
			new_obj->loadFromFile(in);

			DiagramItem *new_it = static_cast<DiagramItem *>(new_obj);
			QGraphicsScene::addItem(new_it);
			QGraphicsItem *new_graphitem = static_cast<QGraphicsItem *>(new_obj);
			items.push_back(new_graphitem);
		}
		else if(type == EItemsType::COMMENT_ITEM)
		{
			DiagramComment *new_comment = new DiagramComment();
			new_comment->item_type = type;
			new_comment->loadFromFile(in);

			DiagramItem *new_it = static_cast<DiagramItem *>(new_comment);
			QGraphicsScene::addItem(new_it);
			QGraphicsItem *new_graphitem = static_cast<QGraphicsItem *>(new_comment);
			items.push_back(new_graphitem);
		}
		else if(type == EItemsType::MESSAGE_ITEM)
		{
			DiagramArrow *new_arr = new DiagramArrow();
			new_arr->item_type = type;
			new_arr->loadFromFile(in, items);

			DiagramItem *new_it = static_cast<DiagramItem *>(new_arr);
			QGraphicsScene::addItem(new_it);
			QGraphicsItem *new_graphitem = static_cast<QGraphicsItem *>(new_arr);
			items.push_back(new_graphitem);
		}
		else if(type == EItemsType::LOOP_ITEM)
		{
			DiagramLoop *new_loop = new DiagramLoop();
			new_loop->item_type = type;
			new_loop->loadFromFile(in);

			DiagramItem *new_it = static_cast<DiagramItem *>(new_loop);
			QGraphicsScene::addItem(new_it);
			QGraphicsItem *new_graphitem = static_cast<QGraphicsItem *>(new_loop);
			items.push_back(new_graphitem);
		}
		else
		{
			file.close();
			return false;
		}
	}

	file.close();

	if(in.status() != QDataStream::Ok)
		return false;

	diagram_filename = filename;

	update_activity();

	return true;
}

//Сохранить диаграмму в файл
bool GraphicScene::saveDiagramToFile(QString filename)
{
	if(filename == "")
		filename = diagram_filename;

	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
		return false;

	QDataStream out(&file);

	out << last_id;
	if(out.status() != QDataStream::Ok)
		return false;

	out << items.size();
	if(out.status() != QDataStream::Ok)
		return false;

	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *cur_it = static_cast<DiagramItem *>(items[i]);
		if(!cur_it->saveItemType(out) || !cur_it->saveToFile(out))
		{
			file.close();
			return false;
		}
	}

	file.close();

	if(out.status() != QDataStream::Ok)
		return false;

	diagram_filename = filename;

	return true;
}

//Получить текущее имя файла
QString GraphicScene::getFilename() const
{
	return diagram_filename;
}

//Нажали кнопку клавиатуры
/*virtual*/ void GraphicScene::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Control)
		is_ctrl = true;
	if(event->key() == Qt::Key_Shift)
		is_shift = true;
}

//Отжали кнопку клавиатуры
/*virtual*/ void GraphicScene::keyReleaseEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Control)
		is_ctrl = false;
	if(event->key() == Qt::Key_Shift)
		is_shift = false;
}

//Перемещение объекта
void GraphicScene::move_object(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//От перемещения объекта не зависят другие перемещения
	//Так что достаточно просто переместить объект по смещению и следить чтобы тот не ушел за края рабочей области
	
	//Получаем объект
	DiagramObject *obj = static_cast<DiagramObject *>(it);

	//Если уже переместили - то ничего не делать
	if(used.count(obj->id))
		return;

	//Указываем, что сместили элемент
	used.insert(obj->id);

	//Получаем позицию объекта на сцене
	qreal scene_object_pos_x = obj->scenePos().x();
	qreal scene_object_pos_y = obj->scenePos().y();

	//Получаем новые координаты с учетом смещения (объект двигается только вдоль оси x)
	scene_object_pos_x += dx;

	//Следим, чтобы объект не вышел за пределы поля
	scene_object_pos_x = max(0, (int)scene_object_pos_x);
	scene_object_pos_x = min(scene_width - obj->getWidth(), (int)scene_object_pos_x);

	//Присваиваем новые координаты
	obj->setPos(scene_object_pos_x, scene_object_pos_y);
}

//Перемещение синхронного сообщения
void GraphicScene::move_message(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//От перемещения синхронного сообщения не зависят другие перемещения
	//Нужно просто перемещать вдоль оси Y и следить за тем, чтобы не выходило сверху за границы объекта и снизу не выходило за пределы рабочей области
	
	//Получаем стрелку
	DiagramArrow *arr = static_cast<DiagramArrow *>(it);

	//Если уже переместили - то ничего не делать
	if(used.count(arr->id))
		return;

	//Указываем, что сместили элемент
	used.insert(arr->id);

	//Получаем позицию элемента на сцене
	qreal scene_object_pos_x = arr->scenePos().x();
	qreal scene_object_pos_y = arr->scenePos().y();

	//Получаем новые координаты с учетом смещения (только по оси Y)
	scene_object_pos_y += dy;

	//Следим, чтобы элемент не вышел за пределы поля и за пределы объектов сверху
	DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
	DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);
	scene_object_pos_y = max(source->getVerticalSpace() + source->getHeight(), (int)scene_object_pos_y);
	scene_object_pos_y = max(target->getVerticalSpace() + target->getHeight(), (int)scene_object_pos_y);
	scene_object_pos_y = min(scene_height - arr->getHeight(), (int)scene_object_pos_y);

	//Присваиваем новые координаты
	arr->setVerticalSpace(scene_object_pos_y);

	//Обновляем линии жизни
	update_lifelines();
}

//Перемещение сообщения создания
void GraphicScene::move_creation_message(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//От перемещения сообщения создания меняется очень многое
	//Нужно разделить на 2 вида перемещения - вверх и вниз

	//Получаем сообщение
	DiagramArrow *arr = static_cast<DiagramArrow *>(it);

	//Если уже переместили - то ничего не делать
	if(used.count(arr->id))
		return;

	//Указываем, что сместили элемент
	used.insert(arr->id);

	//Получаем позицию элемента на сцене
	qreal scene_object_pos_x = arr->scenePos().x();
	qreal scene_object_pos_y = arr->scenePos().y();

	//Получаем новые координаты с учетом смещения (только по оси Y)
	scene_object_pos_y += dy;

	//Следим, чтобы элемент не вышел  за пределы объекта источника сверху
	DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
	DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);
	scene_object_pos_y = max(source->getVerticalSpace() + source->getHeight(), (int)scene_object_pos_y);

	//Следим, чтобы элемент не превышал допустимую высоту снизу
	int possible_height = scene_height - 2 * arr->getHeight() - dfs_get_possible_height(target, items);
	scene_object_pos_y = min(possible_height, (int)scene_object_pos_y);

	//Присваиваем новые координаты
	arr->setVerticalSpace(scene_object_pos_y);

	//Выравниваем target объект по высоте стрелки
	target->setVerticalSpace(scene_object_pos_y + arr->getHeight() / 2 - target->getHeight() / 2);

	//Если перемещение идет вверх - другие объекты не затрагиваются и происходит просто перемещение стрелки

	//Если перемещение идет вниз
	if(dy > 0)
	{
		//Перемещаем элементы, которые еще не использовались
		dfs_move_items(target, items, dy, used);
	}

	//Обновляем линии жизни
	update_lifelines();
}

//Перемещение свободного комментария
void GraphicScene::move_comment(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//От перемещения свободного комментария не зависят другие перемещения
	//Перемещаем комментарий следя за тем, чтобы тот не вышел за пределы рабочей области
	
	//Получаем комментарий
	DiagramComment *comment = static_cast<DiagramComment *>(it);

	//Если уже переместили - то ничего не делать
	if(used.count(comment->id))
		return;

	//Указываем, что сместили элемент
	used.insert(comment->id);

	//Получаем позицию элемента на сцене
	qreal scene_object_pos_x = comment->scenePos().x();
	qreal scene_object_pos_y = comment->scenePos().y();

	//Получаем новые координаты с учетом смещения
	scene_object_pos_x += dx;
	scene_object_pos_y += dy;

	//Следим, чтобы элемент не вышел за пределы поля
	scene_object_pos_x = max(0, (int)scene_object_pos_x);
	scene_object_pos_x = min(scene_width - comment->getWidth(), (int)scene_object_pos_x);
	scene_object_pos_y = max(0, (int)scene_object_pos_y);
	scene_object_pos_y = min(scene_height - comment->getHeight(), (int)scene_object_pos_y);

	//Присваиваем новые координаты
	comment->setPos(scene_object_pos_x, scene_object_pos_y);
	comment->setVerticalSpace(scene_object_pos_y);
}

//Двойной клик мыши
/*virtual*/ void GraphicScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	//Если нажата кнопка Ctrl - то происходит выбор элементов и двойной клик обрабатывать не нужно
	if(is_ctrl)
		return;

	//Если под курсором нет элемента - ничего не делать
	QGraphicsItem *q_it = itemAt(event->scenePos());
	if(q_it == NULL)
		return;

	//Получаем данные
	DiagramItem *it = static_cast<DiagramItem *>(q_it);
	QString text_item_string = it->getTextItemString();

	//Создаем окно и показываем
	DialogWindow *new_window = new DialogWindow(text_item_string);
	if(new_window->exec() == QDialog::Accepted)
	{
		//Получаем новый комментарий для объекта
		QString s = new_window->getTextComment();

		//Устанавливаем текстовый комментарий
		if(s != text_item_string)
			it->setTextComment(s);
	}

	//Удаляем окно
	delete new_window;
}

//Сделать дубликат выделенных элементов
void GraphicScene::duplicateSelection()
{
	//Нужно скопировать все выделенные элементы правильно, поменять нужные значения, развыделить старые и выделить новые

	//Создаем структуру для хранения данных о заменах идентификаторов
	map<int, int> new_id;

	//Копируем циклы
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем не выделенные элементы
		if(!items[i]->isSelected())
			continue;

		//Пропускаем элементы не являющиеся циклами
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::LOOP_ITEM)
			continue;

		//Остались выделенные циклы

		//Копируем
		DiagramLoop *loop = static_cast<DiagramLoop *>(items[i]);
		DiagramItem *duplicate_it = loop->getCopy();

		//Говорим о замене id
		new_id[duplicate_it->id] = last_id + 1;

		//Добавляем элемент
		QGraphicsItem *q_it = static_cast<QGraphicsItem *>(duplicate_it);
		addItem(q_it);
	}

	//Копируем комментарии
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем не выделенные элементы
		if(!items[i]->isSelected())
			continue;

		//Пропускаем элементы не являющиеся коментарием
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::COMMENT_ITEM)
			continue;

		//Остались выделенные комментарии

		//Копируем
		DiagramComment *comment = static_cast<DiagramComment *>(items[i]);
		DiagramItem *duplicate_it = comment->getCopy();

		//Говорим о замене id
		new_id[duplicate_it->id] = last_id + 1;

		//Добавляем элемент
		QGraphicsItem *q_it = static_cast<QGraphicsItem *>(duplicate_it);
		addItem(q_it);
	}

	//Копируем объекты
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем не выделенные элементы
		if(!items[i]->isSelected())
			continue;

		//Пропускаем элементы не являющиеся объектами
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//Остались выделенные объекты

		//Копируем
		DiagramObject *obj = static_cast<DiagramObject *>(items[i]);
		DiagramItem *duplicate_it = obj->getCopy();

		//Говорим о замене id
		new_id[duplicate_it->id] = last_id + 1;

		//Добавляем элемент
		QGraphicsItem *q_it = static_cast<QGraphicsItem *>(duplicate_it);
		addItem(q_it);
	}

	//Копируем стрелки
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем не выделенные элементы
		if(!items[i]->isSelected())
			continue;

		//Пропускаем элементы не являющиеся стрелками
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		//Остались выделенные объекты

		//Копируем
		DiagramArrow *arr = static_cast<DiagramArrow *>(items[i]);
		DiagramItem *duplicate_it = arr->getCopy();
		DiagramArrow *duplicate_arr = static_cast<DiagramArrow *>(duplicate_it);

		//Получаем данные о связи
		DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
		DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);

		//Если сообщение создания
		if(arr->isCreationMessageArrow)
		{
			//Если target не дублируется
			if(!new_id.count(target->id))
			{
				//Не дублировать и пропустить
				delete duplicate_it;
				continue;
			}
		}
		
		//Обновляем информацию о source
		if(new_id.count(source->id))
		{
			//Поиск нового источника
			for(int j = 0; j < items.size(); j++)
			{
				DiagramItem *new_source = static_cast<DiagramItem *>(items[j]);
				QGraphicsItem *q_new_source = static_cast<QGraphicsItem *>(items[j]);
				
				//Если нашли - указываем и завершаем
				if(new_source->id == new_id[source->id])
				{
					duplicate_arr->sourceItem = q_new_source;
					break;
				}
			}
		}

		//Обновляем информацию о target
		if(new_id.count(target->id))
		{
			//Поиск нового источника
			for(int j = 0; j < items.size(); j++)
			{
				DiagramItem *new_target = static_cast<DiagramItem *>(items[j]);
				QGraphicsItem *q_new_target = static_cast<QGraphicsItem *>(items[j]);
				
				//Если нашли - указываем и завершаем
				if(new_target->id == new_id[target->id])
				{
					duplicate_arr->targetItem = q_new_target;
					break;
				}
			}
		}

		//Говорим о замене id
		new_id[duplicate_it->id] = last_id + 1;

		//Добавляем элемент
		QGraphicsItem *q_it = static_cast<QGraphicsItem *>(duplicate_it);
		addItem(q_it);
	}

	//Развыделяем старые элементы и выделяем новые
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		
		//Если нашли элемент, который был продублирован
		if(new_id.count(it->id))
		{
			//Развыделить его
			items[i]->setSelected(false);

			//Поиск его копии и выделение ее
			for(int j = 0; j < items.size(); j++)
			{
				DiagramItem *copy_it = static_cast<DiagramItem *>(items[j]);
				if(copy_it->id == new_id[it->id])
				{
					items[j]->setSelected(true);

					//Если продублировали объект - нужно проверить - на правильной ли он высоте
					if(copy_it->item_type == EItemsType::OBJECT_ITEM)
					{
						bool isFind = false;
						for(int k = 0; k < items.size(); k++)
						{
							//Производим поиск сообщений создания, которые идут в текущий объект
							DiagramItem *check_it = static_cast<DiagramItem *>(items[k]);

							if(check_it->item_type != EItemsType::MESSAGE_ITEM)
								continue;

							DiagramArrow *check_arr = static_cast<DiagramArrow *>(items[k]);
							if(!check_arr->isCreationMessageArrow)
								continue;

							//Если нашли такую стрелку - то отметим это
							DiagramObject *check_target = static_cast<DiagramObject *>(check_arr->targetItem);
							if(check_target->id == copy_it->id)
								isFind = true;
						}
						if(!isFind)
							copy_it->setVerticalSpace(vertical_space_default);
					}

					break;
				}
			}
		}
	}
}

//Вырезать выделенные элементы в буфер обмена
void GraphicScene::cutSelection()
{
	copySelection();
	removeSelectedItem();
}

//Скопировать выделенные элементы в буфер обмена
void GraphicScene::copySelection()
{
	//Создаем массив байтов
	QByteArray buf("");

	//Скопировать все циклы
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем все не выделенные
		if(!items[i]->isSelected())
			continue;
		
		//Пропускаем все не циклы
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::LOOP_ITEM)
			continue;

		//Остались выделенные циклы

		//Буферизируем
		DiagramLoop *loop = static_cast<DiagramLoop *>(items[i]);
		buf.append(loop->getInfoForBuffer() + "\t");
	}

	//Скопировать все комментарии
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем все не выделенные
		if(!items[i]->isSelected())
			continue;
		
		//Пропускаем все не комментарии
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::COMMENT_ITEM)
			continue;

		//Остались выделенные комментарии

		//Буферизируем
		DiagramComment *comment = static_cast<DiagramComment *>(items[i]);
		buf.append(comment->getInfoForBuffer() + "\t");
	}

	//Скопировать все объекты
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем все не выделенные
		if(!items[i]->isSelected())
			continue;

		//Пропускаем все не объекты
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//Остались выделенные объекты

		//Буферизируем
		DiagramObject *obj = static_cast<DiagramObject *>(items[i]);
		buf.append(obj->getInfoForBuffer() + "\t");
	}

	//Скопировать все стрелки, учитывая, что оба объекта, связанные со стрелкой должны быть скопированы тоже
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем все не выделенные
		if(!items[i]->isSelected())
			continue;

		//Пропускаем все не стрелки
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		//Остались выделенные стрелки

		//Проверяем - скопировались ли вместе со стрелкой связные объекты
		DiagramArrow *arr = static_cast<DiagramArrow *>(items[i]);
		DiagramItem *source = static_cast<DiagramItem *>(arr->sourceItem);
		DiagramItem *target = static_cast<DiagramItem *>(arr->targetItem);
		bool is_source = false;
		bool is_target = false;
		for(int j = 0; j < items.size(); j++)
		{
			//Пропускаем все не выделенные
			if(!items[j]->isSelected())
				continue;

			//Пропускаем все не объекты
			DiagramItem *check_it = static_cast<DiagramItem *>(items[j]);
			if(check_it->item_type != EItemsType::OBJECT_ITEM)
				continue;

			if(check_it->id == source->id)
				is_source = true;
			if(check_it->id == target->id)
				is_target = true;
		}
		if(!is_source || !is_target)
			continue;

		//Буферизуем
		buf.append(arr->getInfoForBuffer() + "\t");
	}

	//Заносим в буфер
	mimeData->setData("text/csv", buf);
	QApplication::clipboard()->setMimeData(mimeData);
}

//Вставить элементы из буфера обмена
void GraphicScene::pasteFromBuffer()
{
	//Развыделяем все элементы
	unselectAll();

	//Достаем из буфера данные
	const QMimeData *out = QApplication::clipboard()->mimeData();
	QByteArray buf = out->data("text/csv");

	//Разбиваем на токены
	QList<QByteArray> list = buf.split('\t');

	//Создаем структуру в которой будем хранить новые ид
	map<int, int> new_id;

	//Проходим по токенам и вставляем элементы
	for(int i = 0; i < list.length(); i++)
	{
		//Если нашли цикл
		if(list[i] == "<diagramloop_tag>")
		{
			//Определяем набор данных для него
			QList<QByteArray> l;
			l.append(list[i]);
			do
			{
				i++;
				l.append(list[i]);
			}
			while(list[i] != "</diagramloop_tag>");

			//Создаем новый элемент
			DiagramLoop *loop = new DiagramLoop();

			//Передаем ему данные
			loop->setInfoFromBuffer(l);

			//Запоминаем новый id
			new_id[loop->id] = last_id + 1;

			//Добавляем на форму
			QGraphicsItem *q_loop = static_cast<QGraphicsItem *>(loop);
			addItem(q_loop);

			//Выделяем
			q_loop->setSelected(true);
		}
		else if(list[i] == "<diagramcomment_tag>")	//Если нашли комментарий
		{
			//Определяем набор данных для него
			QList<QByteArray> l;
			l.append(list[i]);
			do
			{
				i++;
				l.append(list[i]);
			}
			while(list[i] != "</diagramcomment_tag>");

			//Создаем новый элемент
			DiagramComment *comment = new DiagramComment();

			//Передаем ему данные
			comment->setInfoFromBuffer(l);

			//Запоминаем новый id
			new_id[comment->id] = last_id + 1;

			//Добавляем на форму
			QGraphicsItem *q_comment = static_cast<QGraphicsItem *>(comment);
			addItem(q_comment);

			//Выделяем
			q_comment->setSelected(true);
		}
		else if(list[i] == "<diagramobject_tag>") //Если нашли объект
		{
			//Определяем набор данных для него
			QList<QByteArray> l;
			l.append(list[i]);
			do
			{
				i++;
				l.append(list[i]);
			}
			while(list[i] != "</diagramobject_tag>");

			//Создаем новый элемент
			DiagramObject *obj = new DiagramObject();

			//Передаем ему данные
			obj->setInfoFromBuffer(l);

			//Запоминаем новый id
			new_id[obj->id] = last_id + 1;

			//Добавляем на форму
			QGraphicsItem *q_obj = static_cast<QGraphicsItem *>(obj);
			addItem(q_obj);

			//Выделяем
			q_obj->setSelected(true);
		}
		else if(list[i] == "<diagramarrow_tag>") //Если нашли стрелку
		{
			//Определяем набор данных для него
			QList<QByteArray> l;
			l.append(list[i]);
			do
			{
				i++;
				l.append(list[i]);
			}
			while(list[i] != "</diagramarrow_tag>");

			//Создаем новый элемент
			DiagramArrow *arr = new DiagramArrow();

			//Передаем ему данные
			arr->setInfoFromBuffer(l, items, new_id);

			//Запоминаем новый id
			new_id[arr->id] = last_id + 1;

			//Добавляем на форму
			QGraphicsItem *q_arr = static_cast<QGraphicsItem *>(arr);
			addItem(q_arr);

			//Выделяем
			q_arr->setSelected(true);
		}
	}

	//Пройтись по всем выделенным объектам и если в объект не входит сообщение создания - поднять до уровня по умолчанию
	for(int i = 0; i < items.size(); i++)
	{
		//Пропустить не выделенные
		if(!items[i]->isSelected())
			continue;

		//Пропустить не объекты
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//Получаем id выделенного объекта
		int obj_id = it->id;

		//Проходим по всем стрелкам создания и смотрим на принадлежность
		bool is_connect = false;
		for(int j = 0; j < items.size(); j++)
		{
			//Пропустить не выделенные
			if(!items[j]->isSelected())
				continue;

			//Пропустить не стрелки
			DiagramItem *it = static_cast<DiagramItem *>(items[j]);
			if(it->item_type != EItemsType::MESSAGE_ITEM)
				continue;

			//Пропустить обычные стрелки
			DiagramArrow *arr = static_cast<DiagramArrow *>(items[j]);
			if(!arr->isCreationMessageArrow)
				continue;

			//Проверка
			DiagramItem *target = static_cast<DiagramItem *>(arr->targetItem);
			if(target->id == obj_id)
			{
				is_connect = true;
				break;
			}
		}

		//Если нет связи со стрелкой создания
		if(!is_connect)
		{
			//Поднять объект на высоту по умолчанию
			it->setVerticalSpace(vertical_space_default);
		}
	}

	//Обновляем линии жизни
	update_lifelines();

	//Обновляем активности
	update_activity();

	//Обновляем сцену
	update();
}