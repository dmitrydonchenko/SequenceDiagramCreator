#pragma once

//Состояния программы
enum EStateType
{
	DEFAULT_STATE,							//По умолчанию
	SELECT_PLACE_FOR_OBJECT,				//Выбор места для нового объекта
	SELECT_PLACE_FOR_COMMENT,				//Выбор места для свободного комментария
	SELECT_MESSAGE_BEGIN_STATE,				//Выбор источника сообщения
	SELECT_MESSAGE_END_STATE,				//Выбор получателя сообщения
	SELECT_CREATION_MESSAGE_BEGIN_STATE,	//Выбор источника сообщения создания
	SELECT_CREATION_MESSAGE_END_STATE,		//Выбор получателя сообщения создания
	SELECT_LOOP_RECTANGLE_FIRST_VERTEX,		//Выбор первой вершины прямоугольной области для цикла
	SELECT_LOOP_RECTANGLE_AREA				//Выделение прямоугольной области для цикла
};