#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music");
    setWindowIcon(QIcon(":/icons/logo.png"));

    connect(modelMusica_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(DataChangedTableMusic(const QModelIndex&, const QModelIndex&)));
    connect(modelUserPlaylist_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(DataChangedTableUserPlaylist(const QModelIndex&, const QModelIndex&)));
    connect(modelPlaylistMusica_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(DataChangedTableMusic(const QModelIndex&, const QModelIndex&)));
    connect(modelUserPlaylistMusica_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(DataChangedTableMusic(const QModelIndex&, const QModelIndex&)));

    // Gett the enter key from searchBox
    connect(ui->lineEdit_Search, SIGNAL(returnPressed()), this, SLOT(SearchBar()));


    QFile file("db_connect.txt");

    QString line, server, username, password, db_name;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Ficheiro com os dados da base de dados nao foi encontrado";
        file_error_ = true;
        QTimer::singleShot(0, this, SLOT(ErrorDatabaseCloseProgram()));
    }
    else{
        QTextStream stream(&file);
        int count = 0;
        while (!stream.atEnd()){
            line = stream.readLine();
            if(count == 0){ // link do servidor mySQL
                //server = line.remove(0, 7);
                int pos = line.indexOf(QChar(':'));
                server = line.mid(pos + 1);
                server.replace(" ", "");
            }
            else if(count == 1){ // Username
                int pos = line.indexOf(QChar(':'));
                username = line.mid(pos + 1);
                username.replace(" ", "");
            }
            else if(count == 2){ // Password
                int pos = line.indexOf(QChar(':'));
                password = line.mid(pos + 1);
                password.replace(" ", "");
            }
            else if(count == 3){ // Database name
                int pos = line.indexOf(QChar(':'));
                db_name = line.mid(pos + 1);
                db_name.replace(" ", "");
            }
            count++;
        }
    }
    file.close();
    qDebug() << "server: " << server << "; username: " << username << "; password: " << password << "; db_name: " << db_name;

    db_.setHostName(server);
    db_.setDatabaseName(db_name);
    db_.setUserName(username);
    db_.setPassword(password);

    RefreshDatabase();

    DefaultValues();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RefreshDatabase()
{
    if(db_.open()){
        qDebug() << "Conecção realizada com sucesso!";
    }
    else{
        qDebug() << "Não foi possível realizar a conecção com a Base de Dados.";
        QTimer::singleShot(0, this, SLOT(ErrorDatabaseCloseProgram()));
    }
}

void MainWindow::ErrorDatabaseCloseProgram()
{
    qDebug() << "cheguei aqui" << file_error_;
    if(file_error_ == true){
        QMessageBox msgBoxErrorDB(QMessageBox::Critical, tr("Erro!"), tr("Houve um problema com a leitura do ficheiro com os dados de acesso à base de dados. O programa vai abortar."), QMessageBox::Ok, this);
        msgBoxErrorDB.setWindowIcon(QIcon(":/icons/logo.png"));
        if(msgBoxErrorDB.exec() == QMessageBox::Ok)
            this->close();
    }
    else{
        QMessageBox msgBoxErrorDB(QMessageBox::Critical, tr("Erro!"), tr("Houve um problema com a conexão à base de dados. O programa vai abortar."), QMessageBox::Ok, this);
        msgBoxErrorDB.setWindowIcon(QIcon(":/icons/logo.png"));
        if(msgBoxErrorDB.exec() == QMessageBox::Ok)
            this->close();
    }
}

void MainWindow::DefaultValues()
{
    // Esconde a barra de marcadores
    ui->mainToolBar->hide();

    // Coloca a vista como a primeira TAB
    ui->tabWidget->setCurrentIndex(0);

    tabWidgetUserPlaylists = ui->tabWidget->widget(3); // Vai guardar a tab num widget, para depois ser removido e adicionado quando e feito login e logout
    tabWidgetUserShared = ui->tabWidget->widget(4);

    ui->tableViewGrupo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewAlbum->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewMusica->setSelectionMode(QAbstractItemView::ExtendedSelection); // Poder selecionar várias musicas (por exemplo para adicionar a Playist)
    ui->tableViewPlaylists->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewPlaylistMusicas->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->textEditInformacoesGrupo->setReadOnly(true);
    ui->textEditInformacoesAlbum->setReadOnly(true);
    ui->textBrowserInformacoesMusica->setReadOnly(true);

    // Add Grupos Musicais
    ui->tableViewGrupo->setModel(modelGrupoMusical_);
    // QRY SQL equivalente: "SELECT * FROM grupousical";
    modelGrupoMusical_->setTable("artista");
    modelGrupoMusical_->setFilter("grupomusical_grupomusical = true");
    modelGrupoMusical_->select();
    ui->tableViewGrupo->hideColumn(0); // ID
    ui->tableViewGrupo->hideColumn(2); // Data de nascimento
    ui->tableViewGrupo->hideColumn(3); // Nacionalidade
    ui->tableViewGrupo->hideColumn(5); // Is compositor
    ui->tableViewGrupo->hideColumn(6); // Is Music
    ui->tableViewGrupo->hideColumn(7); // Is Group


    // QRY SQL equivalente: "SELECT * FROM playlist WHERE private = false";
    modelPlaylist_->setTable("playlist");
    modelPlaylist_->setFilter("private = false");
    RefreshTablePlaylists();
    ui->tableViewPlaylists->setModel(modelPlaylist_);
    ui->tableViewPlaylists->hideColumn(1);
    ui->tableViewPlaylists->hideColumn(2);

    user_.is_editor = false;
    ChangeValuesIsLogIn();
    RefreshTableConcertos();
}

void MainWindow::on_pushButtonLogIn_clicked()
{
    if(is_login_){
        // Encerra a sessão
        is_login_ = false;
        ChangeValuesIsLogIn();
        user_.nome = "";
        user_.username = "";
        user_.is_editor = false;
        ChangeValuesIsLogIn();
    }
    else{
        LogIn log_in;
        log_in.setModal(true);
        log_in.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);

        // Checks if the windows was closed properly or not
        if(log_in.exec() == QDialog::Rejected){
            qDebug() << "A janela LogIn foi fechada na cruz!!";
        }
        else{
            // Foi realizado um Login ou Registo
            qDebug() << "A janela LogIn foi fechada com sucesso!";
            user_.nome = log_in.GetNome();
            user_.username = log_in.GetUsername();
            user_.is_editor = log_in.GetEditor();

            is_login_ = true;
            ChangeValuesIsLogIn();
        }
    }
}

