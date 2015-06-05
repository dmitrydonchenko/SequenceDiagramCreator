#include "graphicscene.h"

GraphicScene::GraphicScene(QObject *parent)
	: QGraphicsScene(parent)
{
	//��������� id �� ���������
	last_id = -1;
	current_state = DEFAULT_STATE;
	diagram_filename = "";

	//��������������� �������� ��� ����������� ��������� �� ���������
	is_drag = false;
	is_ctrl = false;
	is_shift = false;
	is_move = false;
	is_duplicate = false;

	//������������� ��������������� �������� drag and drop ���������� �� ���������
	mouse_drag_pos_x = 0;
	mouse_drag_pos_y = 0;

	//������� ������������� BSP ������ ��-�� ���� � ���������
	setItemIndexMethod(QGraphicsScene::NoIndex);

	//������� ����� ��� �������� ������, ��� ������
	mimeData = new QMimeData();
}

GraphicScene::~GraphicScene()
{
	//������� ��� ��������
	for(int i = 0; i < items.size(); i++)
		delete items[i];
}

//���������� �������� �� �����
void GraphicScene::addItem(QGraphicsItem *item)
{
	//����������� id ��������
	DiagramItem* it = static_cast<DiagramItem *>(item);
	it->id = ++last_id;

	//��������� ����� ������� � ������
	items.push_back(item);

	//�������������� ������� �� �����������
	QGraphicsScene::addItem(item);
}

//������ ������ ����
/*virtual*/ void GraphicScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	//��������� �������� � ����������� �� �������� ���������
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
	//�������� ����� �����
	update_lifelines();
}

//���������� ����� ��������� ��� �����
void GraphicScene::setNewState(EStateType s)
{
	current_state = s;
}

//������� ���������� �������
void GraphicScene::removeSelectedItem()
{
	//�������� �� ���� ��������� � ������� ���������� ��������� - ������� � ������������ � ������
	for(int i = 0; i < items.size(); i++)
	{
		//���� ������� �� ������� - ����������
		if(!items[i]->isSelected())
			continue;

		//���� ������� �� �������� �������� ��� ��������� ������������ ��� ������ - ����������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::COMMENT_ITEM && it->item_type != EItemsType::MESSAGE_ITEM && it->item_type != EItemsType::LOOP_ITEM)
			continue;

		//�������� ������ ���������� ��������, ������� �������� ��� �������� ��� ������������

		//������ �������� � ����������� �� ���� �������
		if(it->item_type == EItemsType::COMMENT_ITEM)
		{
			//��������� �����������

			//������� �� �������
			items.erase(items.begin() + i);

			//������� �� �����
			this->removeItem(it);

			//������� ������
			delete it;
		}
		else if(it->item_type == EItemsType::LOOP_ITEM)
		{
			//����

			//������� �� �������
			items.erase(items.begin() + i);

			//������� �� �����
			this->removeItem(it);

			//������� ������
			delete it;
		}
		else if(it->item_type == EItemsType::MESSAGE_ITEM)
		{
			//�������

			//�������� �������
			DiagramArrow *cur_arr = static_cast<DiagramArrow *>(it);
			DiagramObject *target = static_cast<DiagramObject *>(cur_arr->targetItem);

			//���� ��������� ��������
			if(cur_arr->isCreationMessageArrow)
			{
				//��������� ������ ������� ��� ������
				target->setVerticalSpace(vertical_space_default);
			}

			//������� �� �������
			items.erase(items.begin() + i);

			//������� �� �����
			this->removeItem(it);

			//������� ������
			delete it;
		}

		//����� ��������, �������� �� ��� �� ����� � �������
		i--;
	}

	//������ ������� ��� ���������� �������

	//�������� �� ���� ��������� � ������� ���������� ��������� - ��������
	for(int i = 0; i < items.size(); i++)
	{
		//���� ������� �� ������� - ����������
		if(!items[i]->isSelected())
			continue;

		//���� ������� �� �������� �������� - ����������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//�������� ������ ���������� ��������, ������� �������� ���������

		//������� ��� ���������, ��������� � ���� ��������
		for(int j = 0; j < items.size(); j++)
		{
			//���������� ������� ��� ���
			DiagramItem *cur_obj = static_cast<DiagramItem *>(items[j]);
			if(cur_obj->item_type != EItemsType::MESSAGE_ITEM)
				continue;

			//�������� �������
			DiagramArrow *cur_arr = static_cast<DiagramArrow *>(items[j]);
			DiagramObject *target = static_cast<DiagramObject *>(cur_arr->targetItem);
			DiagramObject *source = static_cast<DiagramObject *>(cur_arr->sourceItem);

			//���������, ������� �� ��� ������� � ������� ��������
			if(source->id != it->id && target->id != it->id)
				continue;

			//���� ��������� ��������
			if(cur_arr->isCreationMessageArrow)
			{
				//������� ���� �� �������� �������
				if(source->id == it->id)
				{
					//��������� ������ ������
					target->setVerticalSpace(vertical_space_default);
				}

				//������ �������

				//������� �� �����
				this->removeItem(items[j]);

				//������� ������
				delete items[j];

				//������� �� �������
				items.erase(items.begin() + j);
			}
			else	//���� ���������� ���������
			{
				//������� �� �����
				this->removeItem(items[j]);

				//������� ������
				delete items[j];

				//������� �� �������
				items.erase(items.begin() + j);
			}
			//������������
			j = -1;
		}

		//������� ������ �� �������
		//(index �� ������ ��� ���������� ����� �������� �������
		//�.�. ��������� � ������� ��������� ����� ������ ����� �������� �������)
		items.erase(items.begin() + i);

		//������� �� �����
		this->removeItem(it);

		//������� ������
		delete it;

		//������������
		i = -1;
	}

	//�������� ����� �����
	update_lifelines();

	//��������� �����
	QGraphicsScene::update();
}

