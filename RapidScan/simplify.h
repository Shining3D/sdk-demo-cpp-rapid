#ifndef SIMPLIFY_H
#define SIMPLIFY_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Simplify : public QDialog
{
    Q_OBJECT

public:
	explicit Simplify(QWidget *parent = nullptr);
	~Simplify();

signals:
	void simplifySignal(QByteArray);//The mainwindow receive this signal
private slots:
	void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
};

#endif // SIMPLIFY_H