void MainWindow::on_tableViewGrupo_clicked(const QModelIndex &index)
{
    if(index.isValid()){
        QSqlQuery qry;
        qry.prepare("SELECT DISTINCT(album_id), a.nome, data_lancamento "
                    "FROM album a, album_musica am, (SELECT musica_id FROM musica WHERE artista_id_grupo = :GrupoID) temp "
                    "WHERE a.album_id = am.album_album_id AND musica_musica_id = temp.musica_id");
        qry.bindValue(":GrupoID", index.siblingAtColumn(0).data().toString());

        if(qry.exec()){
            QStandardItem *item = new QStandardItem;

            modelAlbum_->clear();
            modelAlbum_->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
            modelAlbum_->setHorizontalHeaderItem(1, new QStandardItem(QString("Nome")));
            modelAlbum_->setHorizontalHeaderItem(2, new QStandardItem(QString("Data de Lançamento")));

            modelAlbum_->blockSignals(true); // Bloqueia o sinal

            int a = 0;
            while(qry.next()){
                //qDebug() << "id: " << qry.value(0).toString() << "; nome: " << qry.value(1).toString() << "; data: " << qry.value(2).toString();

                // 'nome' e 'genero'
                for(int i = 0; i < 3; i++){
                    item = new QStandardItem(qry.value(i).toString());
                    modelAlbum_->setItem(a, i, item);
                }
                a++;
            }
            modelAlbum_->blockSignals(false); // Desbloqueia o sinal
            modelAlbum_->dataChanged(QModelIndex(), QModelIndex()); // Como o sinal estava bloqueado, a tabela nao foi atualizada. Logo chamamos a funcao para atualizar, mas como nao queremos que corra o que tem la dentro, enviamos um QModelIndex invalido.

            ui->tableViewAlbum->setModel(modelAlbum_);
            ui->tableViewAlbum->hideColumn(0); // Esconde o ID do Album
        }
        else{
            qDebug() << "Erro ao realizar a listagem dos Albuns do Grupo: " <<  index.siblingAtColumn(1).data().toString();
        }

        // Mostra todas as musicas do Grupo Musical
        ShowMusics(true, index);

        // Ir buscar informacões do grupo e colocar no painel de informacoes
        // Adicionar Historia, Editora e periodo ativo
        ui->textEditInformacoesGrupo->clear();
        ui->textEditInformacoesAlbum->clear();

        ui->textEditInformacoesGrupo->moveCursor(QTextCursor::Start);
        ui->textEditInformacoesGrupo->insertHtml("História: <br>" + index.siblingAtColumn(4).data().toString() + "<p>");

        QString editora = "";
        qry.clear();
        qry.prepare("SELECT editora_nome FROM editora_grupomusical WHERE artista_artista_id = :GrupoID");
        qry.bindValue(":GrupoID", index.siblingAtColumn(0).data().toString());
        if(!qry.exec()){
            qDebug() << "Erro ao realizar a listagem da Editora do grupo: " <<  index.siblingAtColumn(1).data().toString() << " (pode não ter editora)";
        }
        else{
            if(qry.next()){
                editora = qry.value(0).toString();
                ui->textEditInformacoesGrupo->moveCursor(QTextCursor::End);
                ui->textEditInformacoesGrupo->insertHtml("Editora: <br>" + editora + "<p>");
            }
        }

        QString participantes = "";
        ui->textEditInformacoesGrupo->moveCursor(QTextCursor::End);
        ui->textEditInformacoesGrupo->insertHtml("Participantes: <br>");
        qry.clear();
        qry.prepare("SELECT a.nome "
                    "FROM artista a, grupomusical_musico gm "
                    "WHERE gm.artista_id_grupo = :GrupoID AND a.artista_id = gm.artista_id_musico");
        qry.bindValue(":GrupoID", index.siblingAtColumn(0).data().toString());
        if(!qry.exec()){
            qDebug() << "Erro ao realizar a listagem dos participantes do grupo: " <<  index.siblingAtColumn(1).data().toString();
        }
        else{
            while(qry.next()){
                participantes = qry.value(0).toString();
                ui->textEditInformacoesGrupo->moveCursor(QTextCursor::End);
                ui->textEditInformacoesGrupo->insertHtml(participantes + "<br>");
            }
            participantes = "";
            ui->textEditInformacoesGrupo->moveCursor(QTextCursor::End);
            ui->textEditInformacoesGrupo->insertHtml(participantes + "<br>");
        }

        QString periodo_inicio = "";
        QString periodo_fim = "";
        ui->textEditInformacoesGrupo->moveCursor(QTextCursor::End);
        ui->textEditInformacoesGrupo->insertHtml("Periodo Ativo: <br>");
        qry.clear();
        qry.prepare("SELECT data_inicio, data_fim "
                    "FROM periodoativo "
                    "WHERE artista_artista_id = :GrupoID");
        qry.bindValue(":GrupoID", index.siblingAtColumn(0).data().toString());
        if(!qry.exec()){
            qDebug() << "Erro ao realizar a listagem dos periodos ativos do grupo: " <<  index.siblingAtColumn(1).data().toString();
        }
        else{
            while(qry.next()){
                periodo_inicio = qry.value(0).toString();
                periodo_fim = qry.value(1).toString();
                ui->textEditInformacoesGrupo->moveCursor(QTextCursor::End);
                ui->textEditInformacoesGrupo->insertHtml(periodo_inicio + "<br>" + periodo_fim + "<p>");
            }
        }

        ui->textEditInformacoesGrupo->moveCursor(QTextCursor::Start);
    }
}

void MainWindow::on_tableViewAlbum_clicked(const QModelIndex &index)
{
    if(index.isValid()){
        ShowMusics(false, index);

        // Adicionar informacoes dos albuns (criticas)

        ui->textEditInformacoesAlbum->clear();
        ui->textBrowserInformacoesMusica->clear();

        QSqlQuery qry;
        qry.prepare("SELECT titulo, texto, pontuacao FROM critica WHERE album_album_id = :AlbumID");
        qry.bindValue(":AlbumID", index.siblingAtColumn(0).data().toInt());
        if(!qry.exec()){
            qDebug() << "Erro ao realizar a listagem das Críticas do Album: " <<  index.siblingAtColumn(1).data().toString() << " (pode não ter críticas)";
        }
        else{
            while(qry.next()){
                ui->textEditInformacoesAlbum->moveCursor(QTextCursor::End);
                ui->textEditInformacoesAlbum->insertHtml("Título:  " + qry.value(0).toString() + "<br>"
                                                      "Texto: <br> " + qry.value(1).toString() + "<br>"
                                                      "Pontuação(0-5): " + qry.value(2).toString() + "<p>");
            }
        }

        qry.clear();
        qry.prepare("SELECT DISTINCT(m.genero) "
                    "FROM musica m, album_musica am "
                    "WHERE am.album_album_id = :AlbumID AND am.musica_musica_id = m.musica_id");
        qry.bindValue(":AlbumID", index.siblingAtColumn(0).data().toInt());
        if(!qry.exec()){
            qDebug() << "Erro ao realizar a listagem das Críticas do Album: " <<  index.siblingAtColumn(1).data().toString() << " (pode não ter críticas)";
        }
        else{
            ui->textEditInformacoesAlbum->moveCursor(QTextCursor::End);
            ui->textEditInformacoesAlbum->insertHtml("Géneros: <br>");
            while(qry.next()){
                ui->textEditInformacoesAlbum->moveCursor(QTextCursor::End);
                ui->textEditInformacoesAlbum->insertHtml(qry.value(0).toString() + "<br>");
            }
        }
    }
}

void MainWindow::DataChangedTableMusic(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(bottomRight);

    if(topLeft.isValid()){
        if(topLeft.column() == 1) // Nome das musicas "nome"
        {
            QSqlQuery qry;
            qry.prepare("UPDATE musica SET nome = :Nome WHERE musica_id = :ID;");
            qry.bindValue(":Nome", topLeft.data().toString());
            qry.bindValue(":ID", topLeft.siblingAtColumn(0).data().toInt());
            if(!qry.exec())
                qDebug() << "Erro ao realizar o UPDATE da tabela 'musica' no 'nome'" << qry.lastError();
        }
        else if(topLeft.column() == 2) // Género das musicas "genero"
        {
            QSqlQuery qry;
            qry.prepare("UPDATE musica SET genero = :Genero WHERE musica_id = :ID;");
            qry.bindValue(":Genero", topLeft.data().toString());
            qry.bindValue(":ID", topLeft.siblingAtColumn(0).data().toInt());
            if(!qry.exec())
                qDebug() << "Erro ao realizar o UPDATE da tabela 'musica' no 'genero'" << qry.lastError();
        }
        else if(topLeft.column() == 3) // Data das musicas "data_lancamento"
        {
            QSqlQuery qry;
            qry.prepare("UPDATE musica SET data_lancamento = :Data WHERE musica_id = :ID;");
            qry.bindValue(":Data", topLeft.data().toDate());
            qry.bindValue(":ID", topLeft.siblingAtColumn(0).data().toInt());
            if(!qry.exec())
                qDebug() << "Erro ao realizar o UPDATE da tabela 'musica' no 'data_lancamento'" << qry.lastError();
        }
    }
}

