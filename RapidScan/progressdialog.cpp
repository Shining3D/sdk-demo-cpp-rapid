#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    ui->progressBar->setRange(0, 100);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::onBeginAsync(QString reason)
{
    ui->label_Reason->setText(reason);
    ui->progressBar->setValue(0);
    show();
}

void ProgressDialog::onFinishAsync()
{
    hide();
}

void ProgressDialog::onProgress(int progress)
{
    ui->progressBar->setValue(progress);
}
