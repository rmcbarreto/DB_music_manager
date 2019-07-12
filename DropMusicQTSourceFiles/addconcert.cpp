#include "addconcert.h"
#include "ui_addconcert.h"

AddConcert::AddConcert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddConcert)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Adicionar Compositor...");
    setWindowIcon(QIcon(":/icons/logo.png"));

    connect(ui->lineEditNome, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEdits()));
    connect(ui->lineEditPais, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEdits()));
    connect(ui->lineEditLocalidade, SIGNAL(textChanged(const QString &)), this, SLOT(CheckLineEdits()));

    AddItemsComboBox();
}

AddConcert::~AddConcert()
{
    delete ui;
}

void AddConcert::AddItemsComboBox()
{
    AddItemComboBox("Selecione um...", 0);

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

            AddItemComboBox(qry.value(1).toString(), iterate);
            iterate++;
        }
    }
}

void AddConcert::AddItemComboBox(QString name, int id)
{
    QVariant qv;
    qv.setValue(id);
    ui->comboBoxGrupo->addItem(name, qv);
}

void AddConcert::on_pushButton_clicked()
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO concerto(nome, data, pais, localidade)"
                "VALUES(:Nome, :Data, :Pais, :Localidade)");
    qry.bindValue(":Nome", ui->lineEditNome->text());
    qry.bindValue(":Data", ui->dateEdit->date());
    qry.bindValue(":Pais", ui->lineEditPais->text());
    qry.bindValue(":Localidade", ui->lineEditLocalidade->text());

    if(!qry.exec()){
        qDebug() << "Não foi possivel adicionar o concerto! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar o Concerto."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Concerto adicionado com sucesso!"),
                             QMessageBox::Ok);
    }

    qry.clear();
    qry.prepare("INSERT INTO artista_concerto(artista_artista_id, concerto_nome, concerto_data)"
                "VALUES(:GrupoID, :Nome, :Data)");
    for(int i = 0; i < grupo_.size(); i++){
        if(grupo_[i].nome == ui->comboBoxGrupo->currentText()){
            qry.bindValue(":GrupoID", grupo_[i].id);
            break;
        }
    }
    qry.bindValue(":Nome", ui->lineEditNome->text());
    qry.bindValue(":Data", ui->dateEdit->date());

    if(!qry.exec()){
        qDebug() << "Não foi possivel adicionar o grupo ao concerto! - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Não foi possível adicionar o Grupo ao Concerto."),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Grupo adicionado ao Concerto com sucesso!"),
                             QMessageBox::Ok);
        accept();
    }
    close();
}

void AddConcert::CheckLineEdits()
{
    // Check if all fields are filled, and if so, enables the pushButton
    QString text1 = ui->lineEditNome->text();
    QString text2 = ui->lineEditPais->text();
    QString text3 = ui->lineEditLocalidade->text();

    bool ok1 = !text1.isEmpty();
    bool ok2 = !text2.isEmpty();
    bool ok3 = !text3.isEmpty();

    ui->pushButton->setEnabled(ok1 & ok2 & ok3);
}