void MainWindow::ShowMusics(bool is_grupo, const QModelIndex &index)
{
    QSqlQuery qry;
    if(is_grupo){
        qry.prepare("SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                    "FROM musica m "
                    "WHERE m.artista_id_grupo = :GrupoNome");
        qry.bindValue(":GrupoNome", index.siblingAtColumn(0).data().toString());
    }
    else if(!is_grupo){
        qry.prepare("SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                    "FROM musica m, (SELECT musica_musica_id FROM album_musica WHERE album_album_id = :AlbumID) a "
                    "WHERE m.musica_id = a.musica_musica_id");
        qry.bindValue(":AlbumID", index.siblingAtColumn(0).data().toInt());
    }

    if(qry.exec()){
        //const QSignalBlocker blocker(modelMusica); //  Bloqueia o sinal de modelMusica ate ao fim desta funcao

        QStandardItem *item = new QStandardItem;
        //ui->tableViewMusica->clearSpans();
        modelMusica_->clear();
        modelMusica_->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
        modelMusica_->setHorizontalHeaderItem(1, new QStandardItem(QString("Nome")));
        modelMusica_->setHorizontalHeaderItem(2, new QStandardItem(QString("Género")));
        modelMusica_->setHorizontalHeaderItem(3, new QStandardItem(QString("Data de Lançamento")));
        modelMusica_->setHorizontalHeaderItem(4, new QStandardItem(QString("Link")));
        //modelMusica_->removeRows(0, modelMusica_->rowCount());

        modelMusica_->blockSignals(true); // Bloqueia o sinal

        int a = 0;
        while(qry.next()){
            //qDebug() << "id: " << qry.value(0).toString() << "; nome: " << qry.value(1).toString() << "; genero: " << qry.value(2).toString() << "; data: " << qry.value(3).toString() << "; Link: " << qry.value(4).toString();

            // 'nome' e 'genero'
            for(int i = 0; i < 5; i++){
                item = new QStandardItem(qry.value(i).toString());
                modelMusica_->setItem(a, i, item);
            }
            a++;
        }
        modelMusica_->blockSignals(false); // Desbloqueia o sinal
        modelMusica_->dataChanged(QModelIndex(), QModelIndex()); // Como o sinal estava bloqueado, a tabela nao foi atualizada. Logo chamamos a funcao para atualizar, mas como nao queremos que corra o que tem la dentro, enviamos um QModelIndex invalido.

        ui->tableViewMusica->setModel(modelMusica_);
        ui->tableViewMusica->hideColumn(0); // Esconde o ID da musica
        //ui->tableViewMusica->hideColumn(4); // Esconde o Link da musica
    }
    else{
        if(is_grupo)
            qDebug() << "Erro ao realizar a listagem das musicas do grupo: " <<  index.siblingAtColumn(0).data().toString();
        else
            qDebug() << "Erro ao realizar a listagem das musicas do album: " <<  index.siblingAtColumn(1).data().toString();
    }
}

void MainWindow::on_tableViewMusica_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    QModelIndexList indexList = ui->tableViewMusica->selectionModel()->selectedIndexes();

    /*
    int row;
    foreach (QModelIndex index, indexList) {
        row = index.row();
        qDebug() << "row: " << row << "id: " << index.siblingAtColumn(0).data().toString() << " ; Nome: " << index.siblingAtColumn(1).data().toString();
    }
    */

    // Adicionar informacoes nas musicas (link, concertos)

    ui->textBrowserInformacoesMusica->clear();
    ui->textBrowserInformacoesMusica->setOpenExternalLinks(true);

    //ui->textBrowserInformacoesMusica->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    ui->textBrowserInformacoesMusica->moveCursor(QTextCursor::Start);
    ui->textBrowserInformacoesMusica->insertHtml("Link: <a target=\"_blank\" href=\"" + index.siblingAtColumn(4).data().toString() + "\">" + index.siblingAtColumn(4).data().toString() + "</a> <p>");

    QSqlQuery qry;
    QString concertos = "";
    ui->textBrowserInformacoesMusica->moveCursor(QTextCursor::End);
    ui->textBrowserInformacoesMusica->insertHtml("Concertos: <br>");
    qry.prepare("SELECT concerto_nome "
                "FROM concerto_musica "
                "WHERE musica_musica_id = :MusicaID");
    qry.bindValue(":MusicaID", index.siblingAtColumn(0).data().toString());
    if(!qry.exec()){
        qDebug() << "Erro ao realizar a listagem dos concertos da musica: " <<  index.siblingAtColumn(1).data().toString();
    }
    else{
        while(qry.next()){
            concertos = qry.value(0).toString();
            ui->textBrowserInformacoesMusica->moveCursor(QTextCursor::End);
            ui->textBrowserInformacoesMusica->insertHtml(concertos + "<br>");
        }
        concertos = " ";
        ui->textBrowserInformacoesMusica->moveCursor(QTextCursor::End);
        ui->textBrowserInformacoesMusica->insertHtml(concertos + "<br>");
    }

    qry.clear();
    QString compositores = "";
    ui->textBrowserInformacoesMusica->moveCursor(QTextCursor::End);
    ui->textBrowserInformacoesMusica->insertHtml("Compositores: <br>");
    qry.prepare("SELECT nome "
                "FROM artista a, (SELECT artista_artista_id FROM compositor_artista_musica WHERE musica_musica_id = :MusicaID) aux "
                "WHERE a.artista_id = aux.artista_artista_id");
    qry.bindValue(":MusicaID", index.siblingAtColumn(0).data().toString());
    if(!qry.exec()){
        qDebug() << "Erro ao realizar a listagem dos Compositores da musica: " <<  index.siblingAtColumn(1).data().toString();
    }
    else{
        while(qry.next()){
            compositores = qry.value(0).toString();
            ui->textBrowserInformacoesMusica->moveCursor(QTextCursor::End);
            ui->textBrowserInformacoesMusica->insertHtml(compositores + "<br>");
        }
    }

    ui->textBrowserInformacoesMusica->moveCursor(QTextCursor::Start);

    //ui->textBrowserInformacoesMusica->setTextInteractionFlags(ui->textBrowserInformacoesMusica->textInteractionFlags() | Qt::LinksAccessibleByMouse | Qt::TextBrowserInteraction | Qt::TextEditorInteraction );
}

void MainWindow::ChangeValuesIsLogIn()
{
    if(is_login_){
        ui->pushButtonLogIn->setText("Encerrar Sessão");
        ui->pushButtonAddPlaylist->setEnabled(true);
        ui->pushButtonComentar->setEnabled(true);
        ui->pushButtonShare->setEnabled(true);
        ui->actionRemover_Playlists->setEnabled(true);
        ui->actionRemover_Partilha->setEnabled(true);
        ui->actionAdicionar->setEnabled(true);
        ui->actionAdicionar_Compositor_M_sica->setEnabled(true);
        ui->actionAdicionar_Musica_ao_Album->setEnabled(true);
        ui->actionAdicionar_Musicos_ao_rupo->setEnabled(true);
        ui->actionAdicionar_Concerto->setEnabled(true);
        ui->actionAdicionar_Musica_ao_Concerto->setEnabled(true);
        ui->actionRemover_Concerto->setEnabled(true);
        ui->tabWidget->addTab(tabWidgetUserPlaylists, "Minhas Playlists");
        ui->tabWidget->addTab(tabWidgetUserShared, "Partilhas");
        RefreshTableUserPlaylists();
        RefreshTabShared();
    }
    else{
        ui->pushButtonLogIn->setText("Entrar-Registar");
        ui->pushButtonAddPlaylist->setEnabled(false);
        ui->pushButtonComentar->setEnabled(false);
        ui->pushButtonShare->setEnabled(false);
        ui->actionRemover_Playlists->setEnabled(false);
        ui->actionRemover_Partilha->setEnabled(false);
        ui->actionAdicionar->setEnabled(false);
        ui->actionAdicionar_Compositor_M_sica->setEnabled(false);
        ui->actionAdicionar_Musica_ao_Album->setEnabled(false);
        ui->actionAdicionar_Musicos_ao_rupo->setEnabled(false);
        ui->actionAdicionar_Concerto->setEnabled(false);
        ui->actionAdicionar_Musica_ao_Concerto->setEnabled(false);
        ui->actionRemover_Concerto->setEnabled(false);
        //ui->tabMinhasPlayists->setDisabled(true); // Desativa a tab (pode ser uma solucao, em vez de remover)
        //tabWidgetUserPlaylists = ui->tabWidget->widget(2);
        ui->tabWidget->removeTab(4);
        ui->tabWidget->removeTab(3);
    }
    ActivateUserEditor();
}

