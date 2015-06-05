#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QtGui>
#include <iostream>
#include <vector>

#include "dialogwindow.h"
#include "EItemsType.h"
#include "DiagramEditorParam.h"
#include "algo_sequence.h"

using namespace std;

/*
����� �������� ��������� (��������� �������� ��������� ��� ���� �������� ���������)
*/

class DiagramItem : public QGraphicsItem
{
public:
	//����� ��������
	int id;					//������������� �������� ���������
	EItemsType item_type;	//��� �������� ���������

	//������������/ �����������
	DiagramItem(QGraphicsItem *parent = 0);
	~DiagramItem();

	//������������ �������, ������� ����� ��������������
	//�������, ������������ ������������� �������, ������� ����� ��������� �������� (Bounding Box)
	virtual QRectF boundingRect() const = 0;
	//�������, ����������� ��������� ��������
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	//�������, ������� ������ ����� �������� �������� � ���������� ��������� �� ���
	virtual DiagramItem* getCopy() const = 0;

	//�������, ������� ���������� �������������� (���� ����� �������������)
	//�������, ������������ ������ ������� ��������
	virtual QPainterPath shape() const;

	//������ ��� ������ � ��������
	void setAllParamFromOtherItem(const DiagramItem * const other);	//�������� ��� ��������� �� ������� ��������
	void setWidth(int w);											//���������� ������ �������
	void setHeight(int h);											//���������� ������ �������
	void setSize(int w, int h);										//���������� ������� �������
	void setPos(qreal x, qreal y);									//���������� ��������� �������
	void setPosX(qreal x);											//���������� ��������� ������� �� ��� X
	void setPosY(qreal y);											//���������� ��������� ������� �� ��� Y
	int getWidth() const;											//�������� ������ �������
	int getHeight() const;											//�������� ������ �������
	int getVerticalSpace() const;									//������ �� ����� ������ ��������� ������� ���������
	QString getTextItemString() const;								//�������� ��������� ����������� ��������
	virtual QByteArray getInfoForBuffer();							//�������� ���������� ��� ������
	virtual void setInfoFromBuffer(QList<QByteArray> list);			//����������� ������ �� ������ ��� �������� ��������
	virtual void setTextComment(QString s);							//���������� ��������� �����������
	virtual void setVerticalSpace(int val);							//������ ������, �� ������� ���������� ������� ���������
	virtual bool saveToFile(QDataStream &stream);					//��������� ������� � ����
	virtual bool loadFromFile(QDataStream &stream);					//��������� ������� �� �����
	bool saveItemType(QDataStream &stream);							//��������� ��� ��������
	bool loadItemType(QDataStream &stream, EItemsType &type);		//��������� ��� ��������

private:

protected:
	//�������� �������
	int width, height;					//�������
	int vertical_space;					//������ (�� �������� ���� ������), �� ������� ���������� ������� ���������

	//������ �������� ���������
	int text_pos_x, text_pos_y;				//������� ���������� �����������
	QString text_item_string;				//��������� �����������

	//������ ��� ������ � ��������
	virtual void updateTextCommentPos(QString s);		//�������� ������� ���������� �����������
};

#endif // DIAGRAMITEM_H
