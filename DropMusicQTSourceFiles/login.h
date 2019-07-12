#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class LogIn;
}

class LogIn : public QDialog
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = nullptr);
    ~LogIn();
    QString GetUsername();
    QString GetNome();
    bool GetEditor();

private slots:
    void on_pushButtonRegistar_clicked();
    void on_pushButtonEntrar_clicked();

    void CheckLineEditsEntrar();
    void CheckLineEditsRegistar();

private:
    Ui::LogIn *ui;
    QString username_;
    QString nome_;
    bool editor_;
};

#endif // LOGIN_H