void MainWindow::ActivateUserEditor()
{
    if(user_.is_editor){
        ui->tableViewGrupo->setEditTriggers(QTableView::DoubleClicked);
        ui->tableViewAlbum->setEditTriggers(QTableView::DoubleClicked);
        ui->tableViewMusica->setEditTriggers(QTableView::DoubleClicked);
        ui->tableViewPlaylists->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewPlaylistMusicas->setEditTriggers(QTableView::DoubleClicked);
        ui->tableViewConcertos->setEditTriggers(QTableView::DoubleClicked);
        ui->tableViewUserSharedWithMe->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewUserShared->setEditTriggers(QTableView::NoEditTriggers);
    }
    else
    {
        ui->tableViewGrupo->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewAlbum->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewMusica->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewPlaylists->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewPlaylistMusicas->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewConcertos->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewUserSharedWithMe->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableViewUserShared->setEditTriggers(QTableView::NoEditTriggers);
    }
}

void MainWindow::on_pushButtonAddPlaylist_clicked()
{
    CreatePlaylist create_playlist;
    create_playlist.setModal(true);
    create_playlist.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);
    create_playlist.SetUsername(user_.username);

    // Checks if the windows was closed properly or not
    if(create_playlist.exec() == QDialog::Rejected){
        qDebug() << "A janela AddPlaylist foi fechada na cruz!!";
    }
    else{
        // Foi criada ou escolhida uma playlist
        qDebug() << "A janela AddPlaylist foi fechada com sucesso!";
        // Agora tenho de adicionar as musicas selecionadas à playlist
        RefreshTablePlaylists();
        RefreshTableUserPlaylists();

        // Get Selected items in tableMusica
        if(ui->tableViewMusica->currentIndex().isValid()){
            QModelIndexList indexList = ui->tableViewMusica->selectionModel()->selectedIndexes();

            foreach (QModelIndex index, indexList) {
                QSqlQuery qry;
                qry.prepare("INSERT INTO musica_playlist(musica_musica_id, playlist_nome, playlist_utilizador_username) "
                            "VALUES(:MusicaID, :PlaylistNome, :Username)");
                qry.bindValue(":MusicaID", index.siblingAtColumn(0).data().toInt());
                qry.bindValue(":PlaylistNome", create_playlist.GetPlaylist());
                qry.bindValue(":Username", user_.username);
                if(!qry.exec()){
                    qDebug() << "Adicionar Playlist - Não foi possível executar a query para adicionar músicas à playlist - Erro: " << qry.lastError();
                    QMessageBox::warning(this, tr("Erro!"),
                                         tr("Adicionar Playlist - Não foi possível executar a query para adicionar músicas à playlist."),
                                         QMessageBox::Ok);
                    break;
                }
            }
        }
    }
}

void MainWindow::RefreshTablePlaylists()
{
    if(!modelPlaylist_->select()){
        qDebug() << "TablePlaylist - Houve um problema ao realizar a pesquisa a DB";
    }
}

void MainWindow::RefreshTableUserPlaylists()
{
    modelUserPlaylist_->setTable("playlist");
    modelUserPlaylist_->setFilter(QString("utilizador_username = \"%1\"").arg(user_.username));
    if(!modelUserPlaylist_->select()){
        qDebug() << "TableUserPlaylist - Houve um problema ao realizar a pesquisa a DB";
    }
    ui->tableViewUserPlaylists->setModel(modelUserPlaylist_);
    ui->tableViewUserPlaylists->hideColumn(2);
}

void MainWindow::DataChangedTableUserPlaylist(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(bottomRight);
    if(topLeft.isValid()){
        qDebug() << "TableViewUserPlaylist inside data changed!";
        RefreshTablePlaylists();
    }
}

void MainWindow::on_tableViewPlaylists_clicked(const QModelIndex &index)
{
    QSqlQuery qry;
    qry.prepare("SELECT musica_id, nome, genero, data_lancamento "
                "FROM musica, musica_playlist "
                "WHERE musica_id = musica_musica_id AND playlist_nome = :PlaylistNome");
    qry.bindValue(":PlaylistNome", index.siblingAtColumn(0).data().toString());

    if(qry.exec()){
        QStandardItem *item = new QStandardItem;

        modelPlaylistMusica_->clear();
        modelPlaylistMusica_->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
        modelPlaylistMusica_->setHorizontalHeaderItem(1, new QStandardItem(QString("Nome")));
        modelPlaylistMusica_->setHorizontalHeaderItem(2, new QStandardItem(QString("Género")));
        modelPlaylistMusica_->setHorizontalHeaderItem(3, new QStandardItem(QString("Data de Lançamento")));

        modelPlaylistMusica_->blockSignals(true); // Bloqueia o sinal

        int a = 0;
        while(qry.next()){
            //qDebug() << "id: " << qry.value(0).toString() << "; nome: " << qry.value(1).toString() << "; genero: " << qry.value(2).toString() << "; data: " << qry.value(3).toString();

            // 'nome' e 'genero'
            for(int i = 0; i < 4; i++){
                item = new QStandardItem(qry.value(i).toString());
                modelPlaylistMusica_->setItem(a, i, item);
            }
            a++;
        }
        modelPlaylistMusica_->blockSignals(false); // Desbloqueia o sinal
        modelPlaylistMusica_->dataChanged(QModelIndex(), QModelIndex()); // Como o sinal estava bloqueado, a tabela nao foi atualizada. Logo chamamos a funcao para atualizar, mas como nao queremos que corra o que tem la dentro, enviamos um QModelIndex invalido.

        ui->tableViewPlaylistMusicas->setModel(modelPlaylistMusica_);
        ui->tableViewPlaylistMusicas->hideColumn(0); // Esconde o ID da musica
    }
    else{
        qDebug() << "Erro ao realizar a listagem das musicas da playlist: " <<  index.siblingAtColumn(0).data().toString();
    }
}

void MainWindow::on_tableViewUserPlaylists_clicked(const QModelIndex &index)
{
    QSqlQuery qry;
    qry.prepare("SELECT musica_id, nome, genero, data_lancamento "
                "FROM musica, musica_playlist "
                "WHERE musica_id = musica_musica_id AND playlist_nome = :PlaylistNome");
    qry.bindValue(":PlaylistNome", index.siblingAtColumn(0).data().toString());

    if(qry.exec()){
        QStandardItem *item = new QStandardItem;

        modelUserPlaylistMusica_->clear();
        modelUserPlaylistMusica_->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
        modelUserPlaylistMusica_->setHorizontalHeaderItem(1, new QStandardItem(QString("Nome")));
        modelUserPlaylistMusica_->setHorizontalHeaderItem(2, new QStandardItem(QString("Género")));
        modelUserPlaylistMusica_->setHorizontalHeaderItem(3, new QStandardItem(QString("Data de Lançamento")));

        modelUserPlaylistMusica_->blockSignals(true); // Bloqueia o sinal

        int a = 0;
        while(qry.next()){
            //qDebug() << "id: " << qry.value(0).toString() << "; nome: " << qry.value(1).toString() << "; genero: " << qry.value(2).toString() << "; data: " << qry.value(3).toString();

            // 'nome' e 'genero'
            for(int i = 0; i < 4; i++){
                item = new QStandardItem(qry.value(i).toString());
                modelUserPlaylistMusica_->setItem(a, i, item);
            }
            a++;
        }
        modelUserPlaylistMusica_->blockSignals(false); // Desbloqueia o sinal
        modelUserPlaylistMusica_->dataChanged(QModelIndex(), QModelIndex()); // Como o sinal estava bloqueado, a tabela nao foi atualizada. Logo chamamos a funcao para atualizar, mas como nao queremos que corra o que tem la dentro, enviamos um QModelIndex invalido.

        ui->tableViewUserPlaylistMusicas->setModel(modelUserPlaylistMusica_);
        ui->tableViewUserPlaylistMusicas->hideColumn(0); // Esconde o ID da musica
    }
    else{
        qDebug() << "Erro ao realizar a listagem das musicas da playlist do user: " <<  index.siblingAtColumn(0).data().toString();
    }
}

