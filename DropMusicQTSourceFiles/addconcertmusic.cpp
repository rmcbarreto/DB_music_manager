#include "addconcertmusic.h"
#include "ui_addconcertmusic.h"

AddConcertMusic::AddConcertMusic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddConcertMusic)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Adicionar Compositor...");
    setWindowIcon(QIcon(":/icons/logo.png"));

    AddItemsComboBox();
}

AddConcertMusic::~AddConcertMusic()
{
    delete ui;
}

void AddConcertMusic::AddItemsComboBox()
{
    AddItemComboBoxMusica("Selecione um...", 0);
    AddItemComboBoxConcert("Selecione um...", 0);

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
    qry.prepare("SELECT nome, data FROM concerto");
    if(!qry.exec()){
        qDebug() << "There are no musicians!";
    }
    else{
        int iterate = 1;
        while(qry.next()){
            aux.nome = qry.value(0).toString();
            aux.data = qry.value(1).toDate();
            aux.aux = qry.value(0).toString() + " " + qry.value(1).toString();
            concerto_.append(aux);

            AddItemComboBoxConcert(aux.aux, iterate);
            iterate++;
        }
    }
}

void AddConcertMusic::AddItemComboBoxMusica(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxMusica->addItem(name, qv);
}

void AddConcertMusic::AddItemComboBoxConcert(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxConcerto->addItem(name, qv);
}

void AddConcertMusic::on_pushButton_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO concerto_musica(concerto_nome, concerto_data, musica_musica_id)"
                "VALUES(:Nome, :Data, :MusicaID)");

    for(int i = 0; i < concerto_.size(); i++){
        if(concerto_[i].aux == ui->comboBoxConcerto->currentText()){
            qry.bindValue(":Nome", concerto_[i].nome);
            qry.bindValue(":Data", concerto_[i].data);
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
        qDebug() << "Não foi possivel adicionar a musica ao concerto! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar a Musica ao Concerto."),
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
