#include "appstate.hpp"
#include "record.hpp"
#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtCore/QStringList>

AppState::Data::Data()
: keys(TypeMax), values(TypeMax) {
#define INIT(type, init) {keys[type] = #type;values[type] = QVariant(init);}
	INIT(AspectRatio, -1.0);
	INIT(Crop, -1.0);
	INIT(PlaySpeed, 1.0);
//	INIT(AudioRenderer, Map());
	INIT(Volume, 100);
	INIT(Muted, false);
	INIT(Amp, 1.0);
//	INIT(VideoRenderer, Map());
	INIT(SubPos, 1.0);
	INIT(SubSync, 0);
	INIT(LastOpenFile, QString());
//	INIT(ToolBoxRect, QRect());
	INIT(TrayFirst, true);
//	INIT(BackendName, QString());
//	INIT(PanelLayout, QString("OneLine"));
	INIT(VolNorm, true);
	INIT(OpenUrlList, QStringList());
	INIT(UrlEncoding, QString());
#undef INIT
}

AppState::Data AppState::d;

void AppState::save() const {
	Record r;
	r.beginGroup("app-state");
	for (int i=0; i<d.values.size(); ++i)
		r.setValue(d.keys[i], d.values[i]);
	r.endGroup();
}

void AppState::load() {
	Record r;
	r.beginGroup("app-state");
	for (int i=0; i<d.values.size(); ++i)
		d.values[i] = r.value(d.keys[i], d.values[i]);
	r.endGroup();
}
