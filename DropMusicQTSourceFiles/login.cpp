#include "login.h"
#include "ui_login.h"

LogIn::LogIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogIn)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Login/Registo");
    setWindowIcon(QIcon(":/icons/logo.png"));

    // If the line edits are changed is called the CheckLineEdits() function
    connect(ui->lineEditEntrarUsername, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEditsEntrar()));
    connect(ui->lineEditEntrarPassword, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEditsEntrar()));

    connect(ui->lineEditRegistarNome, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEditsRegistar()));
    connect(ui->lineEditRegistarUsername, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEditsRegistar()));
    connect(ui->lineEditRegistarPassword, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEditsRegistar()));
}

LogIn::~LogIn()
{
    delete ui;
}

void LogIn::on_pushButtonRegistar_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO utilizador(username, nome, editor, password)"
                "VALUES(:Username, :Nome, :Editor, :Password)");
    qry.bindValue(":Username", ui->lineEditRegistarUsername->text());
    qry.bindValue(":Nome", ui->lineEditRegistarNome->text());
    qry.bindValue(":Editor", false);
    qry.bindValue(":Password", ui->lineEditRegistarPassword->text());
    if(!qry.exec()){
        qDebug() << "Registo - Não foi possível executar a query";
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Utilizador já existe, tente um novo username."),
                             QMessageBox::Ok);

    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Utilizador adicionado com sucesso!"),
                             QMessageBox::Ok);

        qDebug() << "Registo - query executada com sucesso!";
        username_ = ui->lineEditEntrarUsername->text();
        nome_ = ui->lineEditRegistarNome->text();
        editor_ = false;

        accept();
        close();
    }
}

void LogIn::on_pushButtonEntrar_clicked()
{
    QSqlQuery qry;
    qry.prepare("SELECT nome, editor, password FROM utilizador WHERE username = :user");
    qry.bindValue(":user", ui->lineEditEntrarUsername->text());
    if(!qry.exec()){
        qDebug() << "query nao foi executada com sucesso";
    }
    else{
        if(qry.next()){
            QString nome = qry.value(0).toString(); // Gets the last ID
            bool editor = qry.value(1).toBool(); // Gets the last ID
            QString password = qry.value(2).toString(); // Gets the last ID


            if(ui->lineEditEntrarPassword->text() == password){
                QMessageBox::information(this, tr("Sucesso!"),
                                     tr("Login com sucesso!"),
                                     QMessageBox::Ok);
                username_ = ui->lineEditEntrarUsername->text();
                nome_ = nome;
                editor_ = editor;

                accept();
                close();
            }
            else{
                // Existe um username igual, mas a password está errada
                QMessageBox::warning(this, tr("Erro!"),
                                     tr("Username ou password incorretos. Tente novamente."),
                                     QMessageBox::Ok);
            }

            qDebug() << "nome: " << nome << " - Username: " << ui->lineEditEntrarUsername->text() << " - Password: " << password;
        }
        else{
            // Não existe nenhum username igual
            QMessageBox::warning(this, tr("Erro!"),
                                 tr("Username ou password incorretos. Tente novamente."),
                                 QMessageBox::Ok);

            qDebug() << "nao ha resultados a apresentar";
        }
    }

}

void LogIn::CheckLineEditsEntrar()
{
    // Check if all fields are filled, and if so, enables the pushButton
    QString text2 = ui->lineEditEntrarUsername->text();
    QString text1 = ui->lineEditEntrarPassword->text();

    bool ok1 = !text1.isEmpty();
    bool ok2 = !text2.isEmpty();

    ui->pushButtonEntrar->setEnabled(ok1 & ok2);
}

void LogIn::CheckLineEditsRegistar()
{
    // Check if all fields are filled, and if so, enables the pushButton
    QString text1 = ui->lineEditRegistarNome->text();
    QString text2 = ui->lineEditRegistarUsername->text();
    QString text3 = ui->lineEditRegistarPassword->text();

    bool ok1 = !text1.isEmpty();
    bool ok2 = !text2.isEmpty();
    bool ok3 = !text3.isEmpty();

    ui->pushButtonRegistar->setEnabled(ok1 & ok2 & ok3);
}

QString LogIn::GetUsername()
{
    return username_;
}

QString LogIn::GetNome()
{
    return nome_;
}

bool LogIn::GetEditor()
{
    return editor_;
}