//����������� �����
void GraphicScene::move_loop(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//���������� ���� ����� �� ���, ����� ��� �� ����� �� ������� ������� �������
	
	//�������� ����
	DiagramLoop *loop = static_cast<DiagramLoop *>(it);

	//���� ��� ����������� - �� ������ �� ������
	if(used.count(loop->id))
		return;

	//���������, ��� �������� �������
	used.insert(loop->id);

	//�������� ������� �������� �� �����
	qreal scene_object_pos_x = loop->scenePos().x();
	qreal scene_object_pos_y = loop->scenePos().y();
	loop->leftVertex.setX(scene_object_pos_x);
	loop->leftVertex.setY(scene_object_pos_y);
	loop->rightVertex.setX(scene_object_pos_x + loop->getWidth());
	loop->rightVertex.setY(scene_object_pos_y + loop->getHeight());

	//�������� ����� ���������� � ������ ��������
	qreal old_scene_object_pos_x = loop->scenePos().x();
	qreal old_scene_object_pos_y = loop->scenePos().y();
	scene_object_pos_x += dx;
	scene_object_pos_y += dy;

	//������, ����� ������� �� ����� �� ������� ����
	scene_object_pos_x = max(0, (int)scene_object_pos_x);
	scene_object_pos_x = min(scene_width - loop->getWidth(), (int)scene_object_pos_x);
	scene_object_pos_y = max(0, (int)scene_object_pos_y);
	scene_object_pos_y = min(scene_height - loop->getHeight(), (int)scene_object_pos_y);

	//����������� ����� ����������
	loop->setPos(scene_object_pos_x, scene_object_pos_y);

	//�������� ��������
	int r_dx = scene_object_pos_x - old_scene_object_pos_x;
	int r_dy = scene_object_pos_y - old_scene_object_pos_y;

	//�������
	loop->move_loop(r_dx, r_dy, used);

	/*
	bool hasParent = false; 
	//�������������� ������� "��������" � �����
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

//������ ������ ����
/*virtual*/ void GraphicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if(current_state == EStateType::SELECT_LOOP_RECTANGLE_AREA)
	{
		mouseReleaseEventSelectLoopRectangleLastVertex(event);
	}
	else
	{
		//���� ������ ����� ������ ����
		if(event->button() == Qt::LeftButton)
		{
			//�������, ��� ������� �������������� �����������
			is_drag = false;

			//���� �������� ������ - ���������� ��� ��� ������� ���� �� ������� �������
			if(!is_move && !is_ctrl)
			{
				//����������� ���
				unselectAll();

				//�������� �������� ������� ��� ��������
				QGraphicsItem *it = itemAt(event->scenePos());

				//���� ��� �������� ���� �������
				if(it != NULL)
				{
					//�������� ������������ �������
					while(it->parentItem() != NULL)
						it = it->parentItem();

					//��������
					it->setSelected(true);
				}
			}

			//�������� ����� �����
			update_lifelines();
		}

		//�������� ���� ������������
		is_move = false;


		//�������� ���� ������������
		is_duplicate = false;

		//��������� �����
		QGraphicsScene::update();
	}
}

