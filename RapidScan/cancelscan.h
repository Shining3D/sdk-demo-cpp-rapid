#ifndef CANCELSCAN_H
#define CANCELSCAN_H
/*
An interface corresponding to a "cancel" button
*/
#include <QDialog>
#include <QByteArray>
#include <QVector>
namespace Ui {
class cancelScan;
}

class cancelScan : public QDialog
{
    Q_OBJECT

public:
    explicit cancelScan(QWidget *parent = nullptr);
    ~cancelScan();
signals:
	void cancelScanSignal(QByteArray);//The mainwindow receive this signal contains data 
private slots:
    void on_pushButton_cancelScan_clicked();

private:
    Ui::cancelScan *ui;
};

#endif // CANCELSCAN_H
