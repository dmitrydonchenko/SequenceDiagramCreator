#ifndef GRAPHICSCENE_H
#define GRAPHICSCENE_H

#include <QtGui>
#include <iostream>
#include <vector>

#include "diagramobject.h"
#include "diagramcomment.h"
#include "diagramarrow.h"
#include "diagramloop.h"

#include "EStateType.h"
#include "DiagramEditorParam.h"
#include "algo_sequence.h"

using namespace std;

/*
�����, ����������� ����� ������������ �������������.
*/

class GraphicScene : public QGraphicsScene
{
	Q_OBJECT

public:
	//������������/�����������
	GraphicScene(QObject *parent = 0);
	~GraphicScene();

	//������������ ������ ��� ������ �� ������
	void addItem(QGraphicsItem *item);				//���������� �������� �� �����

	//������ ��� ������ �� ������
	void setNewState(EStateType s);					//���������� ����� ��������� ��� �����
	void removeSelectedItem();						//������� ���������� �������
	void scaleScene(int p);							//��������������� �����
	void changeStateOfLineEnd();					//�������� ��������� ����������� ������� (����������� ����� ����� ��� ���)
	void duplicateSelection();						//������� �������� ���������� ���������
	void cutSelection();							//�������� ���������� �������� � ����� ������
	void copySelection();							//����������� ���������� �������� � ����� ������
	void pasteFromBuffer();							//�������� �������� �� ������ ������
	void moveChildLoop(int dx, int dy, DiagramItem *it, set<int> &used);	

	//������ ��� ������ � �������/������
	void newDiagram();								//�������� ���������
	bool openDiagramFromFile(QString filename);		//������� ��������� �� �����
	bool saveDiagramToFile(QString filename);		//��������� ��������� � ����

	//��������������� ������
	QString getFilename() const;					//�������� ������� ��� �����

private:
	//�����
	QString diagram_filename;			//������� ���� � ����� ���������

	//������ �����
	int last_id;						//��������� �������������� id
	vector<QGraphicsItem *> items;		//�������� ���������, ����������� �� �����
	EStateType current_state;			//������� ��������� �����

	//��� ������ � �������
	QMimeData *mimeData;

	//��������������� ��������
	DiagramArrow *new_arrow;			//��������� �� ���������� �������
	DiagramLoop *new_loop;				//��������� �� ����������� ����

	//��������������� �������� ��� ����������� ���������
	bool is_drag;						//���������� �� ������ �������� �������������� �������� �� ����� ��� ���
	bool is_ctrl;						//������ �� ������� ctrl �� ������ ������ ��� ���
	bool is_shift;						//������ �� ������� shift �� ������ ������ ��� ���
	bool is_duplicate;					//���� �� ����������� ������������ �� ������� �������������� ��� ���
	bool is_move;						//���� �� ��������������

	//��������������� �������� ��� Drag and Drop
	qreal mouse_drag_pos_x, mouse_drag_pos_y;	//������� ������� � ������ ������ �������������� ������� ������������ �����

	//��������������� ������
	void update_lifelines();															//�������� ����� �����
	void update_activity();																//�������� ����������
	void unselectAll();																	//������ ��������� �� ���� ��������
	bool isCreationMessageCanBeCreated(DiagramObject *from, DiagramObject *to);			//����� �� ������� �������� ���� �������
	int getMaxArrowHeight(DiagramObject *obj);											//�������� ������ ��������� �������, ��������� � ��������
	
	//������� �� ���������
	void mousePressEventDefault(QGraphicsSceneMouseEvent *event);								//������� �� ������� ������ ���� � ������� ���������
	void mousePressEventSelectPlaceForObject(QGraphicsSceneMouseEvent *event);					//������� �� ������� ������ ���� � ��������� ���������� �������
	void mousePressEventSelectPlaceForComment(QGraphicsSceneMouseEvent *event);					//������� �� ������� ������ ���� � ��������� ���������� �����������
	void mousePressEventSelectMessageBegin(QGraphicsSceneMouseEvent *event);					//������� �� ������� ������ ���� � ��������� ���������� ������ ����. ���������
	void mousePressEventSelectMessageEnd(QGraphicsSceneMouseEvent *event);						//������� �� ������� ������ ���� � ��������� ���������� ����� ����. ���������
	void mousePressEventSelectCreationMessageBegin(QGraphicsSceneMouseEvent *event);			//������� �� ������� ������ ���� � ��������� ���������� ������ ��������� ��������
	void mousePressEventSelectCreationMessageEnd(QGraphicsSceneMouseEvent *event);				//������� �� ������� ������ ���� � ��������� ���������� ����� ��������� ��������
	void mousePressEventSelectLoopRectangleFirstVertex(QGraphicsSceneMouseEvent *event);		//������� �� ������� ������ ���� � ��������� ������ ������ ������� �������������� �����
	void mouseMoveEventSelectLoopRectangleArea(QGraphicsSceneMouseEvent *event);				//������� �� ������������ ���� � ��������� ��������� ������������� ������� ��� �����
	void mouseReleaseEventSelectLoopRectangleLastVertex(QGraphicsSceneMouseEvent *event);		//������� �� ������� ������ ���� � ��������� ������ ��������� ������� �������������� �����

	//Drag and Drop �������
	void move_object(int dx, int dy, DiagramItem *it, set<int> &used);					//����������� �������
	void move_message(int dx, int dy, DiagramItem *it, set<int> &used);					//����������� ����������� ���������
	void move_creation_message(int dx, int dy, DiagramItem *it, set<int> &used);		//����������� ��������� ��������
	void move_comment(int dx, int dy, DiagramItem *it, set<int> &used);					//����������� ���������� �����������
	void move_loop(int dx, int dy, DiagramItem *it, set<int> &used);					//����������� �����			

protected:
	//������� ����
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);			//������ ������ ����
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);		//������ ������ ����
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);			//����������� ����
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);	//������� ���� ����

	//������� ����������
	virtual void keyPressEvent(QKeyEvent *event);							//������ ������ ����������
	virtual void keyReleaseEvent(QKeyEvent *event);							//������ ������ ����������
};

#endif // GRAPHICSCENE_H
