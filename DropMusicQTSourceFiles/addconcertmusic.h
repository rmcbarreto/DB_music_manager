#ifndef ADDCONCERTMUSIC_H
#define ADDCONCERTMUSIC_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class AddConcertMusic;
}

class AddConcertMusic : public QDialog
{
    Q_OBJECT

public:
    explicit AddConcertMusic(QWidget *parent = nullptr);
    ~AddConcertMusic();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AddConcertMusic *ui;
    void AddItemsComboBox();
    void AddItemComboBoxMusica(QString name, int id);
    void AddItemComboBoxConcert(QString name, int id);

    struct geral{
        QString nome;
        int id;
        QDate data;
        QString aux;
    };

    QList<geral> concerto_;
    QList<geral> musica_;
};

#endif // ADDCONCERTMUSIC_H
