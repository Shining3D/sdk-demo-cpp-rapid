#ifndef SAVE_H
#define SAVE_H
/*
An interface corresponding to a "Save" button
*/
#include <QDialog>
#include <QByteArray>
namespace Ui {
class save;
}

class save : public QDialog
{
    Q_OBJECT

public:
    explicit save(QWidget *parent = nullptr);
    ~save();
signals:
	void saveSignal(QByteArray);//The mainwindow receive this signal
private slots:
    void on_pushButton_clicked();
	void on_pushButton_Path_clicked();
private:
    Ui::save *ui;
	
};

#endif // SAVE_H
