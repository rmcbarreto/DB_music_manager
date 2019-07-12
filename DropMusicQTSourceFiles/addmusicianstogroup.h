#ifndef ADDMUSICIANSTOGROUP_H
#define ADDMUSICIANSTOGROUP_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class AddMusiciansToGroup;
}

class AddMusiciansToGroup : public QDialog
{
    Q_OBJECT

public:
    explicit AddMusiciansToGroup(QWidget *parent = nullptr);
    ~AddMusiciansToGroup();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AddMusiciansToGroup *ui;
    void AddItemsComboBox();
    void AddItemComboBoxGrupo(QString name, int id);
    void AddItemComboBoxMusico(QString name, int id);

    struct artista{
        QString nome;
        int id;
    };

    QList<artista> grupo_;
    QList<artista> musico_;
};

#endif // ADDMUSICIANSTOGROUP_H
