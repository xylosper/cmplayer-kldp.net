#ifndef NATIVEVIDEORENDERER_HPP
#define NATIVEVIDEORENDERER_HPP

#include <QtGui/QWidget>
#include <gst/gst.h>
#include <gst/interfaces/navigation.h>

class PlayEngine;	class OsdRenderer;

class NativeVideoRenderer : public QWidget {
	Q_OBJECT
public:
	NativeVideoRenderer(PlayEngine *engine, QWidget *parent = 0);
	~NativeVideoRenderer();
	GstElement *bin() const;
	QSize sizeHint() const;
	void addOsdRenderer(OsdRenderer *osd);
	double frameRate() const;
	double aspectRatio() const;
	double cropRatio() const;
	GstNavigation *nav() const;
public slots:
	void setAspectRatio(double ratio);
	void setCropRatio(double ratio);
signals:
	void frameRateChanged(double frameRate);
private slots:
	void mrlChanged();
	void setXOverlay();
protected:
	void paintEvent(QPaintEvent *event);
	void showEvent(QShowEvent *event);
	void resizeEvent(QResizeEvent *event);
//	void mouseMoveEvent(QMouseEvent *event);
//	void mousePressEvent(QMouseEvent *event);
//	void mouseReleaseEvent(QMouseEvent *event);
private:

	class CropBox;
	static bool isSameRatio(double r1, double r2) {
		return (r1 < 0) ? r2 < 0 : qFuzzyCompare(r1, r2);
	}
	void updateBoxSize();
	friend class GstVideoInfo;
	void setFrameSize(const QSize &size);
	void setFrameRate(double frameRate);
	void updateXOverlayGeometry();
	void windowExposed();
	class XOverlay;
	struct Data;
	Data *d;
};

#endif // NATIVEVIDEORENDERER_HPP
