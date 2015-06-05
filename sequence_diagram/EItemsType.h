#pragma once

//Типы элементов диаграммы
enum EItemsType
{
	DEFAULT_ITEM,		//Тип элемента по умолчанию
	OBJECT_ITEM,		//Объект диаграммы последовательности
	COMMENT_ITEM,		//Свободный комментарий
	MESSAGE_ITEM,		//Синхронное сообщение
	LOOP_ITEM			// Цикл
};