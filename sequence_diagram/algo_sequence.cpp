#include "algo_sequence.h"

#include "DiagramArrow.h"

//����� ������������ ���������� �� ������ ������� �������� ������������ �� ������� ������� ������� ��������� ��������, ������� ����� ����������� � root
int dfs_get_possible_height(DiagramObject *root, vector<QGraphicsItem *> &items)
{
	//�������� ������� ������
	int deep = dfs_get_deep_of_tree(root, items);

	int arrow_height = 7;		//������ ������� �����������
	int object_height = 50;		//������ ������� �����������

	return (deep * object_height + (deep + 1) * arrow_height) / 2;
}

//����� ������� ������
int dfs_get_deep_of_tree(DiagramObject *root, vector<QGraphicsItem *> &items)
{
	int cur_id = root->id;
	int max_deep = 1;
	for(int i = 0; i < items.size(); i++)
	{
		//���������� �� �������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		//���������� �������, �� ���������� ����������� ��������
		DiagramArrow *arr = static_cast<DiagramArrow *>(items[i]);
		if(!arr->isCreationMessageArrow)
			continue;

		//���������� ��������� ��������, ������� ����� � ������� ������
		DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
		DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);
		if(target->id == cur_id)
			continue;

		//���������� ���������, ������� �� ������� � ������� ��������
		if(source->id != cur_id)
			continue;

		//�������� ������ ��������� ��������, ������� ����� � ������ �������
		//���� ����� �� ����� �����, �� ��� ������� ���� ����� ��������� �������� ����

		//�������� �������� ����� �� ���� ������ �����������
		max_deep = max(max_deep, dfs_get_deep_of_tree(target, items) + 1);
	}
	return max_deep;
}

//������� �������� ������ � ������ root �� dy ����
void move_items(DiagramObject *root, vector<QGraphicsItem *> &items, int dy)
{
	set<int> used;	//������� ��������� �������������� ���������

	//��������
	dfs_move_items(root, items, dy, used);
}

//������� �������� ������ � ������ root �� dy ���� (������������ � move_items)
void dfs_move_items(DiagramObject *root, vector<QGraphicsItem *> &items, int dy, set<int> &used)
{
	int cur_id = root->id;

	for(int i = 0; i < items.size(); i++)
	{
		//���������� �� �������
		DiagramItem *it = static_cast<DiagramItem *>(items[i]);
		if(it->item_type != EItemsType::MESSAGE_ITEM)
			continue;

		//���������� ��������� ��������, ������� ����� � ������� ������
		DiagramArrow *arr = static_cast<DiagramArrow *>(items[i]);
		DiagramObject *source = static_cast<DiagramObject *>(arr->sourceItem);
		DiagramObject *target = static_cast<DiagramObject *>(arr->targetItem);
		if(arr->isCreationMessageArrow && target->id == root->id)
			continue;

		//���������� ���������, ������� �� ��������� � �������� �������
		if(source->id != root->id && target->id != root->id)
			continue;

		//���������� ���������, ������� ��� ���� ������������
		if(used.count(arr->id))
			continue;

		//�������� ������ ��� ���������� ��������� � ��������� ��������, ������� ����� � ������ �������

		//��������� ������� ������
		int cur_arrow_height = arr->getVerticalSpace();

		//��������� ������ ������� �������
		int cur_arrow_low_height = cur_arrow_height + arr->getHeight();

		//��������� �������, �� ������� �������� ������ ������� ����� ��������
		int new_cur_arrow_low_height = cur_arrow_low_height + dy;

		//���� ������� ������� �������� ���������� ��������, �� ������������ �� ����� ������ ������� �������� ������� ���������
		if(arr->isCreationMessageArrow)
		{
			//��������� ������������ ������ ����� �� ������� ������� �������
			int possible_high_height = dfs_get_possible_height(target, items);

			//������������ �������
			new_cur_arrow_low_height = min(scene_height - possible_high_height - 7, new_cur_arrow_low_height);
		}

		//������������ ������� � �������� ������� �������
		new_cur_arrow_low_height = min(scene_height, new_cur_arrow_low_height);

		//��������� �������� �������� ��� �������
		int real_dy = new_cur_arrow_low_height - cur_arrow_low_height;

		//��������� ����� ������� ��� �������
		int new_pos_of_cur_arrow = cur_arrow_height + real_dy;

		//�������
		arr->setVerticalSpace(new_pos_of_cur_arrow);

		//������������ ������� target
		if(arr->isCreationMessageArrow)
			target->setVerticalSpace(new_pos_of_cur_arrow + arr->getHeight() / 2 - target->getHeight() / 2);

		//���������� ������� �������
		used.insert(arr->id);

		//���� ��������� �������� - �� ��������� �� ���� ������
		if(arr->isCreationMessageArrow)
			dfs_move_items(target, items, dy, used);
	}
}