#include "addmusicstoalbum.h"
#include "ui_addmusicstoalbum.h"

AddMusicsToAlbum::AddMusicsToAlbum(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMusicsToAlbum)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Adicionar Músicas ao Álbum...");
    setWindowIcon(QIcon(":/icons/logo.png"));

    AddItemsComboBox();
}

AddMusicsToAlbum::~AddMusicsToAlbum()
{
    delete ui;
}

void AddMusicsToAlbum::AddItemsComboBox()
{
    AddItemComboBoxMusica("Selecione um...", 0);
    AddItemComboBoxAlbum("Selecione um...", 0);

    geral aux;
    QSqlQuery qry;
    qry.prepare("SELECT musica_id, nome FROM musica");
    if(!qry.exec()){
        qDebug() << "There are no musics!";
    }
    else{
        int iterate = 1;
        while(qry.next()){
            aux.id = qry.value(0).toInt();
            aux.nome = qry.value(1).toString();
            musica_.append(aux);

            AddItemComboBoxMusica(qry.value(1).toString(), iterate);
            iterate++;
        }
    }

    qry.clear();
    qry.prepare("SELECT album_id, nome FROM album");
    if(!qry.exec()){
        qDebug() << "There are no musicians!";
    }
    else{
        int iterate = 1;
        while(qry.next()){
            aux.id = qry.value(0).toInt();
            aux.nome = qry.value(1).toString();
            album_.append(aux);

            AddItemComboBoxAlbum(qry.value(1).toString(), iterate);
            iterate++;
        }
    }
}

void AddMusicsToAlbum::AddItemComboBoxMusica(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxMusica->addItem(name, qv);
}

void AddMusicsToAlbum::AddItemComboBoxAlbum(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxAlbum->addItem(name, qv);
}

void AddMusicsToAlbum::on_pushButton_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO album_musica(album_album_id, musica_musica_id)"
                "VALUES(:AlbumID, :MusicaID)");

    for(int i = 0; i < album_.size(); i++){
        if(album_[i].nome == ui->comboBoxAlbum->currentText()){
            qry.bindValue(":AlbumID", album_[i].id);
            break;
        }
    }
    for(int i = 0; i < musica_.size(); i++){
        if(musica_[i].nome == ui->comboBoxMusica->currentText()){
            qry.bindValue(":MusicaID", musica_[i].id);
            break;
        }
    }

    if(!qry.exec()){
        qDebug() << "Não foi possivel adicionar a musica ao album! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar a Musica ao Album."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Musica adicionado com sucesso!"),
                             QMessageBox::Ok);
        accept();
        close();
    }
}
