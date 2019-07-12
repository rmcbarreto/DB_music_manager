#ifndef ADDMUSICSTOALBUM_H
#define ADDMUSICSTOALBUM_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class AddMusicsToAlbum;
}

class AddMusicsToAlbum : public QDialog
{
    Q_OBJECT

public:
    explicit AddMusicsToAlbum(QWidget *parent = nullptr);
    ~AddMusicsToAlbum();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AddMusicsToAlbum *ui;

    void AddItemsComboBox();
    void AddItemComboBoxMusica(QString name, int id);
    void AddItemComboBoxAlbum(QString name, int id);

    struct geral{
        QString nome;
        int id;
    };

    QList<geral> album_;
    QList<geral> musica_;
};

#endif // ADDMUSICSTOALBUM_H