//�������� ����� �����
void GraphicScene::update_lifelines()
{
	int max_h = 0;		//������������ ������ ���������

	//�������� �� ���� �������� �����
	for(int i = 0; i < items.size(); i++)
	{
		//���������� - ���������� ��������� ��� ���
		DiagramItem *cur_obj = static_cast<DiagramItem *>(items[i]);
		if(cur_obj->item_type == EItemsType::MESSAGE_ITEM)
		{
			//������� ������������ ������ ���������
			DiagramArrow *diag_arrow = static_cast<DiagramArrow *>(items[i]);
			int cur_h = diag_arrow->getVerticalSpace();
			if(cur_h > max_h)
				max_h = cur_h;
		}
		else if(cur_obj->item_type == EItemsType::OBJECT_ITEM)
		{
			//���� ������, �� ������� �� ��� ������
			DiagramObject *diag_obj = static_cast<DiagramObject *>(items[i]);
			int cur_h = diag_obj->getVerticalSpace() + diag_obj->getHeight();
			if(cur_h > max_h)
				max_h = cur_h;
		}
	}

	//����������� ����� �����
	int const_h = life_line_len_default;	//�������������� ����� ����� ����� (����� ����� ����� �� ������������� ����� �� �������)
	for(int i = 0; i < items.size(); i++)
	{
		//���������� - ������ ��� ���
		DiagramItem *cur_obj = static_cast<DiagramItem *>(items[i]);
		if(cur_obj->item_type == EItemsType::OBJECT_ITEM)
		{
			//����������� ���� ������ �� ����������
			DiagramObject *diag_obj = static_cast<DiagramObject *>(items[i]);
			if(diag_obj->isLifeLineStop())	
			{
				//�������� ��������� �������, ��������� � ��������
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

	//�������������� ���������� ��������
	update_activity();

	//��������� �����
	QGraphicsScene::update();
}

//����������� ����
/*virtual*/ void GraphicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if(current_state == EStateType::SELECT_LOOP_RECTANGLE_AREA)
	{
		mouseMoveEventSelectLoopRectangleArea(event);
	}
	else
	{
		//���� ���������� ������� ��������������
		if(is_drag)
		{
			//���� ���������� ������� ������������ ���������� ��������� - ��������������
			if(is_shift && !is_duplicate)
			{
				is_duplicate = true;
				duplicateSelection();
			}

			//������� ��������� �������������� ���������
			set<int> used;

			//��������� ��������, ��� ������� ������ ���� ��������

			//����� ������� ����
			qreal new_mouse_drag_pos_x = event->scenePos().x();
			qreal new_mouse_drag_pos_y = event->scenePos().y();

			//��������� ��������
			int dx = new_mouse_drag_pos_x - mouse_drag_pos_x;
			int dy = new_mouse_drag_pos_y - mouse_drag_pos_y;

			//��������� ������� ����
			mouse_drag_pos_x = new_mouse_drag_pos_x;
			mouse_drag_pos_y = new_mouse_drag_pos_y;

			//���� �������� ��������� - ���������
			if(dx != 0 || dy != 0)
				is_move = true;

			//�������� ������ ���������, ������� ����� ��������
			vector<QGraphicsItem *> items_for_drag;
			for(int i = 0; i < items.size(); i++)
				if(items[i]->isSelected())
					items_for_drag.push_back(items[i]);

			//��������� �� �� ������ - vert_space
			for(int i = 0; i < items_for_drag.size(); i++)
				for(int j = i + 1; j < items_for_drag.size(); j++)
					if(((DiagramItem *)items_for_drag[i])->getVerticalSpace() > ((DiagramItem *)items_for_drag[j])->getVerticalSpace())
						swap(items_for_drag[i], items_for_drag[j]);

			//�������� �� ���� ���������� ��������� � ������� ��
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

		//�������� ����� �����
		update_lifelines();

		//��������� �����
		QGraphicsScene::update();
	}
}

//��������������� �����
void GraphicScene::scaleScene(int p)
{
	//�������� �������� ������������� ����� � ���� �� ������� - �������
	QList<QGraphicsView *> list = QGraphicsScene::views();
	if(list.size() == 0)
		return;
	QGraphicsView *v = list[0];

	//��������������� (� ������������� - ��������� �� ���������)
	v->resetMatrix();
	v->scale(qreal(p) / 100.0, qreal(p) / 100);
}


//������� �� ������� ������ ���� � ������� ���������
void GraphicScene::mousePressEventDefault(QGraphicsSceneMouseEvent *event)
{
	//���� ������ ����� ������ ����
	if(event->button() == Qt::LeftButton)
	{
		//�������� �������� ������� ��� ��������
		QGraphicsItem *it = itemAt(event->scenePos());

		//���� ��� �������� ���� �������
		if(it != NULL)
		{
			//�������� ������������ �������
			while(it->parentItem() != NULL)
				it = it->parentItem();

			//���� �� ������ ������� ctrl - �������� ������� � ����� ��������� � ���������
			if(!is_ctrl)
			{
				if(!it->isSelected())
					unselectAll();
				it->setSelected(true);
			}
			else	//����� ������������� ���������
				it->setSelected(!it->isSelected());

			//���������, ��� �������� ������� ��������������
			is_drag = true;

			//���������� ���������� ������ ��������������
			mouse_drag_pos_x = event->scenePos().x();
			mouse_drag_pos_y = event->scenePos().y();

			//���� ��� ���� - ��������� ��� ���� ��� ��������� �����
			DiagramItem *cur_it = static_cast<DiagramItem *>(it);
			if(cur_it->item_type == EItemsType::LOOP_ITEM)
			{
				DiagramLoop *loop = static_cast<DiagramLoop *>(it);
				//�������� ��� ��������� �����
				loop->stacked_loops = loop->getLoopInVector(items);
			}
		}
		else
		{
			//���� ������� ctrl �� ������ - ����� ��������� �� ���� ��������
			if(!is_ctrl)
				unselectAll();
		}
	}
}

//������� �� ������� ������ ���� � ��������� ���������� �������
void GraphicScene::mousePressEventSelectPlaceForObject(QGraphicsSceneMouseEvent *event)
{
	//���� ������ ����� ������ ����
	if(event->button() == Qt::LeftButton)
	{
		//���������� ���������� �� ��� X - ���� ����� ������� ������
		qreal x_pos = event->scenePos().x();

		//������� ����� ������
		DiagramObject *new_obj = new DiagramObject();

		//������ ��� ����������
		new_obj->setPosX(x_pos);

		//��������� �� �����
		addItem(new_obj);
	}

	//��������� � ��������� �� ���������
	setNewState(EStateType::DEFAULT_STATE);
}

//������� �� ������� ������ ���� � ��������� ���������� �����������
void GraphicScene::mousePressEventSelectPlaceForComment(QGraphicsSceneMouseEvent *event)
{
	//���� ������ ����� ������ ����
	if(event->button() == Qt::LeftButton)
	{
		//���������� ���������� - ���� ����� ������� ������
		qreal x_pos = event->scenePos().x();
		qreal y_pos = event->scenePos().y();

		//������� ����� ������
		DiagramComment *new_obj = new DiagramComment();

		//������ ��� ����������
		new_obj->setPos(x_pos, y_pos);
		new_obj->setVerticalSpace(y_pos);

		//��������� �� �����
		addItem(new_obj);
	}

	//��������� � ��������� �� ���������
	setNewState(EStateType::DEFAULT_STATE);
}

//������� �� ������� ������ ���� � ��������� ���������� ������ ����. ���������
void GraphicScene::mousePressEventSelectMessageBegin(QGraphicsSceneMouseEvent *event)
{
	//���� ������ ����� ������ ����
	if(event->button() == Qt::LeftButton)
	{
		//������� ��������� �� ���� ��������
		unselectAll();

		//�������� ������� ��� ��������
		QGraphicsItem *it = itemAt(event->scenePos());

		//����� ������ �������� �������� (��������)
		if(it != NULL)
		{
			while(it->parentItem() != NULL)
				it = it->parentItem();
			it->setSelected(true);
		}

		//���� ������� ���� � �� �������� ��������
		if(it != NULL && ((DiagramItem *)it)->item_type == EItemsType::OBJECT_ITEM)
		{
			//��������
			it->setSelected(true);

			//������� ����� �������
			new_arrow = new DiagramArrow();

			//���������, ������ ���� �������
			new_arrow->sourceItem = it;

			//��������� ����������� ������ ������� �� ������� �����
			DiagramObject *source_obj = static_cast<DiagramObject *>(it);
			int min_height = source_obj->getVerticalSpace() + source_obj->getHeight();
			int mouse_pos_y = event->scenePos().y() - new_arrow->getHeight() / 2;
			int init_height = max(min_height, mouse_pos_y);
			new_arrow->setVerticalSpace(init_height);

			//������ ������� ���������
			setNewState(EStateType::SELECT_MESSAGE_END_STATE);
		}
		else
			setNewState(EStateType::DEFAULT_STATE);	//����� ��������� � ��������� �� ���������
	}
	else
		setNewState(EStateType::DEFAULT_STATE);	//����� ��������� � ��������� �� ���������
}

//������� �� ������� ������ ���� � ��������� ���������� ����� ����. ���������
void GraphicScene::mousePressEventSelectMessageEnd(QGraphicsSceneMouseEvent *event)
{
	//���� ������ ����� ������ ����
	if(event->button() == Qt::LeftButton)
	{
		//������� ��������� �� ���� ��������
		unselectAll();

		//�������� ������� ��� ��������
		QGraphicsItem *it = itemAt(event->scenePos());

		//����� ������ �������� �������� (��������)
		if(it != NULL)
		{
			while(it->parentItem() != NULL)
				it = it->parentItem();
			it->setSelected(true);
		}

		//���� ������� ���� � �� �������� �������� � ������� �� ���� � ��� �� ������ �� �������� �����
		if(it != NULL && ((DiagramItem *)it)->item_type == EItemsType::OBJECT_ITEM
			&& ((DiagramItem *)it)->id != ((DiagramItem *)new_arrow->sourceItem)->id)
		{
			//��������
			it->setSelected(true);

			//��������� ���� ���� �������
			new_arrow->targetItem = it;

			//��������� ������� �� �����
			this->addItem(new_arrow);

			//�������� �������� �������
			new_arrow->refreshValues();
		}
		else
			delete new_arrow;		//������� ��������� ������ ����������� ���������
	}
	else
		delete new_arrow;			//������� ��������� ������ ����������� ���������

	//������ ��������� �� ���������
	setNewState(DEFAULT_STATE);
}

//������� �� ������� ������ ���� � ��������� ���������� ������ ��������� ��������
void GraphicScene::mousePressEventSelectCreationMessageBegin(QGraphicsSceneMouseEvent *event)
{
	//���� ������ ����� ������ ����
	if(event->button() == Qt::LeftButton)
	{
		//������� ��������� �� ���� ��������
		unselectAll();

		//�������� ������� ��� ��������
		QGraphicsItem *it = itemAt(event->scenePos());

		//����� ������ �������� �������� (��������)
		if(it != NULL)
		{
			while(it->parentItem() != NULL)
				it = it->parentItem();
			it->setSelected(true);
		}

		//���� ������� ���� � �� �������� ��������
		if(it != NULL && ((DiagramItem *)it)->item_type == EItemsType::OBJECT_ITEM)
		{
			//��������
			it->setSelected(true);

			//������� ����� �������
			new_arrow = new DiagramArrow();
			new_arrow->isCreationMessageArrow = true;

			//���������, ������ ���� �������
			new_arrow->sourceItem = it;

			//��������� ����������� ������ ������� �� ������� �����
			DiagramObject *source_obj = static_cast<DiagramObject *>(it);
			int min_height = source_obj->getVerticalSpace() + source_obj->getHeight();
			int mouse_pos_y = event->scenePos().y() - new_arrow->getHeight() / 2;
			int init_height = max(min_height, mouse_pos_y);
			new_arrow->setVerticalSpace(init_height);

			//������ ������� ���������
			setNewState(EStateType::SELECT_CREATION_MESSAGE_END_STATE);
		}
		else
			setNewState(EStateType::DEFAULT_STATE);	//����� ��������� � ��������� �� ���������
	}
	else
		setNewState(EStateType::DEFAULT_STATE);	//����� ��������� � ��������� �� ���������
}

//������� �� ������� ������ ���� � ��������� ���������� ����� ��������� ��������
void GraphicScene::mousePressEventSelectCreationMessageEnd(QGraphicsSceneMouseEvent *event)
{
	//������ ��������� �� ���������
	setNewState(DEFAULT_STATE);

	//���� ������ �� ����� ������ ����
	if(event->button() != Qt::LeftButton)
	{
		delete new_arrow;			//������� ��������� ������ ����������� ���������
		return;
	}

	//������� ��������� �� ���� ��������
	unselectAll();

	//�������� ������� ��� ��������
	QGraphicsItem *it = itemAt(event->scenePos());

	//����� ������ �������� �������� (��������)
	if(it != NULL)
	{
		while(it->parentItem() != NULL)
			it = it->parentItem();
		it->setSelected(true);
	}

	//���� ������� ���� � �� �������� �������� � ������� �� ���� � ��� �� ������ �� �������� ����� � ����� ������� ��������
	if(it != NULL && 
		((DiagramItem *)it)->item_type == EItemsType::OBJECT_ITEM && 
		((DiagramItem *)it)->id != ((DiagramItem *)new_arrow->sourceItem)->id &&
		isCreationMessageCanBeCreated((DiagramObject *)new_arrow->sourceItem, (DiagramObject *)it))
	{
		//���������� ����������� ��������� ���������� ��� ������� �������
		int possible_height = dfs_get_possible_height((DiagramObject *)it, items);

		//���� ������� ������� ����� ����������� ������� ����� (�������� ��������� �������� ����� �������� �� ������� ������� �������)
		if(new_arrow->getVerticalSpace() + new_arrow->getHeight() / 2 > scene_height - possible_height)
		{
			QMessageBox::warning(NULL, "������", "������ ������� ��������� ��������, ��� ��� ��������, ����� ���������� ��������, ����� �������� �� ������� ������� �������");
			delete new_arrow;
			return;
		}

		//��������
		it->setSelected(true);

		//��������� ���� ���� �������
		new_arrow->targetItem = it;

		//����������, �� ����� ������ ��������� ������
		int old_height = ((DiagramObject *)new_arrow->targetItem)->getVerticalSpace() + ((DiagramObject *)new_arrow->targetItem)->getHeight() / 2;

		//���������� ����� ������
		int new_height = new_arrow->getVerticalSpace() + new_arrow->getHeight();

		//���������� ��������
		int dy = new_height - old_height;

		//��������� ������� �� �����
		this->addItem(new_arrow);

		//�������� �������� �������
		new_arrow->refreshValues();

		//����������� ��� ������� ��������
		move_items((DiagramObject *)it, items, dy);
	}
	else
		delete new_arrow;		//������� ��������� ������ ����������� ���������
}

//������� �� ������� ������ ���� � ��������� ������ ������ ������� �������������� �����
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

//������� �� ������������ ���� � ��������� ��������� ������������� ������� ��� �����
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

//������� �� ������� ������ ���� � ��������� ������ ��������� ������� �������������� �����
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
			QMessageBox::warning(NULL, "������", "������� ��������� ������ ������� ��� �����");
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
			//���������� ������� "��������" � ����� � �������, �������� �� ���� ���������
			for(int i = 0; i < items.size(); i++)
			{
				DiagramItem *it = static_cast<DiagramItem *>(items[i]);
				if(it->item_type == EItemsType::LOOP_ITEM)
				{
					DiagramLoop *loop = static_cast<DiagramLoop *>(items[i]);
					// ���� � ����� ���� ��������
					if(loop->leftVertex.x() <= new_loop->leftVertex.x() && loop->leftVertex.y() <= new_loop->leftVertex.y() && 
						loop->rightVertex.x() > new_loop->rightVertex.x() && loop->rightVertex.y() > new_loop->rightVertex.y())
					{
						new_loop->parentLoop = loop;
					}
					// ���� ���� �������� ���������
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

//������ ��������� �� ���� ��������
void GraphicScene::unselectAll()
{
	for(int i = 0; i < items.size(); i++)
		items[i]->setSelected(false);
}

//�������� ����������
void GraphicScene::update_activity()
{
	//������� ��������
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *cur_it = static_cast<DiagramItem *>(items[i]);
		if(cur_it->item_type == EItemsType::OBJECT_ITEM)
		{
			//����� ���� �������, ��������� � �������� � ��������� �� � ������
			vector<DiagramArrow *> cur_arr;
			for(int j = 0; j < items.size(); j++)
			{
				DiagramItem *exp_arr = static_cast<DiagramItem *>(items[j]);
				if(exp_arr->item_type == EItemsType::MESSAGE_ITEM)
				{
					DiagramArrow *arr = static_cast<DiagramArrow *>(items[j]);
					DiagramItem *source = static_cast<DiagramItem *>(arr->sourceItem);
					DiagramItem *target = static_cast<DiagramItem *>(arr->targetItem);
					//���������� - ������� �� ������� � ������� �������� � ���� ��, �� ��������� � ������
					if(source->id == cur_it->id || target->id == cur_it->id)
						cur_arr.push_back(arr);
				}
			}

			//��������� ������ ������� ��� �������� ������� �� ������
			for(int j = 0; j < cur_arr.size(); j++)
				for(int k = j + 1; k < cur_arr.size(); k++)
					if(cur_arr[j]->getVerticalSpace() > cur_arr[k]->getVerticalSpace())
						swap(cur_arr[j], cur_arr[k]);

			//������ ������� �����������
			vector<pair<int, int> > activity;									//������� �����������
			DiagramObject *cur_obj = static_cast<DiagramObject *>(items[i]);	//������ �������
			int balance = 0;													//������
			int cur_start = -1;													//������ ������� (��� �����������)
			for(int i = 0; i < cur_arr.size(); i++)
			{
				//���������� ������� �������� � ���� ��� �������
				DiagramItem *source = static_cast<DiagramItem *>(cur_arr[i]->sourceItem);
				DiagramItem *target = static_cast<DiagramItem *>(cur_arr[i]->targetItem);

				//���� ���� ��������� ��������, ������� ������ � ������� ������ - �� ��������� ��� �������
				if(cur_arr[i]->isCreationMessageArrow && cur_obj->id == target->id)
					continue;

				//���� ��������� �������� ������� �� �������
				if(cur_arr[i]->isCreationMessageArrow && source->id == cur_obj->id)
				{
					int s = cur_arr[i]->getVerticalSpace() - cur_obj->getVerticalSpace();
					int f = s + 10;
					activity.push_back(make_pair(s, f));
					continue;
				}

				//���� ������� ������� �� �������
				if(source->id == cur_obj->id)
				{
					balance--;

					//���� ���������� ������� ��� �� ����������� ��� ����������� ������ ���
					if(balance >= 0)
					{
						//���� ���� ������ ����������
						if(cur_start != -1)
						{
							int s = cur_arr[cur_start]->getVerticalSpace() - cur_obj->getVerticalSpace();
							int f = cur_arr[i]->getVerticalSpace() - cur_obj->getVerticalSpace();
							activity.push_back(make_pair(s, f));
							if(balance == 0)
								cur_start = -1;
						}
					}
					else	//���� ���������� ��� � ������
					{
						int s = cur_arr[i]->getVerticalSpace() - cur_obj->getVerticalSpace();
						int f = s + 10;
						activity.push_back(make_pair(s, f));
					}
				}
				else	//���� ������
				{
					balance++;

					//���� ������� �������� ������ ��������� - ��������� ������ ������
					if(balance == 1)
						cur_start = i;

					//������ ������ ������� ������� ����������
					int s = cur_arr[i]->getVerticalSpace() - cur_obj->getVerticalSpace();
					int f = s + 10;
					activity.push_back(make_pair(s, f));
				}
			}

			//���������� ������� � �������������
			for(int i = 1; i < activity.size(); i++)
			{
				int left1 = activity[i - 1].first;
				int right1 = activity[i - 1].second;
				int left2 = activity[i].first;
				int right2 = activity[i].second;

				//���� ���� �����������
				if(min(right1, right2) >= max(left1, left2))
				{
					activity[i - 1].first = min(left1, left2);
					activity[i - 1].second = max(right1, right2);
					activity.erase(activity.begin() + i);
					i--;
				}
			}

			//������ ����������
			cur_obj->setActivity(activity);
		}
	}
}

//����� �� ������� �������� ���� �������
bool GraphicScene::isCreationMessageCanBeCreated(DiagramObject *from, DiagramObject *to)
{
	//���-�� �������� ������� �������� ������ ���� �� ����� 1
	
	//����� ���� �������, �������� � ������ to
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *cur_it = static_cast<DiagramItem *>(items[i]);

		//����������� ��� �������
		if(cur_it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		DiagramArrow *cur_arr = static_cast<DiagramArrow *>(items[i]);
		DiagramObject *source = static_cast<DiagramObject *>(cur_arr->sourceItem);
		DiagramObject *target = static_cast<DiagramObject *>(cur_arr->targetItem);

		//���� ������� �� �������� ���������� �������� - ����������
		if(!cur_arr->isCreationMessageArrow)
			continue;

		//���� ������� �� �������� �������� - ����������
		if(target->id != to->id)
			continue;

		return false;

	}

	//������� �� ������ ������� � ������ �� ��������� (� ������ � ������ ������)

	//����� �����
	DiagramObject *root = from;		//������ ������
	bool isEnd = true;				//�������� ��������� ������
	while(isEnd)
	{
		isEnd = false;

		//����� ��������� ��������, ��������� � ������� ������
		for(int i = 0; i < items.size(); i++)
		{
			//����������� ��� �������� - �� �������
			DiagramItem *cur_it = static_cast<DiagramItem *>(items[i]);
			if(cur_it->item_type != EItemsType::MESSAGE_ITEM)
				continue;

			DiagramArrow *cur_arr = static_cast<DiagramArrow *>(items[i]);
			DiagramObject *source = static_cast<DiagramObject *>(cur_arr->sourceItem);
			DiagramObject *target = static_cast<DiagramObject *>(cur_arr->targetItem);

			//���� ������� �� �������� ���������� �������� - ����������
			if(!cur_arr->isCreationMessageArrow)
				continue;

			//���� ������� �� ������ � ������� ������ - ����������
			if(target->id != root->id)
				continue;

			//����������� ���� �� ������
			root = source;
			isEnd = true;
			break;
		}
	}

	//��������� - ���� �� ����� ������� � ������
	if(to->id == root->id)
		return false;
	
	return true;
}

//�������� ��������� ����������� ������� (����������� ����� ����� ��� ���)
void GraphicScene::changeStateOfLineEnd()
{
	//�������� ��� ���������� ������� �� ������ ������
	for(int i = 0; i < items.size(); i++)
	{
		//���� ������� �� ������� - ����������
		if(!items[i]->isSelected())
			continue;

		//���� �� ������ - ����������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//�������� ������
		DiagramObject *obj = static_cast<DiagramObject *>(it);

		//�������� ��������� �������, ��������� � ��������
		int max_arrow_h = getMaxArrowHeight(obj);

		//�������� ��������� ����� ����� �������
		obj->changeStateLifeLineStop(max_arrow_h);
	}

	//��������� ����� �����
	update_lifelines();
}

//�������� ������ ��������� �������, ��������� � ��������
int GraphicScene::getMaxArrowHeight(DiagramObject *obj)
{
	//�������� ��������� �������, ��������� � ��������
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

//�������� ���������
void GraphicScene::newDiagram()
{
	//������� ��� ��������
	for(int i = 0; i < items.size(); i++)
		delete items[i];
	items.clear();

	//��������� id �� ���������
	last_id = -1;
	current_state = DEFAULT_STATE;
	diagram_filename = "";

	//��������� �����
	update();
}

//������� ��������� �� �����
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

//��������� ��������� � ����
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

//�������� ������� ��� �����
QString GraphicScene::getFilename() const
{
	return diagram_filename;
}

//������ ������ ����������
/*virtual*/ void GraphicScene::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Control)
		is_ctrl = true;
	if(event->key() == Qt::Key_Shift)
		is_shift = true;
}

//������ ������ ����������
/*virtual*/ void GraphicScene::keyReleaseEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Control)
		is_ctrl = false;
	if(event->key() == Qt::Key_Shift)
		is_shift = false;
}

//����������� �������
void GraphicScene::move_object(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//�� ����������� ������� �� ������� ������ �����������
	//��� ��� ���������� ������ ����������� ������ �� �������� � ������� ����� ��� �� ���� �� ���� ������� �������
	
	//�������� ������
	DiagramObject *obj = static_cast<DiagramObject *>(it);

	//���� ��� ����������� - �� ������ �� ������
	if(used.count(obj->id))
		return;

	//���������, ��� �������� �������
	used.insert(obj->id);

	//�������� ������� ������� �� �����
	qreal scene_object_pos_x = obj->scenePos().x();
	qreal scene_object_pos_y = obj->scenePos().y();

	//�������� ����� ���������� � ������ �������� (������ ��������� ������ ����� ��� x)
	scene_object_pos_x += dx;

	//������, ����� ������ �� ����� �� ������� ����
	scene_object_pos_x = max(0, (int)scene_object_pos_x);
	scene_object_pos_x = min(scene_width - obj->getWidth(), (int)scene_object_pos_x);

	//����������� ����� ����������
	obj->setPos(scene_object_pos_x, scene_object_pos_y);
}

//����������� ����������� ���������
void GraphicScene::move_message(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//�� ����������� ����������� ��������� �� ������� ������ �����������
	//����� ������ ���������� ����� ��� Y � ������� �� ���, ����� �� �������� ������ �� ������� ������� � ����� �� �������� �� ������� ������� �������
	
	//�������� �������
	DiagramArrow *arr = static_cast<DiagramArrow *>(it);

	//���� ��� ����������� - �� ������ �� ������
	if(used.count(arr->id))
		return;

	//���������, ��� �������� �������
	used.insert(arr->id);

	//�������� ������� �������� �� �����
	qreal scene_object_pos_x = arr->scenePos().x();
	qreal scene_object_pos_y = arr->scenePos().y();

	//�������� ����� ���������� � ������ �������� (������ �� ��� Y)
	scene_object_pos_y += dy;

	//������, ����� ������� �� ����� �� ������� ���� � �� ������� �������� ������
	DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
	DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);
	scene_object_pos_y = max(source->getVerticalSpace() + source->getHeight(), (int)scene_object_pos_y);
	scene_object_pos_y = max(target->getVerticalSpace() + target->getHeight(), (int)scene_object_pos_y);
	scene_object_pos_y = min(scene_height - arr->getHeight(), (int)scene_object_pos_y);

	//����������� ����� ����������
	arr->setVerticalSpace(scene_object_pos_y);

	//��������� ����� �����
	update_lifelines();
}

