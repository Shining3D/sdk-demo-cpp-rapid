#ifndef MESH_H
#define MESH_H

#include <QDialog>
#include <QByteArray>

/*
An interface corresponding to a "Mesh" button
*/
namespace Ui {
class mesh;
}

class mesh : public QDialog
{
    Q_OBJECT

public:
    explicit mesh(QWidget *parent = nullptr);
    ~mesh();
signals:
	void meshSignal(QByteArray);//The mainwindow receive this signal contains data 
private slots:
    void on_pushButton_mesh_clicked();
private:
    Ui::mesh *ui;
};

#endif // MESH_H
