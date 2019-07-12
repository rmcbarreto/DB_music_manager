#include "informations.h"
#include "ui_informations.h"

Informations::Informations(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Informations)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - Informações variadas");
    setWindowIcon(QIcon(":/icons/logo.png"));

    view_->setWindowIcon(QIcon(":/icons/logo.png"));
}

Informations::~Informations()
{
    delete ui;
}

void Informations::on_pushButtonGruposAtivos_clicked()
{
    model_->clear();

    QSqlQuery qry;
    qry.prepare("SELECT a.nome "
                "FROM periodoativo pa, artista a "
                "WHERE a.artista_id = pa.artista_artista_id AND pa.data_fim IS NULL");

    if(qry.exec()){
        model_->setQuery(qry);
        view_->setModel(model_);
        view_->show();
        /*
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Query executada com sucesso!"),
                             QMessageBox::Ok);
                             */
    }
    else{
        qDebug() << "Erro - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Problema ao executar a query."),
                             QMessageBox::Ok);
    }
}

void Informations::on_pushButtonEditoraMaisGrupos_clicked()
{
    model_->clear();

    QSqlQuery qry;
    qry.prepare("SELECT e.nome, COUNT(*) as numero_grupos "
                "FROM editora e, artista a, editora_grupomusical eg "
                "WHERE e.nome = eg.editora_nome AND eg.artista_artista_id = artista_id "
                "GROUP BY e.nome HAVING numero_grupos = (SELECT MAX(aux.cont) "
                                                "FROM (SELECT COUNT(*) AS cont "
                                                        "FROM editora e, artista a, editora_grupomusical eg "
                                                        "WHERE e.nome = eg.editora_nome AND eg.artista_artista_id = artista_id "
                                                        "GROUP BY e.nome) aux)");

    if(qry.exec()){
        model_->setQuery(qry);
        view_->setModel(model_);
        view_->show();
    }
    else{
        qDebug() << "Erro - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Problema ao executar a query."),
                             QMessageBox::Ok);
    }
}

void Informations::on_pushButtonMusicasMaisTocadasConcertos_clicked()
{
    model_->clear();

    QSqlQuery qry;
    qry.prepare("SELECT m.nome, COUNT(*) as numero_concertos "
                "FROM musica m, concerto_musica cm, concerto c "
                "WHERE m.musica_id = cm.musica_musica_id AND cm.concerto_nome=c.nome AND cm.concerto_data=c.data "
                "GROUP BY m.musica_id HAVING numero_concertos = (SELECT MAX(aux.cont) "
                                                      "FROM (SELECT COUNT(*) AS cont, m.musica_id "
                                                              "FROM musica m, concerto_musica cm, concerto c "
                                                              "WHERE m.musica_id = cm.musica_musica_id AND cm.concerto_nome = c.nome AND cm.concerto_data = c.data "
                                                              "GROUP BY m.musica_id) aux)");

    if(qry.exec()){
        model_->setQuery(qry);
        view_->setModel(model_);
        view_->show();
    }
    else{
        qDebug() << "Erro - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Problema ao executar a query."),
                             QMessageBox::Ok);
    }
}

void Informations::on_pushButtonUtilizadoresMaisCriticas_clicked()
{
    model_->clear();

    QSqlQuery qry;
    qry.prepare("SELECT nome, COUNT(*) as numero_criticas "
                "FROM critica, utilizador "
                "WHERE utilizador_username = username "
                "GROUP BY utilizador_username HAVING numero_criticas = (SELECT MAX(aux.cont) "
                                                                        "FROM (SELECT COUNT(*) AS cont, utilizador_username "
                                                                            "FROM critica "
                                                                            "GROUP BY utilizador_username) aux)");

    if(qry.exec()){
        model_->setQuery(qry);
        view_->setModel(model_);
        view_->show();
    }
    else{
        qDebug() << "Erro - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Problema ao executar a query."),
                             QMessageBox::Ok);
    }
}

void Informations::on_pushButtonAlbumMediaPontuacao_clicked()
{
    model_->clear();

    QSqlQuery qry;
    qry.prepare("SELECT ROUND(AVG(pontuacao),0) AS Media, a.nome "
                "FROM critica c, album a "
                "WHERE c.album_album_id = a.album_id "
                "GROUP BY album_album_id HAVING Media = (SELECT MIN(aux.cont) "
                                                        "FROM (SELECT ROUND(AVG(pontuacao),0) AS cont "
                                                                "FROM critica "
                                                                "GROUP BY album_album_id) aux) "
                                            "OR Media = (SELECT MAX(aux2.cont) "
                                                        "FROM (SELECT ROUND(AVG(pontuacao),0) AS cont "
                                                                "FROM critica "
                                                                "GROUP BY album_album_id) aux2) "
                "ORDER BY Media DESC");

    if(qry.exec()){
        model_->setQuery(qry);
        view_->setModel(model_);
        view_->show();
    }
    else{
        qDebug() << "Erro - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Problema ao executar a query."),
                             QMessageBox::Ok);
    }
}

void Informations::on_pushButtonUtilizadorMaisMedias_clicked()
{
    model_->clear();

    QSqlQuery qry;
    qry.prepare("SELECT nome, COUNT(*) as Numero "
                "FROM partilha, utilizador "
                "WHERE utilizador_username_partilhado = username "
                "GROUP BY utilizador_username_partilhado HAVING Numero=(SELECT MAX(aux.cont) "
                                                                       "FROM (SELECT COUNT(*) as cont "
                                                                            "FROM partilha p "
                                                                            "GROUP BY p.utilizador_username_partilhado) aux)");

    if(qry.exec()){
        model_->setQuery(qry);
        view_->setModel(model_);
        view_->show();
    }
    else{
        qDebug() << "Erro - " << qry.lastError();
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Problema ao executar a query."),
                             QMessageBox::Ok);
    }
}
