#ifndef SHAREMUSIC_H
#define SHAREMUSIC_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class ShareMusic;
}

class ShareMusic : public QDialog
{
    Q_OBJECT

public:
    explicit ShareMusic(QWidget *parent = nullptr);
    ~ShareMusic();

    void SetUsername(QString username);
    void SetMusicID(int music_id);

private slots:
    void on_pushButtonShare_clicked();
    void CheckCurrentIndex();

private:
    Ui::ShareMusic *ui;
    void AddItemsComboBox();
    void AddItemComboBox(QString name, int id);


    struct aux_user{
        QString username;
        QString nome;
    };

    QString username_;
    int music_id_;
    QList<aux_user> user_atributes_;
};

#endif // SHAREMUSIC_H
