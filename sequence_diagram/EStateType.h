#pragma once

//��������� ���������
enum EStateType
{
	DEFAULT_STATE,							//�� ���������
	SELECT_PLACE_FOR_OBJECT,				//����� ����� ��� ������ �������
	SELECT_PLACE_FOR_COMMENT,				//����� ����� ��� ���������� �����������
	SELECT_MESSAGE_BEGIN_STATE,				//����� ��������� ���������
	SELECT_MESSAGE_END_STATE,				//����� ���������� ���������
	SELECT_CREATION_MESSAGE_BEGIN_STATE,	//����� ��������� ��������� ��������
	SELECT_CREATION_MESSAGE_END_STATE,		//����� ���������� ��������� ��������
	SELECT_LOOP_RECTANGLE_FIRST_VERTEX,		//����� ������ ������� ������������� ������� ��� �����
	SELECT_LOOP_RECTANGLE_AREA				//��������� ������������� ������� ��� �����
};