#ifndef ABREPEATDIALOG_H
#define ABREPEATDIALOG_H

#include <QDialog>

namespace Xine {class ABRepeater;}

class ABRepeatDialog : public QDialog {
	Q_OBJECT
public:
	ABRepeatDialog(Xine::ABRepeater *repeater, QWidget *parent = 0);
	~ABRepeatDialog();
private slots:
	void getAFromTime();
	void getAFromSubtitle();
	void getBFromTime();
	void getBFromSubtitle();
	void start();
private:
	struct Data;
	friend struct Data;
	Data *d;
};

#endif