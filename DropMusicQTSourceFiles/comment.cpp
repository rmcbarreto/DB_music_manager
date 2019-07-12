#include "comment.h"
#include "ui_comment.h"

comment::comment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::comment)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Crítica");
    setWindowIcon(QIcon(":/icons/logo.png"));

    button_group_->addButton(ui->radioButton0, 0);
    button_group_->addButton(ui->radioButton1, 1);
    button_group_->addButton(ui->radioButton2, 2);
    button_group_->addButton(ui->radioButton3, 3);
    button_group_->addButton(ui->radioButton4, 4);
    button_group_->addButton(ui->radioButton5, 5);

    connect(ui->lineEdit_titulo, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEdits()));
    connect(ui->textEdit_comentario, SIGNAL(textChanged()), this, SLOT(CheckLineEdits()));
    connect(button_group_, SIGNAL(buttonClicked(int)), this, SLOT(CheckLineEdits()));

}

comment::~comment()
{
    delete ui;
}

void comment::SetAlbumID(int album_id)
{
    album_id_ = album_id;
}

void comment::SetUsername(QString username)
{
    username_ = username;
}

void comment::on_pushButton_comentar_clicked()
{
    qDebug() << "Titulo: " << ui->lineEdit_titulo->text() << "; comentario: " << ui->textEdit_comentario->toPlainText();
    qDebug() << "Radio button checked: " << button_group_->checkedId();


    QSqlQuery qry;
    qry.prepare("INSERT INTO critica(titulo, texto, pontuacao, album_album_id, utilizador_username)"
                "VALUES(:Titulo, :Texto, :Pont, :AlbumID, :Username)");
    qry.bindValue(":Titulo", ui->lineEdit_titulo->text());
    qry.bindValue(":Texto", ui->textEdit_comentario->toPlainText());
    qry.bindValue(":Pont", button_group_->checkedId());
    qry.bindValue(":AlbumID", album_id_);
    qry.bindValue(":Username", username_);

    if(!qry.exec()){
        qDebug() << "Adicionar Critica - Não foi possível executar a query";
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar a crítica."),
                             QMessageBox::Ok);

    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Crítica adicionada com sucesso!"),
                             QMessageBox::Ok);

        accept();
    }

    close();
}

void comment::CheckLineEdits()
{
    // Check if all fields are filled, and if so, enables the pushButton
    QString text1 = ui->lineEdit_titulo->text();
    QString text2 = ui->textEdit_comentario->toPlainText();

    bool ok1 = !text1.isEmpty();
    bool ok2 = !text2.isEmpty();
    bool ok3 = false;
    if(button_group_->checkedId() != -1){
         ok3 = true;
    }

    ui->pushButton_comentar->setEnabled(ok1 & (ok2 | ok3));
}

