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
	NO_SMOOTH = 0, //!< 不平滑
	FLAT = 1, //!< 平面补洞
	TANGENT = 2, //!< 切线补洞
	CURVATURE = 3  //!< 曲率补洞
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