//����������� ��������� ��������
void GraphicScene::move_creation_message(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//�� ����������� ��������� �������� �������� ����� ������
	//����� ��������� �� 2 ���� ����������� - ����� � ����

	//�������� ���������
	DiagramArrow *arr = static_cast<DiagramArrow *>(it);

	//���� ��� ����������� - �� ������ �� ������
	if(used.count(arr->id))
		return;

	//���������, ��� �������� �������
	used.insert(arr->id);

	//�������� ������� �������� �� �����
	qreal scene_object_pos_x = arr->scenePos().x();
	qreal scene_object_pos_y = arr->scenePos().y();

	//�������� ����� ���������� � ������ �������� (������ �� ��� Y)
	scene_object_pos_y += dy;

	//������, ����� ������� �� �����  �� ������� ������� ��������� ������
	DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
	DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);
	scene_object_pos_y = max(source->getVerticalSpace() + source->getHeight(), (int)scene_object_pos_y);

	//������, ����� ������� �� �������� ���������� ������ �����
	int possible_height = scene_height - 2 * arr->getHeight() - dfs_get_possible_height(target, items);
	scene_object_pos_y = min(possible_height, (int)scene_object_pos_y);

	//����������� ����� ����������
	arr->setVerticalSpace(scene_object_pos_y);

	//����������� target ������ �� ������ �������
	target->setVerticalSpace(scene_object_pos_y + arr->getHeight() / 2 - target->getHeight() / 2);

	//���� ����������� ���� ����� - ������ ������� �� ������������� � ���������� ������ ����������� �������

	//���� ����������� ���� ����
	if(dy > 0)
	{
		//���������� ��������, ������� ��� �� ��������������
		dfs_move_items(target, items, dy, used);
	}

	//��������� ����� �����
	update_lifelines();
}

