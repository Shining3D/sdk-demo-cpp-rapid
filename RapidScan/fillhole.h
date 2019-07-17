#ifndef FILLHOLE_H
#define FILLHOLE_H

#include <QDialog>
#include <QLineEdit>
enum FillHoleType
{
	Edit_Fill_Hole,
	Apply_Fill_Hole
};

//fileDataType init value
enum SmoothType
{
	NO_SMOOTH = 0, //!< ��ƽ��
	FLAT = 1, //!< ƽ�油��
	TANGENT = 2, //!< ���߲���
	CURVATURE = 3  //!< ���ʲ���
};

namespace Ui {
class fillHole;
}

class fillHole : public QDialog
{
    Q_OBJECT

public:
    explicit fillHole(QWidget *parent = nullptr);
    ~fillHole();

	QLineEdit* getEditLine();
	QLineEdit* getApplyLine();

	QPushButton* getEditBtn();
	QPushButton* getApplyBtn();

signals:
	void fillHoleSignal(QByteArray, FillHoleType);
private slots:	
    void onBtnHandle(FillHoleType type);
	void on_EditFillHoleBtn_clicked();
	void on_applyFillHoleBtn_clicked();

private:
    Ui::fillHole *ui;
};

#endif // FILLHOLE_H
