#pragma once

#ifndef ALGOSEQUENCE_H
#define ALGOSEQUENCE_H

#include <QtGui>
#include <iostream>
#include <set>
#include <algorithm>
#include <vector>

#include "DiagramEditorParam.h"

using namespace std;

class DiagramObject;

//����� ������������ ���������� �� ������ ������� �������� ������������ ��� ��������� ��������, ������� ����� ����������� � root
int dfs_get_possible_height(DiagramObject *root, vector<QGraphicsItem *> &items);

//����� ������� ������
int dfs_get_deep_of_tree(DiagramObject *root, vector<QGraphicsItem *> &items);

//������� �������� ������ � ������ root �� dy ����
void move_items(DiagramObject *root, vector<QGraphicsItem *> &items, int dy);

//������� �������� ������ � ������ root �� dy ���� � ���������� �������� � ����������������, ������� �������� � used (� ���� �� ���������� ���������� ��������)
void dfs_move_items(DiagramObject *root, vector<QGraphicsItem *> &items, int dy, set<int> &used);

#endif // ALGOSEQUENCE_H
