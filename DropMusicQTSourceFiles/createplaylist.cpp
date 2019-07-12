#include "createplaylist.h"
#include "ui_createplaylist.h"

CreatePlaylist::CreatePlaylist(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatePlaylist)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Criar Playlist");
    setWindowIcon(QIcon(":/icons/logo.png"));

    ui->comboBox_Privacidade->addItem("Publica");
    ui->comboBox_Privacidade->addItem("Privada");

    connect(ui->lineEdit_NomePlaylist, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEditNomePlaylist()));
    connect(ui->comboBox_Playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckCurrentIndex()));

    AddItemComboBox("Selecione uma...", 0);

}

CreatePlaylist::~CreatePlaylist()
{
    delete ui;
}

void CreatePlaylist::AddItemsComboBox()
{
    QSqlQuery qry;
    qry.prepare("SELECT nome FROM playlist WHERE utilizador_username = :Username");
    qry.bindValue(":Username", username_);
    if(!qry.exec()){
        qDebug() << "There are no playlists!";
    }
    else{
        int iterate = 1;
        while(qry.next()){
            AddItemComboBox(qry.value(0).toString(), iterate);
            iterate++;
        }
    }
}

void CreatePlaylist::AddItemComboBox(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBox_Playlist->addItem(name, qv);
}

void CreatePlaylist::SetUsername(QString username)
{
    username_ = username;
    AddItemsComboBox();
}

void CreatePlaylist::on_pushButton_CriarPlaylist_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO playlist(nome, private, utilizador_username)"
                "VALUES(:Nome, :Private, :Username)");
    qry.bindValue(":Nome", ui->lineEdit_NomePlaylist->text());
    if(ui->comboBox_Privacidade->currentIndex() == 0){
        qry.bindValue(":Private", false);
        qDebug() << "Publica";
    }
    else{
        qDebug() << "Privada";
        qry.bindValue(":Private", true);
    }
    qry.bindValue(":Username", username_);
    if(!qry.exec()){
        qDebug() << "Criar Playlist - Não foi possível executar a query";
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível criar a Playlist. Tente um novo nome."),
                             QMessageBox::Ok);

    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Playlist adicionada com sucesso!"),
                             QMessageBox::Ok);
        selected_playlist_ = ui->lineEdit_NomePlaylist->text();

        accept();
        close();
    }
}

void CreatePlaylist::on_pushButton_AdicionarMusicas_clicked()
{
    selected_playlist_ = ui->comboBox_Playlist->currentText();

    accept();
    close();
}

QString CreatePlaylist::GetPlaylist()
{
    return selected_playlist_;
}

void CreatePlaylist::CheckLineEditNomePlaylist()
{
    // Check if all fields are filled, and if so, enables the pushButton
    QString text1= ui->lineEdit_NomePlaylist->text();
    bool ok1 = !text1.isEmpty();

    ui->pushButton_CriarPlaylist->setEnabled(ok1);
}

void CreatePlaylist::CheckCurrentIndex()
{
    if(ui->comboBox_Playlist->currentIndex() != 0)
        ui->pushButton_AdicionarMusicas->setEnabled(true);
    else
        ui->pushButton_AdicionarMusicas->setEnabled(false);
}
