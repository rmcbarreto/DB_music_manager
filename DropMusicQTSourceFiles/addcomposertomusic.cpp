#include "addcomposertomusic.h"
#include "ui_addcomposertomusic.h"

AddComposerToMusic::AddComposerToMusic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddComposerToMusic)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Adicionar Compositor...");
    setWindowIcon(QIcon(":/icons/logo.png"));

    AddItemsComboBox();
}

AddComposerToMusic::~AddComposerToMusic()
{
    delete ui;
}

void AddComposerToMusic::AddItemsComboBox()
{
    AddItemComboBoxMusica("Selecione um...", 0);
    AddItemComboBoxCompositor("Selecione um...", 0);

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
    qry.prepare("SELECT artista_id, nome FROM artista WHERE compositor_compositor = true");
    if(!qry.exec()){
        qDebug() << "There are no composers!";
    }
    else{
        int iterate = 1;
        while(qry.next()){
            aux.id = qry.value(0).toInt();
            aux.nome = qry.value(1).toString();
            compositor_.append(aux);

            AddItemComboBoxCompositor(qry.value(1).toString(), iterate);
            iterate++;
        }
    }
}

void AddComposerToMusic::AddItemComboBoxMusica(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxMusica->addItem(name, qv);
}

void AddComposerToMusic::AddItemComboBoxCompositor(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxCompositor->addItem(name, qv);
}

void AddComposerToMusic::on_pushButton_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO compositor_artista_musica(artista_artista_id, musica_musica_id)"
                "VALUES(:CompositorID, :MusicaID)");

    for(int i = 0; i < compositor_.size(); i++){
        if(compositor_[i].nome == ui->comboBoxCompositor->currentText()){
            qry.bindValue(":CompositorID", compositor_[i].id);
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
        qDebug() << "Não foi possivel adicionar o compositor à música! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar o Compositor à Música."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Compositor adicionado com sucesso!"),
                             QMessageBox::Ok);
        accept();
        close();
    }
}
