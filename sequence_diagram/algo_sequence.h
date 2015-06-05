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

//ѕоиск минимального рассто€ни€ от нижней границы рабочего пространства дл€ сообщени€ создани€, которое будет подводитьс€ к root
int dfs_get_possible_height(DiagramObject *root, vector<QGraphicsItem *> &items);

//ѕоиск глубины дерева
int dfs_get_deep_of_tree(DiagramObject *root, vector<QGraphicsItem *> &items);

//—мещаем элементы дерева с корнем root на dy вниз
void move_items(DiagramObject *root, vector<QGraphicsItem *> &items, int dy);

//—мещает элементы дерева с корнем root на dy вниз и пропускает элементы с идентификаторами, которые хран€тс€ в used (и туда же записывает пройденные элементы)
void dfs_move_items(DiagramObject *root, vector<QGraphicsItem *> &items, int dy, set<int> &used);

#endif // ALGOSEQUENCE_H
