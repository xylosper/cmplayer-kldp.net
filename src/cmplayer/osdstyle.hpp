#ifndef OSDSTYLE_HPP
#define OSDSTYLE_HPP

#include <QtGui/QWidget>
#include <QtGui/QTextOption>
#include <QtCore/QPointF>

class QSettings;

class OsdStyle {
public:
//	class Widget;
	enum AutoSize {FitToWidth, FitToHeight, FitToDiagonal};
	OsdStyle();
	void save(QSettings *set, const QString &group) const;
	void load(QSettings *set, const QString &group);
	QFont font;
	QColor color_bg, color_fg, shadow_color;
	double border_width, text_scale;
	QTextOption::WrapMode wrap_mode;
	AutoSize auto_size;
	bool has_shadow;
	QPointF shadow_offset;
	int shadow_blur;
};

//class QLabel;

//class OsdStyle::Widget : public QWidget {
//	Q_OBJECT
//public:
//	Widget(QWidget *parent = 0);
//	~Widget();
//	void setStyle(const OsdStyle &style);
//	const OsdStyle &style() const;
//private slots:
//	void updateFont(const QFont &font);
//	static void setColor(QLabel *label, const QColor &color);
//	void slotFont();
//	void slotColor();
//private:
//	struct Data;
//	Data *d;

//};

#endif // OSDSTYLE_HPP
