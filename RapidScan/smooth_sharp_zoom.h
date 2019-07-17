#ifndef SMOOTH_SHARP_ZOOM_H
#define SMOOTH_SHARP_ZOOM_H

#include <QDialog>
// #include <QMetaType>
// #include <qobjectdefs.h>

// enum SmoothSharpZoomType
// {
// 	Smooth,
// 	Sharp,
// 	Zoom
// };

namespace Ui {
class smooth_sharp_zoom;
}

class smooth_sharp_zoom : public QDialog
{
    Q_OBJECT

public:
    explicit smooth_sharp_zoom(QWidget *parent = nullptr);
    ~smooth_sharp_zoom();
	Ui::smooth_sharp_zoom * getUI() { return ui; }

	explicit smooth_sharp_zoom(smooth_sharp_zoom &pSmoothSharpZoom){}

	//Q_ENUM(SmoothSharpZoomType)

signals:
	void smoothSharpZoomSignal(QByteArray);
private slots:
    void onBtnHandle();
	void on_SmoothBtn_clicked();
	//void on_SharpBtn_clicked();
	//void on_ZoomBtn_clicked();

private:
    Ui::smooth_sharp_zoom *ui;
};

//Q_DECLARE_METATYPE(smooth_sharp_zoom);

#endif // SMOOTH_SHARP_ZOOM_H
