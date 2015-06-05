#include "dialogwindow.h"

DialogWindow::DialogWindow(QString current_text, QWidget *parent)
{
	//Задаем GUI интерфейс программе
	ui.setupUi(this);

	//Задаем валидатор
	ui.textcomment->setValidator(&valid);

	//Убираем кнопку вопроса
	setWindowFlags (windowFlags() & ~Qt::WindowContextHelpButtonHint);

	//Устанавливаем текущее текстовое значение
	ui.textcomment->setText(current_text);

	//Устанавливаем поведение окна в зависимости от действий пользователя
	connect(ui.textcomment, SIGNAL(editingFinished()), this, SLOT(saveTextComment()));
}

DialogWindow::~DialogWindow()
{
}

//Обработка введенных данных
void DialogWindow::saveTextComment()
{
	text_comment = ui.textcomment->text();
}

//Получить новый текстовый комментарий
QString DialogWindow::getTextComment() const
{
	return text_comment;
}