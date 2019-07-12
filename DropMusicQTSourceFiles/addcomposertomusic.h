#ifndef ADDCOMPOSERTOMUSIC_H
#define ADDCOMPOSERTOMUSIC_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class AddComposerToMusic;
}

class AddComposerToMusic : public QDialog
{
    Q_OBJECT

public:
    explicit AddComposerToMusic(QWidget *parent = nullptr);
    ~AddComposerToMusic();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AddComposerToMusic *ui;

    void AddItemsComboBox();
    void AddItemComboBoxMusica(QString name, int id);
    void AddItemComboBoxCompositor(QString name, int id);

    struct geral{
        QString nome;
        int id;
    };

    QList<geral> compositor_;
    QList<geral> musica_;
};

#endif // ADDCOMPOSERTOMUSIC_H
