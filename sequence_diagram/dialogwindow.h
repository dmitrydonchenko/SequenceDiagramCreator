#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include <QtGui>
#include "ui_TextCommentDialog.h"

//��������� ��� ����������� ����
class DialogWindowValidator : public QValidator
{
	QValidator::State validate(QString & input, int & pos) const
	{
		for(int i = 0; i < input.length(); i++)
			if(input[i] == '\t')
				return QValidator::Invalid;
		return QValidator::Acceptable;
	}
};

//����� ����������� ���� ��� �������������� ��������� ������������
class DialogWindow : public QDialog
{
	Q_OBJECT

public:
	//�����������/�����������
	DialogWindow(QString current_text = "", QWidget *parent = 0);
	~DialogWindow();

	//�������, ��� ������ � �����
	QString getTextComment() const;		//�������� ����� ��������� �����������

private:
	//GUI ��������� ���������
	Ui::TextCommentDialog ui;

	//���������
	DialogWindowValidator valid;

	//������
	QString text_comment;

private slots:
	//��������� ��������� ������
	void saveTextComment();
};

#endif // DIALOGWINDOW_H
