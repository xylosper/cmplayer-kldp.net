#include "translator.h"
#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtCore/QDir>

struct Translator::Data {
	QTranslator trans;
	QString path;
	QList<QLocale> locale;
};

Translator::Translator()
: d(new Data) {
	qApp->installTranslator(&d->trans);
	d->path = QString::fromLocal8Bit(qgetenv("CMPLAYER_TRANSLATION_PATH"));
	if (d->path.isEmpty())
		d->path = QString(CMPLAYER_TRANSLATION_DIR);
	QDir dir(d->path);
	QStringList file = dir.entryList(QStringList() << "*.qm");
	QRegExp rx("^cmplayer_(.*).qm$");
	d->locale.push_back(QLocale::c());
	for (int i=0; i<file.size(); ++i) {
		if (rx.indexIn(file[i]) == -1)
			continue;
		d->locale.push_back(QLocale(rx.cap(1)));
	}
}

Translator::~Translator() {
	delete d;
}

Translator &Translator::get() {
	static Translator self;
	return self;
}

const LocaleList &Translator::availableLocales() {
	return get().d->locale;
}

bool Translator::load(const QLocale &locale) {
	QString name = locale.name();
	if (locale.language() == QLocale::C)
		name = QLocale::system().name();
	const QString file = "cmplayer_" + name;
	Translator::Data *d = get().d;
	return d->trans.load(file, d->path);
}