void MainWindow::on_pushButtonComentar_clicked()
{
    if(ui->tableViewAlbum->currentIndex().isValid()){
        QModelIndexList indexList = ui->tableViewAlbum->selectionModel()->selectedIndexes();
        //qDebug() << "id: " << indexList.at(0).siblingAtColumn(0).data().toInt() << "; album: " << indexList.at(0).siblingAtColumn(1).data().toString();

        QSqlQuery qry;
        qry.prepare("SELECT titulo "
                    "FROM critica "
                    "WHERE album_album_id = :AlbumID AND utilizador_username = :Username");
        qry.bindValue(":AlbumID", indexList.at(0).siblingAtColumn(0).data().toInt());
        qry.bindValue(":Username", user_.username);
        if(!qry.exec()){
            qDebug() << "nao executou - erro: " << qry.lastError();
            QMessageBox::warning(this, tr("Erro!"),
                                 tr("Problema ao executar a query."),
                                 QMessageBox::Ok);
        }
        else{
            if(qry.next()){
                QMessageBox::information(this, tr("Atenção!"),
                                     tr("Já realizou uma crítica a este álbum!"),
                                     QMessageBox::Ok);
            }
            else{
                comment create_comment;
                create_comment.setModal(true);
                create_comment.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);
                create_comment.SetAlbumID(indexList.at(0).siblingAtColumn(0).data().toInt());
                create_comment.SetUsername(user_.username);

                // Checks if the windows was closed properly or not
                if(create_comment.exec() == QDialog::Rejected){
                    qDebug() << "A janela Comment foi fechada na cruz!!";
                }
                else{
                    qDebug() << "A janela Comment foi fechada coreetamente!!";
                }
            }
        }
    }
    else{
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Adicionar Crítica - Tem de selecionar um Álbum para poder adicionar uma crítica."),
                             QMessageBox::Ok);
    }
}

void MainWindow::on_pushButtonShare_clicked()
{
    if(ui->tableViewMusica->currentIndex().isValid()){
        QModelIndexList indexList = ui->tableViewMusica->selectionModel()->selectedIndexes();
        //qDebug() << "id: " << indexList.at(0).siblingAtColumn(0).data().toInt() << "; musica: " << indexList.at(0).siblingAtColumn(1).data().toString();


        ShareMusic share_music;
        share_music.setModal(true);
        share_music.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);
        share_music.SetMusicID(indexList.at(0).siblingAtColumn(0).data().toInt());
        share_music.SetUsername(user_.username);

        // Checks if the windows was closed properly or not
        if(share_music.exec() == QDialog::Rejected){
            qDebug() << "A janela Share Music foi fechada na cruz!!";
        }
        else{
            qDebug() << "A janela Share Music foi fechada coreetamente!!";
        }
    }
    else{
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Partilhar Música - Tem de selecionar uma Música para poder partilha-la."),
                             QMessageBox::Ok);
    }
    RefreshTabShared();
}

void MainWindow::RefreshTabShared()
{
    // Tabela das musicas que o user partilhou
    QSqlQuery qry;
    qry.prepare("SELECT p.partilha_id, m.musica_id, m.nome, p.utilizador_username_partilhado, m.link "
                "FROM partilha p, musica m "
                "WHERE p.utilizador_username_partilhou = :Username AND p.musica_musica_id = m.musica_id");
    qry.bindValue(":Username", user_.username);

    if(qry.exec()){
        QStandardItem *item = new QStandardItem;

        modelUserShared_->clear();
        modelUserShared_->setHorizontalHeaderItem(0, new QStandardItem(QString("ID partilha")));
        modelUserShared_->setHorizontalHeaderItem(1, new QStandardItem(QString("ID musica")));
        modelUserShared_->setHorizontalHeaderItem(2, new QStandardItem(QString("Nome")));
        modelUserShared_->setHorizontalHeaderItem(3, new QStandardItem(QString("Utilizador")));
        modelUserShared_->setHorizontalHeaderItem(4, new QStandardItem(QString("Link")));

        modelUserShared_->blockSignals(true); // Bloqueia o sinal

        int a = 0;
        while(qry.next()){
            //qDebug() << "id partilha: " << qry.value(0).toString() << "; id musica: " << qry.value(1).toString() <<  "; nome: " << qry.value(2).toString() << "; Utilizador: " << qry.value(3).toString() << "; link: " << qry.value(4).toString();

            // 'nome' e 'genero'
            for(int i = 0; i < 5; i++){
                item = new QStandardItem(qry.value(i).toString());
                modelUserShared_->setItem(a, i, item);
            }
            a++;
        }
        modelUserShared_->blockSignals(false); // Desbloqueia o sinal
        modelUserShared_->dataChanged(QModelIndex(), QModelIndex()); // Como o sinal estava bloqueado, a tabela nao foi atualizada. Logo chamamos a funcao para atualizar, mas como nao queremos que corra o que tem la dentro, enviamos um QModelIndex invalido.

        ui->tableViewUserShared->setModel(modelUserShared_);
        ui->tableViewUserShared->hideColumn(0); // Esconde o ID da partilha
        ui->tableViewUserShared->hideColumn(1); // Esconde o ID da musica
    }
    else{
        qDebug() << "Erro ao realizar a listagem das partilhas do user: " <<  user_.username;
    }

    // Tabela das musicas partilhadas com o user
    qry.clear();
    qry.prepare("SELECT p.partilha_id, m.musica_id, m.nome, p.utilizador_username_partilhou, m.link "
                "FROM partilha p, musica m "
                "WHERE p.utilizador_username_partilhado = :Username AND p.musica_musica_id = m.musica_id");
    qry.bindValue(":Username", user_.username);

    if(qry.exec()){
        QStandardItem *item = new QStandardItem;

        modelUserSharedWithMe_->clear();
        modelUserSharedWithMe_->setHorizontalHeaderItem(0, new QStandardItem(QString("ID partilha")));
        modelUserSharedWithMe_->setHorizontalHeaderItem(1, new QStandardItem(QString("ID musica")));
        modelUserSharedWithMe_->setHorizontalHeaderItem(2, new QStandardItem(QString("Nome")));
        modelUserSharedWithMe_->setHorizontalHeaderItem(3, new QStandardItem(QString("Utilizador")));
        modelUserSharedWithMe_->setHorizontalHeaderItem(4, new QStandardItem(QString("Link")));

        modelUserSharedWithMe_->blockSignals(true); // Bloqueia o sinal

        int a = 0;
        while(qry.next()){
            //qDebug() << "id partilha: " << qry.value(0).toString() << "; id musica: " << qry.value(1).toString() <<  "; nome: " << qry.value(2).toString() << "; Utilizador: " << qry.value(3).toString() << "; link: " << qry.value(4).toString();

            // 'nome' e 'genero'
            for(int i = 0; i < 5; i++){
                item = new QStandardItem(qry.value(i).toString());
                modelUserSharedWithMe_->setItem(a, i, item);
            }
            a++;
        }
        modelUserSharedWithMe_->blockSignals(false); // Desbloqueia o sinal
        modelUserSharedWithMe_->dataChanged(QModelIndex(), QModelIndex()); // Como o sinal estava bloqueado, a tabela nao foi atualizada. Logo chamamos a funcao para atualizar, mas como nao queremos que corra o que tem la dentro, enviamos um QModelIndex invalido.

        ui->tableViewUserSharedWithMe->setModel(modelUserSharedWithMe_);
        ui->tableViewUserSharedWithMe->hideColumn(0); // Esconde o ID da partilha
        ui->tableViewUserSharedWithMe->hideColumn(1); // Esconde o ID da musica
    }
    else{
        qDebug() << "Erro ao realizar a listagem das partilhas do user: " <<  user_.username;
    }

}

