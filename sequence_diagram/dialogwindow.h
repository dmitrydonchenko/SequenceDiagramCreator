#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include <QtGui>
#include "ui_TextCommentDialog.h"

//Валидатор для диалогового окна
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

//Класс диалогового окна для редактирования текстовых комментариев
class DialogWindow : public QDialog
{
	Q_OBJECT

public:
	//Констукторы/Деструкторы
	DialogWindow(QString current_text = "", QWidget *parent = 0);
	~DialogWindow();

	//Функции, для работы с окном
	QString getTextComment() const;		//Получить новый текстовый комментарий

private:
	//GUI интерфейс программы
	Ui::TextCommentDialog ui;

	//Валидатор
	DialogWindowValidator valid;

	//Данные
	QString text_comment;

private slots:
	//Обработка введенных данных
	void saveTextComment();
};

#endif // DIALOGWINDOW_H
