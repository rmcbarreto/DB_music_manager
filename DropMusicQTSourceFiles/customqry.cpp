#include "customqry.h"
#include "ui_customqry.h"

CustomQry::CustomQry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomQry)
{
    ui->setupUi(this);

    setWindowTitle("Drop Music - God Mode");
    setWindowIcon(QIcon(":/icons/logo.png"));

    view_->setWindowIcon(QIcon(":/icons/logo.png"));
}

CustomQry::~CustomQry()
{
    delete ui;
}

void CustomQry::on_pushButton_clicked()
{
    model_->clear();

    // Vai verificar se existe a palavra select
    QRegExp rx = QRegExp("select", Qt::CaseInsensitive);
    rx.indexIn(ui->textEdit->toPlainText());
    QStringList list = rx.capturedTexts();

    QSqlQuery qry;
    qry.prepare(ui->textEdit->toPlainText());

    if(qry.exec()){
        // Se a palavra select existir, entao coloca a tabela num tableview_
        if(list.size() > 0){
            model_->setQuery(qry);
            view_->setModel(model_);
            view_->show();
        }
        QMessageBox::information(this, tr("Sucesso!"),
                             tr("Query executada com sucesso!"),
                             QMessageBox::Ok);
    }
    else{
        QMessageBox::warning(this, tr("Erro!"),
                             tr("Problema ao executar a query."),
                             QMessageBox::Ok);
    }

}