void MainWindow::RefreshTableConcertos()
{
    QSqlQuery qry;
    qry.prepare("SELECT *"
                "FROM concerto ");
    qry.bindValue(":Username", user_.username);

    if(qry.exec()){
        QStandardItem *item = new QStandardItem;

        modelConcertos_->clear();
        modelConcertos_->setHorizontalHeaderItem(0, new QStandardItem(QString("Nome")));
        modelConcertos_->setHorizontalHeaderItem(1, new QStandardItem(QString("Data")));
        modelConcertos_->setHorizontalHeaderItem(2, new QStandardItem(QString("País")));
        modelConcertos_->setHorizontalHeaderItem(3, new QStandardItem(QString("Localidade")));

        modelConcertos_->blockSignals(true); // Bloqueia o sinal

        int a = 0;
        while(qry.next()){
            //qDebug() << "Nome: " << qry.value(0).toString() << "; data: " << qry.value(1).toString() << "; pais: " << qry.value(2).toString() << "; localidade: " << qry.value(3).toString();

            // 'nome' e 'genero'
            for(int i = 0; i < 4; i++){
                item = new QStandardItem(qry.value(i).toString());
                modelConcertos_->setItem(a, i, item);
            }
            a++;
        }
        modelConcertos_->blockSignals(false); // Desbloqueia o sinal
        modelConcertos_->dataChanged(QModelIndex(), QModelIndex()); // Como o sinal estava bloqueado, a tabela nao foi atualizada. Logo chamamos a funcao para atualizar, mas como nao queremos que corra o que tem la dentro, enviamos um QModelIndex invalido.

        ui->tableViewConcertos->setModel(modelConcertos_);
    }
    else{
        qDebug() << "Erro ao realizar a listagem dos concertos";
    }
}

void MainWindow::on_tableViewConcertos_clicked(const QModelIndex &index)
{
    ui->textEditInformacoesConcertos->clear();

    ui->textEditInformacoesConcertos->moveCursor(QTextCursor::Start);
    ui->textEditInformacoesConcertos->insertHtml("Músicos: <br>");

    QSqlQuery qry;
    qry.prepare("SELECT nome "
                "FROM artista a, (SELECT gm.artista_id_musico FROM grupomusical_musico gm, artista_concerto ac WHERE ac.concerto_nome = :ConcertoNome AND ac.concerto_data = :ConcertoData AND ac.artista_artista_id = gm.artista_id_grupo) aux "
                "WHERE a.artista_id = aux.artista_id_musico");
    qry.bindValue(":ConcertoNome", index.siblingAtColumn(0).data().toString());
    qry.bindValue(":ConcertoData", index.siblingAtColumn(1).data().toString());

    QString musicos;
    if(!qry.exec()){
        qDebug() << "Erro ao realizar a listagem dos musicos do concerto: " <<  index.siblingAtColumn(0).data().toString();
    }
    else{
        while(qry.next()){
            musicos = qry.value(0).toString();
            ui->textEditInformacoesConcertos->moveCursor(QTextCursor::End);
            ui->textEditInformacoesConcertos->insertHtml(musicos + "<br>");
        }
        ui->textEditInformacoesConcertos->moveCursor(QTextCursor::End);
        musicos = " ";
        ui->textEditInformacoesConcertos->insertHtml("<br>" + musicos);
    }

    ui->textEditInformacoesConcertos->moveCursor(QTextCursor::End);
    ui->textEditInformacoesConcertos->insertHtml("Musicas tocadas: <br>");

    qry.clear();
    qry.prepare("SELECT nome "
                "FROM musica m, (SELECT musica_musica_id FROM concerto_musica WHERE concerto_nome = :ConcertoNome AND concerto_data = :ConcertoData) aux "
                "WHERE m.musica_id = aux.musica_musica_id");
    qry.bindValue(":ConcertoNome", index.siblingAtColumn(0).data().toString());
    qry.bindValue(":ConcertoData", index.siblingAtColumn(1).data().toString());

    QString musicas;
    if(!qry.exec()){
        qDebug() << "Erro ao realizar a listagem das musicas do concerto: " <<  index.siblingAtColumn(0).data().toString();
    }
    else{
        while(qry.next()){
            musicas = qry.value(0).toString();
            ui->textEditInformacoesConcertos->moveCursor(QTextCursor::End);
            ui->textEditInformacoesConcertos->insertHtml(musicas + "<br>");
        }
    }

    ui->textEditInformacoesConcertos->moveCursor(QTextCursor::Start);
}