//����������� ���������� �����������
void GraphicScene::move_comment(int dx, int dy, DiagramItem *it, set<int> &used)
{
	//�� ����������� ���������� ����������� �� ������� ������ �����������
	//���������� ����������� ����� �� ���, ����� ��� �� ����� �� ������� ������� �������
	
	//�������� �����������
	DiagramComment *comment = static_cast<DiagramComment *>(it);

	//���� ��� ����������� - �� ������ �� ������
	if(used.count(comment->id))
		return;

	//���������, ��� �������� �������
	used.insert(comment->id);

	//�������� ������� �������� �� �����
	qreal scene_object_pos_x = comment->scenePos().x();
	qreal scene_object_pos_y = comment->scenePos().y();

	//�������� ����� ���������� � ������ ��������
	scene_object_pos_x += dx;
	scene_object_pos_y += dy;

	//������, ����� ������� �� ����� �� ������� ����
	scene_object_pos_x = max(0, (int)scene_object_pos_x);
	scene_object_pos_x = min(scene_width - comment->getWidth(), (int)scene_object_pos_x);
	scene_object_pos_y = max(0, (int)scene_object_pos_y);
	scene_object_pos_y = min(scene_height - comment->getHeight(), (int)scene_object_pos_y);

	//����������� ����� ����������
	comment->setPos(scene_object_pos_x, scene_object_pos_y);
	comment->setVerticalSpace(scene_object_pos_y);
}

