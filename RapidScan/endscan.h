#ifndef ENDSCAN_H
#define ENDSCAN_H

#include <QDialog>
#include <QByteArray>
/*
An interface corresponding to a "End" button
*/
namespace Ui {
class endScan;
}

class endScan : public QDialog
{
    Q_OBJECT

public:
    explicit endScan(QWidget *parent = nullptr);
    ~endScan();
signals:
	void endScanSignal(QByteArray);//The mainwindow receive this signal contains data
private slots:
    void on_pushButton_endScan_clicked();

private:
    Ui::endScan *ui;
	
};

#endif // ENDSCAN_H
