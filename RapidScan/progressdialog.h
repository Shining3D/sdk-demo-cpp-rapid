#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
/*
An interface corresponding to a asynchronous progress signal
*/
namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();

public slots:
    void onBeginAsync(QString reason);//Begin  asynchronous signals process
    void onFinishAsync();//Finish  asynchronous signals process
    void onProgress(int progress);//Display progress information
private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
