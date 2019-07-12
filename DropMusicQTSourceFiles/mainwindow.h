#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QStandardItemModel>

#include "createplaylist.h"
#include "comment.h"
#include "login.h"
#include "sharemusic.h"
#include "addinformation.h"
#include "addmusicianstogroup.h"
#include "addmusicstoalbum.h"
#include "addcomposertomusic.h"
#include "customqry.h"
#include "addconcert.h"
#include "addconcertmusic.h"
#include "informations.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ErrorDatabaseCloseProgram();

    void on_pushButtonLogIn_clicked();

    void on_tableViewGrupo_clicked(const QModelIndex &index);

    void on_tableViewAlbum_clicked(const QModelIndex &index);

    void DataChangedTableMusic(const QModelIndex& topLeft, const QModelIndex& bottomRight);

    void DataChangedTableUserPlaylist(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    void on_tableViewMusica_clicked(const QModelIndex &index);

    void on_pushButtonAddPlaylist_clicked();

    void on_tableViewPlaylists_clicked(const QModelIndex &index);

    void on_tableViewUserPlaylists_clicked(const QModelIndex &index);

    void on_pushButtonComentar_clicked();

    void on_pushButtonShare_clicked();

    void on_tableViewConcertos_clicked(const QModelIndex &index);

    void SearchBar();

    void on_actionAdicionar_triggered();

    void on_actionAdicionar_Musicos_ao_rupo_triggered();

    void on_actionAdicionar_Musica_ao_Album_triggered();

    void on_actionAdicionar_Compositor_M_sica_triggered();

    void on_actionRemover_Playlists_triggered();

    void on_actionRemover_Partilha_triggered();

    void on_actionSobre_triggered();

    void on_actionExit_triggered();

    void on_actionQuery_Personalizada_triggered();

    void on_actionRefrescar_Base_de_Dados_triggered();

    void on_actionAdicionar_Concerto_triggered();

    void on_actionAdicionar_Musica_ao_Concerto_triggered();

    void on_actionRemover_Concerto_triggered();

    void on_actionInformacoes_triggered();

private:

    Ui::MainWindow *ui;

    bool file_error_ = false;
    void RefreshDatabase();
    void DefaultValues();
    void ShowMusics(bool is_grupo, const QModelIndex &index);
    void ChangeValuesIsLogIn();
    void ActivateUserEditor();
    void RefreshTablePlaylists();
    void RefreshTableUserPlaylists();
    void RefreshTabShared();
    void RefreshTableConcertos();

    bool is_login_ = false;
    QSqlDatabase db_ = QSqlDatabase::addDatabase("QMYSQL");
    QSqlTableModel *modelGrupoMusical_ = new QSqlTableModel;
    QStandardItemModel *modelAlbum_ = new QStandardItemModel;
    QStandardItemModel *modelMusica_ = new QStandardItemModel;
    QSqlTableModel *modelPlaylist_ = new QSqlTableModel;
    QSqlTableModel *modelUserPlaylist_ = new QSqlTableModel;
    QStandardItemModel *modelPlaylistMusica_ = new QStandardItemModel;
    QStandardItemModel *modelUserPlaylistMusica_ = new QStandardItemModel;
    QStandardItemModel *modelUserShared_ = new QStandardItemModel;
    QStandardItemModel *modelUserSharedWithMe_ = new QStandardItemModel;
    QStandardItemModel *modelConcertos_ = new QStandardItemModel;

    QWidget *tabWidgetUserPlaylists;
    QWidget *tabWidgetUserShared;


    struct user{
        QString nome;
        QString username;
        bool is_editor;
    };

    struct user user_;
};

#endif // MAINWINDOW_H
