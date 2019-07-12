#ifndef ADDCONCERT_H
#define ADDCONCERT_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class AddConcert;
}

class AddConcert : public QDialog
{
    Q_OBJECT

public:
    explicit AddConcert(QWidget *parent = nullptr);
    ~AddConcert();

private slots:
    void on_pushButton_clicked();
    void CheckLineEdits();

private:
    Ui::AddConcert *ui;
    void AddItemsComboBox();
    void AddItemComboBox(QString name, int id);

    struct artista{
        QString nome;
        int id;
    };

    QList<artista> grupo_;

};

#endif // ADDCONCERT_H
