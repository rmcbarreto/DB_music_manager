#ifndef CREATEPLAYLIST_H
#define CREATEPLAYLIST_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class CreatePlaylist;
}

class CreatePlaylist : public QDialog
{
    Q_OBJECT

public:
    explicit CreatePlaylist(QWidget *parent = nullptr);
    ~CreatePlaylist();
    void SetUsername(QString username);
    QString GetPlaylist();

private slots:
    void on_pushButton_CriarPlaylist_clicked();

    void CheckLineEditNomePlaylist();
    void CheckCurrentIndex();

    void on_pushButton_AdicionarMusicas_clicked();

private:
    Ui::CreatePlaylist *ui;
    void AddItemsComboBox();
    void AddItemComboBox(QString name, int id);

    QString username_;
    QString selected_playlist_;
};

#endif // CREATEPLAYLIST_H
