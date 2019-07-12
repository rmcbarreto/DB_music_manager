#ifndef INFORMATIONS_H
#define INFORMATIONS_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>
#include <QTableView>
//#include <QStandardItemModel>

namespace Ui {
class Informations;
}

class Informations : public QDialog
{
    Q_OBJECT

public:
    explicit Informations(QWidget *parent = nullptr);
    ~Informations();

private slots:
    void on_pushButtonGruposAtivos_clicked();

    void on_pushButtonEditoraMaisGrupos_clicked();

    void on_pushButtonMusicasMaisTocadasConcertos_clicked();

    void on_pushButtonUtilizadoresMaisCriticas_clicked();

    void on_pushButtonAlbumMediaPontuacao_clicked();

    void on_pushButtonUtilizadorMaisMedias_clicked();

private:
    Ui::Informations *ui;
    QSqlQueryModel *model_ = new QSqlQueryModel;
    QTableView *view_ = new QTableView;
};

#endif // INFORMATIONS_H
