#ifndef PLAYMENUBAR_H
#define PLAYMENUBAR_H

#include <QWidget>

class PlayMenuBar : public QWidget {
	Q_OBJECT
public:
	PlayMenuBar(QWidget *parent = 0);
	~PlayMenuBar();
	void init(const QList<QWidget *> &tools);
public slots:
	void setTotalTime(qint64 msec);
	void setCurrentTime(qint64 msec);
	void setPlayText(const QString &text);
private:
	struct Data;
	friend struct Data;
	Data *d;
};

#endif