void MainWindow::SearchBar()
{
    QSqlQuery qry;

    if(ui->lineEdit_Search->text().count("-", Qt::CaseInsensitive) == 2){
         // Pesquisa por data
        QRegExp rx = QRegExp("\\s*(\\d*)-(\\d*)-(\\d*)\\s*");

        rx.indexIn(ui->lineEdit_Search->text());
        QStringList date_list = rx.capturedTexts();

        qry.prepare(QString("SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m "
                            "WHERE Year(m.data_lancamento) = %1 OR Year(m.data_lancamento) = %2 OR Year(m.data_lancamento) = %3 "
                            "OR MONTH(m.data_lancamento) = %1 OR MONTH(m.data_lancamento) = %2 OR MONTH(m.data_lancamento) = %3 "
                            "OR DAY(m.data_lancamento) = %1 OR DAY(m.data_lancamento) = %2 OR DAY(m.data_lancamento) = %3"
                            ).arg(date_list.at(1), date_list.at(2), date_list.at(3)));

    }
    else if(ui->lineEdit_Search->text().count("-", Qt::CaseInsensitive) == 1){
         // Pesquisa por data
        QRegExp rx = QRegExp("\\s*(\\d*)-(\\d*)\\s*", Qt::CaseInsensitive);

        rx.indexIn(ui->lineEdit_Search->text());
        QStringList date_list = rx.capturedTexts();

        qry.prepare(QString("SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m "
                            "WHERE YEAR(m.data_lancamento) = %1 OR YEAR(m.data_lancamento) = %2 "
                            "OR MONTH(m.data_lancamento) = %1 OR MONTH(m.data_lancamento) = %2 "
                            "OR DAY(m.data_lancamento) = %1 OR DAY(m.data_lancamento) = %2"
                            ).arg(date_list.at(1), date_list.at(2)));

    }
    else if(ui->lineEdit_Search->text().toInt() != 0){
        // Pesquisa por data e por pontuacao do album (vai realizar a media da pontuacao do album, e mostra as musicas que se encaixam conforme a pontuacao escolhida do utilizador)
        qry.prepare(QString("SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m "
                            "WHERE YEAR(m.data_lancamento) = %1 "
                            "OR MONTH(m.data_lancamento) = %1 "
                            "OR DAY(m.data_lancamento) = %1 "
                            "UNION "
                            "SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m, album_musica am, (SELECT ROUND(AVG(pontuacao),0) AS pont, album_album_id FROM critica GROUP BY album_album_id) aux "
                            "WHERE am.album_album_id = aux.album_album_id AND am.musica_musica_id = m.musica_id AND aux.pont = %1"
                            ).arg(ui->lineEdit_Search->text().toInt()));

    }
    else{
        // pesquisar musicas por autores (da banda), compositores, grupos (bandas), albuns, genero, data, pontuacao
        // Essencial autor (compositor?), album, genero, data, pontuacao
        // 1º: Nome - 2º: Género - 3º:album - 4º: compositor - 5º: grupo - 6º: musico

        qry.prepare(QString("SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m "
                            "WHERE m.nome LIKE '%%1%' "
                            "UNION "
                            "SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m "
                            "WHERE m.genero LIKE '%%1%' "
                            "UNION "
                            "SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m, album_musica am, (SELECT album_id FROM album WHERE nome LIKE '%%1%') aux "
                            "WHERE am.album_album_id = aux.album_id AND am.musica_musica_id = m.musica_id "
                            "UNION "
                            "SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m, compositor_artista_musica am, (SELECT artista_id FROM artista WHERE compositor_compositor = true AND nome LIKE '%%1%') aux "
                            "WHERE am.artista_artista_id = aux.artista_id AND am.musica_musica_id = m.musica_id "
                            "UNION "
                            "SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m, (SELECT artista_id FROM artista WHERE grupomusical_grupomusical = true AND nome LIKE '%%1%') aux "
                            "WHERE m.artista_id_grupo = aux.artista_id "
                            "UNION "
                            "SELECT m.musica_id, m.nome, m.genero, m.data_lancamento, m.link "
                            "FROM musica m, (SELECT DISTINCT(artista_id_grupo) FROM grupomusical_musico gm, (SELECT artista_id FROM artista WHERE musico_musico = true AND nome LIKE '%%1%') aux WHERE gm.artista_id_musico = aux.artista_id) aux2 "
                            "WHERE aux2.artista_id_grupo = m.artista_id_grupo "
                            ).arg(ui->lineEdit_Search->text()));
    }


    if(!qry.exec()){
        qDebug() << "Erro ao realizar a pesquisa das musicas";
    }
    else{

        QStandardItem *item = new QStandardItem;
        //ui->tableViewMusica->clearSpans();
        modelMusica_->clear();
        modelMusica_->setHorizontalHeaderItem(0, new QStandardItem(QString("ID")));
        modelMusica_->setHorizontalHeaderItem(1, new QStandardItem(QString("Nome")));
        modelMusica_->setHorizontalHeaderItem(2, new QStandardItem(QString("Género")));
        modelMusica_->setHorizontalHeaderItem(3, new QStandardItem(QString("Data de Lançamento")));
        modelMusica_->setHorizontalHeaderItem(4, new QStandardItem(QString("Link")));
        //modelMusica_->removeRows(0, modelMusica_->rowCount());

        modelMusica_->blockSignals(true); // Bloqueia o sinal

        int a = 0;
        while(qry.next()){
            //qDebug() << "id: " << qry.value(0).toString() << "; nome: " << qry.value(1).toString() << "; genero: " << qry.value(2).toString() << "; data: " << qry.value(3).toString() << "; Link: " << qry.value(4).toString();

            // 'nome' e 'genero'
            for(int i = 0; i < 5; i++){
                item = new QStandardItem(qry.value(i).toString());
                modelMusica_->setItem(a, i, item);
            }
            a++;
        }
        modelMusica_->blockSignals(false); // Desbloqueia o sinal
        modelMusica_->dataChanged(QModelIndex(), QModelIndex()); // Como o sinal estava bloqueado, a tabela nao foi atualizada. Logo chamamos a funcao para atualizar, mas como nao queremos que corra o que tem la dentro, enviamos um QModelIndex invalido.

        ui->tableViewMusica->setModel(modelMusica_);
        ui->tableViewMusica->hideColumn(0); // Esconde o ID da musica
        ui->tableViewMusica->hideColumn(4); // Esconde o Link da musica
    }
}

void MainWindow::on_actionAdicionar_triggered()
{
    AddInformation add_information;
    add_information.setModal(true);
    add_information.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);
    //add_information.SetMusicID(indexList.at(0).siblingAtColumn(0).data().toInt());
    //add_information.SetUsername(user_.username);

    // Checks if the windows was closed properly or not
    if(add_information.exec() == QDialog::Rejected){
        qDebug() << "A janela Add information foi fechada na cruz!!";
    }
    else{
        qDebug() << "A janela Add information foi fechada coreetamente!!";
    }
}

void MainWindow::on_actionAdicionar_Musicos_ao_rupo_triggered()
{
    AddMusiciansToGroup add_musicians_to_group;
    add_musicians_to_group.setModal(true);
    add_musicians_to_group.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);

    // Checks if the windows was closed properly or not
    if(add_musicians_to_group.exec() == QDialog::Rejected){
        qDebug() << "A janela Add musicians to group foi fechada na cruz!!";
    }
    else{
        qDebug() << "A janela Add musicians to group foi fechada coreetamente!!";
    }
}

void MainWindow::on_actionAdicionar_Musica_ao_Album_triggered()
{
    AddMusicsToAlbum add_music_to_album;
    add_music_to_album.setModal(true);
    add_music_to_album.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);

    // Checks if the windows was closed properly or not
    if(add_music_to_album.exec() == QDialog::Rejected){
        qDebug() << "A janela Add music to album foi fechada na cruz!!";
    }
    else{
        qDebug() << "A janela Add music to album foi fechada coreetamente!!";
    }
}

void MainWindow::on_actionAdicionar_Compositor_M_sica_triggered()
{
    AddComposerToMusic add_composer_to_music;
    add_composer_to_music.setModal(true);
    add_composer_to_music.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);

    // Checks if the windows was closed properly or not
    if(add_composer_to_music.exec() == QDialog::Rejected){
        qDebug() << "A janela Add composer to music foi fechada na cruz!!";
    }
    else{
        qDebug() << "A janela Add composer to music foi fechada coreetamente!!";
    }
}

void MainWindow::on_actionRemover_Playlists_triggered()
{
    if(ui->tableViewUserPlaylists->currentIndex().isValid()){
        QModelIndexList indexList = ui->tableViewUserPlaylists->selectionModel()->selectedIndexes();
        //qDebug() << "nome: " << indexList.at(0).siblingAtColumn(0).data().toString() << "; private: " << indexList.at(0).siblingAtColumn(1).data().toString() << "; utilizador username: " << indexList.at(0).siblingAtColumn(2).data().toString();

        bool erro = false;
        QSqlQuery qry;
        qry.prepare("DELETE FROM musica_playlist "
                    "WHERE playlist_nome = :PlaylistNome AND playlist_utilizador_username = :Username");
        qry.bindValue(":PlaylistNome", indexList.at(0).siblingAtColumn(0).data().toString());
        qry.bindValue(":Username", user_.username);
        if(!qry.exec()){
            qDebug() << "Couldn't remove musics from playlist: " << indexList.at(0).siblingAtColumn(0).data().toString() << "; Error: " << qry.lastError();
            erro = true;
        }

        qry.clear();
        qry.prepare("DELETE FROM playlist "
                    "WHERE nome = :PlaylistNome AND utilizador_username = :Username");
        qry.bindValue(":PlaylistNome", indexList.at(0).siblingAtColumn(0).data().toString());
        qry.bindValue(":Username", user_.username);
        if(!qry.exec()){
            qDebug() << "Couldn't remove playlist: " << indexList.at(0).siblingAtColumn(0).data().toString() << "; Error: " << qry.lastError();
            erro = true;
        }

        if(erro){
            QMessageBox::warning(this, tr("Erro!"),
                                 tr("Remover Playlists - Houve um erro na remoção da Base de Dados."),
                                 QMessageBox::Ok);
        }
        else{
            QMessageBox::information(this, tr("Sucesso!"),
                                 tr("Playlist removida com sucesso!"),
                                 QMessageBox::Ok);
        }

    }
    else{
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Remover Playlists - Tem de selecionar uma Playlist sua para poder remove-la."),
                             QMessageBox::Ok);
    }

    RefreshTableUserPlaylists();
    RefreshTablePlaylists();
}

