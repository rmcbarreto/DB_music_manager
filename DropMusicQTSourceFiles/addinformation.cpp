#include "addinformation.h"
#include "ui_addinformation.h"

AddInformation::AddInformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddInformation)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Adicionar Informações...");
    setWindowIcon(QIcon(":/icons/logo.png"));

    AddItemsComboBox();
}

AddInformation::~AddInformation()
{
    delete ui;
}

void AddInformation::AddItemsComboBox()
{
    grupo aux;
    QSqlQuery qry;
    qry.prepare("SELECT artista_id, nome FROM artista WHERE grupomusical_grupomusical = true");
    if(!qry.exec()){
        qDebug() << "There are no groups!";
    }
    else{
        int iterate = 0;
        while(qry.next()){
            aux.id = qry.value(0).toInt();
            aux.nome = qry.value(1).toString();
            grupo_.append(aux);

            AddItemComboBox(qry.value(1).toString(), iterate);
            iterate++;
        }
    }
}

void AddInformation::AddItemComboBox(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxMusicaAddGrupo->addItem(name, qv);
}

void AddInformation::on_pushButtonGrupoAdd_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO artista(nome, historia, compositor_compositor, musico_musico, grupomusical_grupomusical)"
                "VALUES(:Nome, :Historia, false, false, true)");
    qry.bindValue(":Nome", ui->lineEditGrupoNome->text());
    qry.bindValue(":Historia", ui->textEditGrupoHistoria->toPlainText());
    if(!qry.exec()){
        qDebug() << "Não foi possivel criar um grupo! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível criar um Grupo."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Grupo adicionado com sucesso!"),
                             QMessageBox::Ok);
    }
}

void AddInformation::on_pushButtonMusicaAdd_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO musica(nome, genero, data_lancamento, letra, link, artista_id_grupo)"
                "VALUES(:Nome, :Genero, :Data, :Letra, :Link, :GrupoID)");
    qry.bindValue(":Nome", ui->lineEditMusicaNome->text());
    qry.bindValue(":Genero", ui->lineEditMusicaGenero->text());
    qry.bindValue(":Data", ui->dateEditMusicaDataLancamento->date());
    qry.bindValue(":Letra", ui->textEditMusicaLetra->toPlainText());
    qry.bindValue(":Link", ui->lineEditMusicaLink->text());

    for(int i = 0; i < grupo_.size(); i++){
        if(grupo_[i].nome == ui->comboBoxMusicaAddGrupo->currentText()){
            qry.bindValue(":GrupoID", grupo_[i].id);
            break;
        }
    }

    if(!qry.exec()){
        qDebug() << "Não foi possivel adicionar a musica! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar a Musica."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Musica adicionado com sucesso!"),
                             QMessageBox::Ok);
    }
}

void AddInformation::on_pushButtonMuscioAdd_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO artista(nome, data_nascimento, nacionalidade, compositor_compositor, musico_musico, grupomusical_grupomusical)"
                "VALUES(:Nome, :Data, :Nacionalidade, :ECompositor, true, false)");
    qry.bindValue(":Nome", ui->lineEditMusicoNome->text());
    qry.bindValue(":Data", ui->dateEditMusicoDataNascimento->date());
    qry.bindValue(":Nacionalidade", ui->lineEditMusicoNacionalidade->text());
    qry.bindValue(":ECompositor", ui->checkBoxMusicoECompositor->isChecked());
    if(!qry.exec()){
        qDebug() << "Não foi possivel adicionar um musico! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar um Musico."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Musico adicionado com sucesso!"),
                             QMessageBox::Ok);
    }
}

void AddInformation::on_pushButtonAlbumAdd_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO album(nome, data_lancamento)"
                "VALUES(:Nome, :Data)");
    qry.bindValue(":Nome", ui->lineEditAlbumNome->text());
    qry.bindValue(":Data", ui->dateEditAlbumDataLancamento->date());
    if(!qry.exec()){
        qDebug() << "Não foi possivel adicionar um album! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível criar um Album."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Album criado com sucesso!"),
                             QMessageBox::Ok);
    }
}

void AddInformation::on_pushButtonCompositorAdd_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO artista(nome, data_nascimento, nacionalidade, compositor_compositor, musico_musico, grupomusical_grupomusical)"
                "VALUES(:Nome, :Data, :Nacionalidade, true, false, false)");
    qry.bindValue(":Nome", ui->lineEditCompositorNome->text());
    qry.bindValue(":Data", ui->dateEditCompositorDataNascimento->date());
    qry.bindValue(":Nacionalidade", ui->lineEditCompositorNacionalidade->text());
    if(!qry.exec()){
        qDebug() << "Não foi possivel adicionar um compositor! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar um Compositor."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Compositor adicionado com sucesso!"),
                             QMessageBox::Ok);
    }
}
