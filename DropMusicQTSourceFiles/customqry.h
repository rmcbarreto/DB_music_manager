#ifndef CUSTOMQRY_H
#define CUSTOMQRY_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>
#include <QTableView>
#include <QStandardItemModel>

namespace Ui {
class CustomQry;
}

class CustomQry : public QDialog
{
    Q_OBJECT

public:
    explicit CustomQry(QWidget *parent = nullptr);
    ~CustomQry();

private slots:
    void on_pushButton_clicked();

private:
    Ui::CustomQry *ui;
    QSqlQueryModel *model_ = new QSqlQueryModel;
    QTableView *view_ = new QTableView;
};

#endif // CUSTOMQRY_H
