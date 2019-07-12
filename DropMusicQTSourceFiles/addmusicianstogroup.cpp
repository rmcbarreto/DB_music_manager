#include "addmusicianstogroup.h"
#include "ui_addmusicianstogroup.h"

AddMusiciansToGroup::AddMusiciansToGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMusiciansToGroup)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Adicionar Músicas ao Grupo...");
    setWindowIcon(QIcon(":/icons/logo.png"));

    AddItemsComboBox();
}

AddMusiciansToGroup::~AddMusiciansToGroup()
{
    delete ui;
}

void AddMusiciansToGroup::AddItemsComboBox()
{
    AddItemComboBoxGrupo("Selecione um...", 0);
    AddItemComboBoxMusico("Selecione um...", 0);

    artista aux;
    QSqlQuery qry;
    qry.prepare("SELECT artista_id, nome FROM artista WHERE grupomusical_grupomusical = true");
    if(!qry.exec()){
        qDebug() << "There are no groups!";
    }
    else{
        int iterate = 1;
        while(qry.next()){
            aux.id = qry.value(0).toInt();
            aux.nome = qry.value(1).toString();
            grupo_.append(aux);

            AddItemComboBoxGrupo(qry.value(1).toString(), iterate);
            iterate++;
        }
    }

    qry.clear();
    qry.prepare("SELECT artista_id, nome FROM artista WHERE musico_musico = true");
    if(!qry.exec()){
        qDebug() << "There are no musicians!";
    }
    else{
        int iterate = 1;
        while(qry.next()){
            aux.id = qry.value(0).toInt();
            aux.nome = qry.value(1).toString();
            musico_.append(aux);

            AddItemComboBoxMusico(qry.value(1).toString(), iterate);
            iterate++;
        }
    }
}

void AddMusiciansToGroup::AddItemComboBoxGrupo(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxGrupo->addItem(name, qv);
}

void AddMusiciansToGroup::AddItemComboBoxMusico(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxMusico->addItem(name, qv);
}

void AddMusiciansToGroup::on_pushButton_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO grupomusical_musico(artista_id_grupo, artista_id_musico)"
                "VALUES(:GrupoID, :MusicoID)");

    for(int i = 0; i < grupo_.size(); i++){
        if(grupo_[i].nome == ui->comboBoxGrupo->currentText()){
            qry.bindValue(":GrupoID", grupo_[i].id);
            break;
        }
    }
    for(int i = 0; i < musico_.size(); i++){
        if(musico_[i].nome == ui->comboBoxMusico->currentText()){
            qry.bindValue(":MusicoID", musico_[i].id);
            break;
        }
    }

    if(!qry.exec()){
        qDebug() << "Não foi possivel adicionar o musico ao grupo! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar o Musico ao Grupo."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Musico adicionado com sucesso!"),
                             QMessageBox::Ok);
        accept();
        close();
    }
}
