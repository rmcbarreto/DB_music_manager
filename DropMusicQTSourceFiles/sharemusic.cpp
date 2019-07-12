#include "sharemusic.h"
#include "ui_sharemusic.h"

ShareMusic::ShareMusic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareMusic)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Partilhar");
    setWindowIcon(QIcon(":/icons/logo.png"));

    connect(ui->comboBoxUsers, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckCurrentIndex()));

    AddItemComboBox("Selecione um utilizador...", 0);
}

ShareMusic::~ShareMusic()
{
    delete ui;
}

void ShareMusic::AddItemsComboBox()
{
    aux_user aux;
    QSqlQuery qry;
    qry.prepare("SELECT username, nome FROM utilizador WHERE username <> :Username");
    qry.bindValue(":Username", username_);
    if(!qry.exec()){
        qDebug() << "There are no playlists!";
    }
    else{
        int iterate = 1;
        while(qry.next()){
            aux.username = qry.value(0).toString();
            aux.nome = qry.value(1).toString();
            user_atributes_.append(aux);

            AddItemComboBox(aux.nome, iterate);
            iterate++;
        }
    }
}

void ShareMusic::AddItemComboBox(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxUsers->addItem(name, qv);
}

void ShareMusic::SetUsername(QString username)
{
    username_ = username;
    AddItemsComboBox();
}

void ShareMusic::SetMusicID(int music_id)
{
    music_id_ = music_id;
}

void ShareMusic::on_pushButtonShare_clicked()
{
    QDate data = QDate::currentDate();

    QSqlQuery qry;
    qry.prepare("INSERT INTO partilha(data, utilizador_username_partilhado, utilizador_username_partilhou, musica_musica_id)"
                "VALUES(:Data, :UserPartilhado, :UserPartilhou, :MusicaID)");
    qry.bindValue(":Data", data);
    qry.bindValue(":UserPartilhado", user_atributes_.at(ui->comboBoxUsers->currentIndex()-1).username);
    qry.bindValue(":UserPartilhou", username_);
    qry.bindValue(":MusicaID", music_id_);

    if(!qry.exec()){
        qDebug() << "Partilhar Música - Não foi possível executar a query - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível partilhar a música."),
                             QMessageBox::Ok);

    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Música partilhada com sucesso!"),
                             QMessageBox::Ok);

        accept();
    }

    close();
}

void ShareMusic::CheckCurrentIndex()
{
    if(ui->comboBoxUsers->currentIndex() != 0)
        ui->pushButtonShare->setEnabled(true);
    else
        ui->pushButtonShare->setEnabled(false);
}
