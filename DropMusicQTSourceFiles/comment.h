#ifndef COMMENT_H
#define COMMENT_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>
#include <QButtonGroup>

namespace Ui {
class comment;
}

class comment : public QDialog
{
    Q_OBJECT

public:
    explicit comment(QWidget *parent = nullptr);
    ~comment();
    void SetAlbumID(int album_id);
    void SetUsername(QString username);

private slots:
    void on_pushButton_comentar_clicked();
    void CheckLineEdits();

private:
    Ui::comment *ui;

    QButtonGroup *button_group_ = new QButtonGroup;
    int album_id_;
    QString username_;
};

#endif // COMMENT_H