void MainWindow::on_actionRemover_Partilha_triggered()
{
    if(ui->tableViewUserShared->currentIndex().isValid()){
        QModelIndexList indexList = ui->tableViewUserShared->selectionModel()->selectedIndexes();
        //qDebug() << "id partilha: " << indexList.at(0).siblingAtColumn(0).data().toString() << "; id musica: " << indexList.at(0).siblingAtColumn(1).data().toString() <<  "; nome: " << indexList.at(0).siblingAtColumn(2).data().toString() << "; Utilizador: " << indexList.at(0).siblingAtColumn(3).data().toString() << "; link: " << indexList.at(0).siblingAtColumn(4).data().toString();

        QSqlQuery qry;
        qry.prepare("DELETE FROM partilha "
                    "WHERE partilha_id = :PartilhaID");
        qry.bindValue(":PartilhaID", indexList.at(0).siblingAtColumn(0).data().toString());
        if(!qry.exec()){
            qDebug() << "Couldn't remove shared music: " << indexList.at(0).siblingAtColumn(2).data().toString() << "; Error: " << qry.lastError();
            QMessageBox::warning(this, tr("Erro!"),
                                 tr("Remover Partilha - Houve um erro na remoção da Base de Dados."),
                                 QMessageBox::Ok);
        }
        else{
            QMessageBox::information(this, tr("Sucesso!"),
                                 tr("Partilha removida com sucesso!"),
                                 QMessageBox::Ok);
        }

    }
    else{
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Remover Partilha - Tem de selecionar uma Partilha sua para poder remove-la."),
                             QMessageBox::Ok);
    }

    RefreshTabShared();
}

void MainWindow::on_actionSobre_triggered()
{
    QMessageBox msgBoxAcercaDe;
    msgBoxAcercaDe.setWindowTitle(tr("Acerca de"));
    msgBoxAcercaDe.setWindowIcon(QIcon(":/icons/logo.png"));

    //msgBoxAcercaDe.setText("Software Desenvolvido por:");
    //msgBoxAcercaDe.setInformativeText();
    msgBoxAcercaDe.setText("<p align='center'><ul>"
                           "<p>Drop Music"
                           "<p>Este programa foi desenvolvido pelos alunos do MIEEC abaixo indicados,"
                           "no âmbito da cadeira de Base de Dados."
                           "<p>Bruno Oliveira - brunogoliveira8@gmail.com"
                           "<p>Ricardo Barreto - rmcbarreto@gmail.com"
                           "</ul> ");
    msgBoxAcercaDe.setStandardButtons(QMessageBox::Ok);
    if(msgBoxAcercaDe.exec() == QMessageBox::Yes)
    {
        close();
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionQuery_Personalizada_triggered()
{
    CustomQry custom_query;
    //custom_query.setModal(true);

    custom_query.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);
    custom_query.setWindowModality(Qt::WindowModal);

    // Checks if the windows was closed properly or not
    if(custom_query.exec() == QDialog::Rejected){
        qDebug() << "A janela Custom Query foi fechada na cruz!!";
    }
    else{
        qDebug() << "A janela Custom Query foi fechada coreetamente!!";
    }
}

void MainWindow::on_actionRefrescar_Base_de_Dados_triggered()
{
    RefreshDatabase();
}

void MainWindow::on_actionAdicionar_Concerto_triggered()
{
    AddConcert add_concert;
    add_concert.setModal(true);
    add_concert.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);

    // Checks if the windows was closed properly or not
    if(add_concert.exec() == QDialog::Rejected){
        qDebug() << "A janela Add Concert foi fechada na cruz!!";
    }
    else{
        qDebug() << "A janela Add Concert foi fechada coreetamente!!";
    }

    RefreshTableConcertos();
}

void MainWindow::on_actionAdicionar_Musica_ao_Concerto_triggered()
{
    AddConcertMusic add_concert_music;
    add_concert_music.setModal(true);
    add_concert_music.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);

    // Checks if the windows was closed properly or not
    if(add_concert_music.exec() == QDialog::Rejected){
        qDebug() << "A janela Add Concert Music foi fechada na cruz!!";
    }
    else{
        qDebug() << "A janela Add Concert Music foi fechada coreetamente!!";
    }

    RefreshTableConcertos();
}

void MainWindow::on_actionRemover_Concerto_triggered()
{
    if(ui->tableViewConcertos->currentIndex().isValid()){
        QModelIndexList indexList = ui->tableViewConcertos->selectionModel()->selectedIndexes();
        //qDebug() << "nome: " << indexList.at(0).siblingAtColumn(0).data().toString() << "; data: " << indexList.at(0).siblingAtColumn(1).data().toString() <<  "; pais: " << indexList.at(0).siblingAtColumn(2).data().toString() << "; loaclidade: " << indexList.at(0).siblingAtColumn(3).data().toString();

        QSqlQuery qry;
        qry.prepare("DELETE FROM concerto_musica "
                    "WHERE concerto_nome = :Nome AND concerto_data = :Data");
        qry.bindValue(":Nome", indexList.at(0).siblingAtColumn(0).data().toString());
        qry.bindValue(":Data", indexList.at(0).siblingAtColumn(1).data().toDate());
        if(!qry.exec()){
            qDebug() << "Couldn't remove concerto musica: " << indexList.at(0).siblingAtColumn(0).data().toString() << "; Error: " << qry.lastError();
            QMessageBox::warning(this, tr("Erro!"),
                                 tr("Remover Concerto - Houve um erro na remoção das Musicas do Concerto na Base de Dados."),
                                 QMessageBox::Ok);
        }
        else{
            qry.clear();
            qry.prepare("DELETE FROM artista_concerto "
                        "WHERE concerto_nome = :Nome AND concerto_data = :Data");
            qry.bindValue(":Nome", indexList.at(0).siblingAtColumn(0).data().toString());
            qry.bindValue(":Data", indexList.at(0).siblingAtColumn(1).data().toDate());
            if(!qry.exec()){
                qDebug() << "Couldn't remove concerto grupo: " << indexList.at(0).siblingAtColumn(0).data().toString() << "; Error: " << qry.lastError();
                QMessageBox::warning(this, tr("Erro!"),
                                     tr("Remover Concerto - Houve um erro na remoção do Grupo do Concerto na Base de Dados."),
                                     QMessageBox::Ok);
            }
            else{
                qry.clear();
                qry.prepare("DELETE FROM concerto "
                            "WHERE nome = :Nome AND data = :Data");
                qry.bindValue(":Nome", indexList.at(0).siblingAtColumn(0).data().toString());
                qry.bindValue(":Data", indexList.at(0).siblingAtColumn(1).data().toDate());
                if(!qry.exec()){
                    qDebug() << "Couldn't remove concerto: " << indexList.at(0).siblingAtColumn(0).data().toString() << "; Error: " << qry.lastError();
                    QMessageBox::warning(this, tr("Erro!"),
                                         tr("Remover Concerto - Houve um erro na remoção do Concerto na Base de Dados."),
                                         QMessageBox::Ok);
                }
                else{
                    QMessageBox::information(this, tr("Sucesso!"),
                                             tr("Concerto removido com sucesso!"),
                                             QMessageBox::Ok);
                }
            }
        }
    }
    else{
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Remover Concerto - Tem de selecionar um Concerto para poder remove-lo."),
                             QMessageBox::Ok);
    }

    RefreshTableConcertos();
}

void MainWindow::on_actionInformacoes_triggered()
{
    Informations informations;
    //informations.setModal(true);
    informations.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & Qt::WindowCloseButtonHint);
    informations.setWindowModality(Qt::WindowModal);

    // Checks if the windows was closed properly or not
    if(informations.exec() == QDialog::Rejected){
        qDebug() << "A janela Informations foi fechada na cruz!!";
    }
    else{
        qDebug() << "A janela Informations foi fechada coreetamente!!";
    }
}