//������� ���� ����
/*virtual*/ void GraphicScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	//���� ������ ������ Ctrl - �� ���������� ����� ��������� � ������� ���� ������������ �� �����
	if(is_ctrl)
		return;

	//���� ��� �������� ��� �������� - ������ �� ������
	QGraphicsItem *q_it = itemAt(event->scenePos());
	if(q_it == NULL)
		return;

	//�������� ������
	DiagramItem *it = static_cast<DiagramItem *>(q_it);
	QString text_item_string = it->getTextItemString();

	//������� ���� � ����������
	DialogWindow *new_window = new DialogWindow(text_item_string);
	if(new_window->exec() == QDialog::Accepted)
	{
		//�������� ����� ����������� ��� �������
		QString s = new_window->getTextComment();

		//������������� ��������� �����������
		if(s != text_item_string)
			it->setTextComment(s);
	}

	//������� ����
	delete new_window;
}

//������� �������� ���������� ���������
void GraphicScene::duplicateSelection()
{
	//����� ����������� ��� ���������� �������� ���������, �������� ������ ��������, ����������� ������ � �������� �����

	//������� ��������� ��� �������� ������ � ������� ���������������
	map<int, int> new_id;

	//�������� �����
	for(int i = 0; i < items.size(); i++)
	{
		//���������� �� ���������� ��������
		if(!items[i]->isSelected())
			continue;

		//���������� �������� �� ���������� �������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::LOOP_ITEM)
			continue;

		//�������� ���������� �����

		//��������
		DiagramLoop *loop = static_cast<DiagramLoop *>(items[i]);
		DiagramItem *duplicate_it = loop->getCopy();

		//������� � ������ id
		new_id[duplicate_it->id] = last_id + 1;

		//��������� �������
		QGraphicsItem *q_it = static_cast<QGraphicsItem *>(duplicate_it);
		addItem(q_it);
	}

	//�������� �����������
	for(int i = 0; i < items.size(); i++)
	{
		//���������� �� ���������� ��������
		if(!items[i]->isSelected())
			continue;

		//���������� �������� �� ���������� �����������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::COMMENT_ITEM)
			continue;

		//�������� ���������� �����������

		//��������
		DiagramComment *comment = static_cast<DiagramComment *>(items[i]);
		DiagramItem *duplicate_it = comment->getCopy();

		//������� � ������ id
		new_id[duplicate_it->id] = last_id + 1;

		//��������� �������
		QGraphicsItem *q_it = static_cast<QGraphicsItem *>(duplicate_it);
		addItem(q_it);
	}

	//�������� �������
	for(int i = 0; i < items.size(); i++)
	{
		//���������� �� ���������� ��������
		if(!items[i]->isSelected())
			continue;

		//���������� �������� �� ���������� ���������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//�������� ���������� �������

		//��������
		DiagramObject *obj = static_cast<DiagramObject *>(items[i]);
		DiagramItem *duplicate_it = obj->getCopy();

		//������� � ������ id
		new_id[duplicate_it->id] = last_id + 1;

		//��������� �������
		QGraphicsItem *q_it = static_cast<QGraphicsItem *>(duplicate_it);
		addItem(q_it);
	}

	//�������� �������
	for(int i = 0; i < items.size(); i++)
	{
		//���������� �� ���������� ��������
		if(!items[i]->isSelected())
			continue;

		//���������� �������� �� ���������� ���������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		//�������� ���������� �������

		//��������
		DiagramArrow *arr = static_cast<DiagramArrow *>(items[i]);
		DiagramItem *duplicate_it = arr->getCopy();
		DiagramArrow *duplicate_arr = static_cast<DiagramArrow *>(duplicate_it);

		//�������� ������ � �����
		DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
		DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);

		//���� ��������� ��������
		if(arr->isCreationMessageArrow)
		{
			//���� target �� �����������
			if(!new_id.count(target->id))
			{
				//�� ����������� � ����������
				delete duplicate_it;
				continue;
			}
		}
		
		//��������� ���������� � source
		if(new_id.count(source->id))
		{
			//����� ������ ���������
			for(int j = 0; j < items.size(); j++)
			{
				DiagramItem *new_source = static_cast<DiagramItem *>(items[j]);
				QGraphicsItem *q_new_source = static_cast<QGraphicsItem *>(items[j]);
				
				//���� ����� - ��������� � ���������
				if(new_source->id == new_id[source->id])
				{
					duplicate_arr->sourceItem = q_new_source;
					break;
				}
			}
		}

		//��������� ���������� � target
		if(new_id.count(target->id))
		{
			//����� ������ ���������
			for(int j = 0; j < items.size(); j++)
			{
				DiagramItem *new_target = static_cast<DiagramItem *>(items[j]);
				QGraphicsItem *q_new_target = static_cast<QGraphicsItem *>(items[j]);
				
				//���� ����� - ��������� � ���������
				if(new_target->id == new_id[target->id])
				{
					duplicate_arr->targetItem = q_new_target;
					break;
				}
			}
		}

		//������� � ������ id
		new_id[duplicate_it->id] = last_id + 1;

		//��������� �������
		QGraphicsItem *q_it = static_cast<QGraphicsItem *>(duplicate_it);
		addItem(q_it);
	}

	//����������� ������ �������� � �������� �����
	for(int i = 0; i < items.size(); i++)
	{
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		
		//���� ����� �������, ������� ��� �������������
		if(new_id.count(it->id))
		{
			//����������� ���
			items[i]->setSelected(false);

			//����� ��� ����� � ��������� ��
			for(int j = 0; j < items.size(); j++)
			{
				DiagramItem *copy_it = static_cast<DiagramItem *>(items[j]);
				if(copy_it->id == new_id[it->id])
				{
					items[j]->setSelected(true);

					//���� �������������� ������ - ����� ��������� - �� ���������� �� �� ������
					if(copy_it->item_type == EItemsType::OBJECT_ITEM)
					{
						bool isFind = false;
						for(int k = 0; k < items.size(); k++)
						{
							//���������� ����� ��������� ��������, ������� ���� � ������� ������
							DiagramItem *check_it = static_cast<DiagramItem *>(items[k]);

							if(check_it->item_type != EItemsType::MESSAGE_ITEM)
								continue;

							DiagramArrow *check_arr = static_cast<DiagramArrow *>(items[k]);
							if(!check_arr->isCreationMessageArrow)
								continue;

							//���� ����� ����� ������� - �� ������� ���
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

//�������� ���������� �������� � ����� ������
void GraphicScene::cutSelection()
{
	copySelection();
	removeSelectedItem();
}

//����������� ���������� �������� � ����� ������
void GraphicScene::copySelection()
{
	//������� ������ ������
	QByteArray buf("");

	//����������� ��� �����
	for(int i = 0; i < items.size(); i++)
	{
		//���������� ��� �� ����������
		if(!items[i]->isSelected())
			continue;
		
		//���������� ��� �� �����
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::LOOP_ITEM)
			continue;

		//�������� ���������� �����

		//������������
		DiagramLoop *loop = static_cast<DiagramLoop *>(items[i]);
		buf.append(loop->getInfoForBuffer() + "\t");
	}

	//����������� ��� �����������
	for(int i = 0; i < items.size(); i++)
	{
		//���������� ��� �� ����������
		if(!items[i]->isSelected())
			continue;
		
		//���������� ��� �� �����������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::COMMENT_ITEM)
			continue;

		//�������� ���������� �����������

		//������������
		DiagramComment *comment = static_cast<DiagramComment *>(items[i]);
		buf.append(comment->getInfoForBuffer() + "\t");
	}

	//����������� ��� �������
	for(int i = 0; i < items.size(); i++)
	{
		//���������� ��� �� ����������
		if(!items[i]->isSelected())
			continue;

		//���������� ��� �� �������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//�������� ���������� �������

		//������������
		DiagramObject *obj = static_cast<DiagramObject *>(items[i]);
		buf.append(obj->getInfoForBuffer() + "\t");
	}

	//����������� ��� �������, ��������, ��� ��� �������, ��������� �� �������� ������ ���� ����������� ����
	for(int i = 0; i < items.size(); i++)
	{
		//���������� ��� �� ����������
		if(!items[i]->isSelected())
			continue;

		//���������� ��� �� �������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		//�������� ���������� �������

		//��������� - ������������� �� ������ �� �������� ������� �������
		DiagramArrow *arr = static_cast<DiagramArrow *>(items[i]);
		DiagramItem *source = static_cast<DiagramItem *>(arr->sourceItem);
		DiagramItem *target = static_cast<DiagramItem *>(arr->targetItem);
		bool is_source = false;
		bool is_target = false;
		for(int j = 0; j < items.size(); j++)
		{
			//���������� ��� �� ����������
			if(!items[j]->isSelected())
				continue;

			//���������� ��� �� �������
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

		//����������
		buf.append(arr->getInfoForBuffer() + "\t");
	}

	//������� � �����
	mimeData->setData("text/csv", buf);
	QApplication::clipboard()->setMimeData(mimeData);
}

//�������� �������� �� ������ ������
void GraphicScene::pasteFromBuffer()
{
	//����������� ��� ��������
	unselectAll();

	//������� �� ������ ������
	const QMimeData *out = QApplication::clipboard()->mimeData();
	QByteArray buf = out->data("text/csv");

	//��������� �� ������
	QList<QByteArray> list = buf.split('\t');

	//������� ��������� � ������� ����� ������� ����� ��
	map<int, int> new_id;

	//�������� �� ������� � ��������� ��������
	for(int i = 0; i < list.length(); i++)
	{
		//���� ����� ����
		if(list[i] == "<diagramloop_tag>")
		{
			//���������� ����� ������ ��� ����
			QList<QByteArray> l;
			l.append(list[i]);
			do
			{
				i++;
				l.append(list[i]);
			}
			while(list[i] != "</diagramloop_tag>");

			//������� ����� �������
			DiagramLoop *loop = new DiagramLoop();

			//�������� ��� ������
			loop->setInfoFromBuffer(l);

			//���������� ����� id
			new_id[loop->id] = last_id + 1;

			//��������� �� �����
			QGraphicsItem *q_loop = static_cast<QGraphicsItem *>(loop);
			addItem(q_loop);

			//��������
			q_loop->setSelected(true);
		}
		else if(list[i] == "<diagramcomment_tag>")	//���� ����� �����������
		{
			//���������� ����� ������ ��� ����
			QList<QByteArray> l;
			l.append(list[i]);
			do
			{
				i++;
				l.append(list[i]);
			}
			while(list[i] != "</diagramcomment_tag>");

			//������� ����� �������
			DiagramComment *comment = new DiagramComment();

			//�������� ��� ������
			comment->setInfoFromBuffer(l);

			//���������� ����� id
			new_id[comment->id] = last_id + 1;

			//��������� �� �����
			QGraphicsItem *q_comment = static_cast<QGraphicsItem *>(comment);
			addItem(q_comment);

			//��������
			q_comment->setSelected(true);
		}
		else if(list[i] == "<diagramobject_tag>") //���� ����� ������
		{
			//���������� ����� ������ ��� ����
			QList<QByteArray> l;
			l.append(list[i]);
			do
			{
				i++;
				l.append(list[i]);
			}
			while(list[i] != "</diagramobject_tag>");

			//������� ����� �������
			DiagramObject *obj = new DiagramObject();

			//�������� ��� ������
			obj->setInfoFromBuffer(l);

			//���������� ����� id
			new_id[obj->id] = last_id + 1;

			//��������� �� �����
			QGraphicsItem *q_obj = static_cast<QGraphicsItem *>(obj);
			addItem(q_obj);

			//��������
			q_obj->setSelected(true);
		}
		else if(list[i] == "<diagramarrow_tag>") //���� ����� �������
		{
			//���������� ����� ������ ��� ����
			QList<QByteArray> l;
			l.append(list[i]);
			do
			{
				i++;
				l.append(list[i]);
			}
			while(list[i] != "</diagramarrow_tag>");

			//������� ����� �������
			DiagramArrow *arr = new DiagramArrow();

			//�������� ��� ������
			arr->setInfoFromBuffer(l, items, new_id);

			//���������� ����� id
			new_id[arr->id] = last_id + 1;

			//��������� �� �����
			QGraphicsItem *q_arr = static_cast<QGraphicsItem *>(arr);
			addItem(q_arr);

			//��������
			q_arr->setSelected(true);
		}
	}

	//�������� �� ���� ���������� �������� � ���� � ������ �� ������ ��������� �������� - ������� �� ������ �� ���������
	for(int i = 0; i < items.size(); i++)
	{
		//���������� �� ����������
		if(!items[i]->isSelected())
			continue;

		//���������� �� �������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::OBJECT_ITEM)
			continue;

		//�������� id ����������� �������
		int obj_id = it->id;

		//�������� �� ���� �������� �������� � ������� �� ��������������
		bool is_connect = false;
		for(int j = 0; j < items.size(); j++)
		{
			//���������� �� ����������
			if(!items[j]->isSelected())
				continue;

			//���������� �� �������
			DiagramItem *it = static_cast<DiagramItem *>(items[j]);
			if(it->item_type != EItemsType::MESSAGE_ITEM)
				continue;

			//���������� ������� �������
			DiagramArrow *arr = static_cast<DiagramArrow *>(items[j]);
			if(!arr->isCreationMessageArrow)
				continue;

			//��������
			DiagramItem *target = static_cast<DiagramItem *>(arr->targetItem);
			if(target->id == obj_id)
			{
				is_connect = true;
				break;
			}
		}

		//���� ��� ����� �� �������� ��������
		if(!is_connect)
		{
			//������� ������ �� ������ �� ���������
			it->setVerticalSpace(vertical_space_default);
		}
	}

	//��������� ����� �����
	update_lifelines();

	//��������� ����������
	update_activity();

	//��������� �����
	update();
}