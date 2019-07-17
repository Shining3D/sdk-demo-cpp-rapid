#ifndef SHARPDLG_H
#define SHARPDLG_H

#include <QDialog>

namespace Ui {
class sharpDlg;
}

class sharpDlg : public QDialog
{
    Q_OBJECT

public:
    explicit sharpDlg(QWidget *parent = nullptr);
    ~sharpDlg();

signals:
	void sharpSignal(QByteArray);
private slots:
	void onBtnHandle();
	void on_SharpBtn_clicked();

private:
    Ui::sharpDlg *ui;
};

#endif // SHARPDLG_H
