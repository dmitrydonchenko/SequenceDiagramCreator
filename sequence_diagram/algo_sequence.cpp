#include "algo_sequence.h"

#include "DiagramArrow.h"

//Поиск минимального расстояния от нижней границы рабочего пространства до верхней границы стрелки сообщения создания, которое будет подводиться к root
int dfs_get_possible_height(DiagramObject *root, vector<QGraphicsItem *> &items)
{
	//Получаем глубину дерева
	int deep = dfs_get_deep_of_tree(root, items);

	int arrow_height = 7;		//Высота стрелки стандартная
	int object_height = 50;		//Высота объекта стандартная

	return (deep * object_height + (deep + 1) * arrow_height) / 2;
}

//Поиск глубины дерева
int dfs_get_deep_of_tree(DiagramObject *root, vector<QGraphicsItem *> &items)
{
	int cur_id = root->id;
	int max_deep = 1;
	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем не стрелки
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		//Пропускаем стрелки, не являющиеся сообщениями создания
		DiagramArrow *arr = static_cast<DiagramArrow *>(items[i]);
		if(!arr->isCreationMessageArrow)
			continue;

		//Пропускаем сообщения создания, которые ведут в текущий объект
		DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
		DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);
		if(target->id == cur_id)
			continue;

		//Пропускаем сообщения, которые не связаны с текущим объектом
		if(source->id != cur_id)
			continue;

		//Остались только сообщения создания, которые ведут в другие объекты
		//Если дошли до этого места, то как минимум одно такое сообщение создания есть

		//Пытаемся улучшить ответ за счет других поддеревьев
		max_deep = max(max_deep, dfs_get_deep_of_tree(target, items) + 1);
	}
	return max_deep;
}

//Смещаем элементы дерева с корнем root на dy вниз
void move_items(DiagramObject *root, vector<QGraphicsItem *> &items, int dy)
{
	set<int> used;	//Создаем множество использованных сообщений

	//Сдвигаем
	dfs_move_items(root, items, dy, used);
}

//Смещает элементы дерева с корнем root на dy вниз (используется в move_items)
void dfs_move_items(DiagramObject *root, vector<QGraphicsItem *> &items, int dy, set<int> &used)
{
	int cur_id = root->id;

	for(int i = 0; i < items.size(); i++)
	{
		//Пропускаем не стрелки
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		//Пропускаем сообщения создания, которые ведут в текущий объект
		DiagramArrow *arr = static_cast<DiagramArrow *>(items[i]);
		DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
		DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);
		if(arr->isCreationMessageArrow && target->id == root->id)
			continue;

		//Пропускаем сообщения, которые не относятся к текущему объекту
		if(source->id != root->id && target->id != root->id)
			continue;

		//Пропускаем сообщения, которые уже были использованы
		if(used.count(arr->id))
			continue;

		//Остались только все синхронные сообщения и сообщения создания, которые ведут в другие объекты

		//Вычисляем текущую высоту
		int cur_arrow_height = arr->getVerticalSpace();

		//Вычисляем нижнюю границу стрелки
		int cur_arrow_low_height = cur_arrow_height + arr->getHeight();

		//Вычисляем позицию, на которой окажется нижняя граница после смещения
		int new_cur_arrow_low_height = cur_arrow_low_height + dy;

		//Если текущая стрелка является сообщением создания, то корректируем ее новую нижнюю границу согласно глубине поддерева
		if(arr->isCreationMessageArrow)
		{
			//Вычисляем максимальную высоту снизу до верхней границы стрелки
			int possible_high_height = dfs_get_possible_height(target, items);

			//Корректируем позицию
			new_cur_arrow_low_height = min(scene_height - possible_high_height - 7, new_cur_arrow_low_height);
		}

		//Корректируем позицию с границей рабочей области
		new_cur_arrow_low_height = min(scene_height, new_cur_arrow_low_height);

		//Вычисляем реальное смещение для стрелки
		int real_dy = new_cur_arrow_low_height - cur_arrow_low_height;

		//Вычисляем новую позицию для стрелки
		int new_pos_of_cur_arrow = cur_arrow_height + real_dy;

		//Смещаем
		arr->setVerticalSpace(new_pos_of_cur_arrow);

		//Корректируем позицию target
		if(arr->isCreationMessageArrow)
			target->setVerticalSpace(new_pos_of_cur_arrow + arr->getHeight() / 2 - target->getHeight() / 2);

		//Запоминаем текущую стрелку
		used.insert(arr->id);

		//Если сообщение создания - то переходим по нему дальше
		if(arr->isCreationMessageArrow)
			dfs_move_items(target, items, dy, used);
	}
}