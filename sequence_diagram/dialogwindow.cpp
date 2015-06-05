#include "dialogwindow.h"

DialogWindow::DialogWindow(QString current_text, QWidget *parent)
{
	//������ GUI ��������� ���������
	ui.setupUi(this);

	//������ ���������
	ui.textcomment->setValidator(&valid);

	//������� ������ �������
	setWindowFlags (windowFlags() & ~Qt::WindowContextHelpButtonHint);

	//������������� ������� ��������� ��������
	ui.textcomment->setText(current_text);

	//������������� ��������� ���� � ����������� �� �������� ������������
	connect(ui.textcomment, SIGNAL(editingFinished()), this, SLOT(saveTextComment()));
}

DialogWindow::~DialogWindow()
{
}

//��������� ��������� ������
void DialogWindow::saveTextComment()
{
	text_comment = ui.textcomment->text();
}

//�������� ����� ��������� �����������
QString DialogWindow::getTextComment() const
{
	return text_comment;
}