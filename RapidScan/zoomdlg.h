#ifndef ZOOMDLG_H
#define ZOOMDLG_H

#include <QDialog>

namespace Ui {
class zoomDlg;
}

class zoomDlg : public QDialog
{
    Q_OBJECT

public:
    explicit zoomDlg(QWidget *parent = nullptr);
    ~zoomDlg();

signals:
	void zoomSignal(QByteArray);
private slots:
	void onBtnHandle();
	void on_ZoomBtn_clicked();

private:
    Ui::zoomDlg *ui;
};

#endif // ZOOMDLG_H
