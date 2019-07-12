#ifndef ADDINFORMATION_H
#define ADDINFORMATION_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class AddInformation;
}

class AddInformation : public QDialog
{
    Q_OBJECT

public:
    explicit AddInformation(QWidget *parent = nullptr);
    ~AddInformation();

private slots:
    void on_pushButtonGrupoAdd_clicked();

    void on_pushButtonMusicaAdd_clicked();

    void on_pushButtonMuscioAdd_clicked();

    void on_pushButtonAlbumAdd_clicked();

    void on_pushButtonCompositorAdd_clicked();

private:
    Ui::AddInformation *ui;

    void AddItemsComboBox();
    void AddItemComboBox(QString name, int id);

    struct grupo{
        QString nome;
        int id;
    };

    QList<grupo> grupo_;
};

#endif // ADDINFORMATION_H
