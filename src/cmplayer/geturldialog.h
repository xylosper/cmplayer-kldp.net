#ifndef GETURLDIALOG_H
#define GETURLDIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QUrl>

namespace Core {class Playlist;}

class GetUrlDialog : public QDialog {
	Q_OBJECT
public:
	GetUrlDialog(QWidget *parent = 0);
	~GetUrlDialog();
	void setUrl(const QUrl &url);
	QUrl url() const;
	bool isPlaylist() const;
	Core::Playlist playlist() const;
private slots:
	void slotTimeout();
	void slotFinished(int id);
private:
	void accept();
	struct Data;
	Data *d;
};

#endif
