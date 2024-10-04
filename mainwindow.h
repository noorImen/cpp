#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include"entreprise.h"
#include"offre.h"
#include <QMainWindow>
#include <QListView>
#include <QStandardItemModel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
struct Notification {
    QString message;   // Message de la notification
    QDateTime date;    // Date et heure de la notification
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ajouter_e_clicked();

    void on_commandLinkButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void  afficherArchives();

    void on_tri_e_clicked();

    void on_recherche_e_clicked();
  void   afficherStatistiques();

    void afficherStatistiquesParCompetences();

    void on_PDF_e_clicked();

  void on_ajouter_e_2_clicked();

    void on_pushButton_3_clicked();

  void on_commandLinkButton_2_clicked();

    void on_pushButton_4_clicked();

  void on_tri_e_2_clicked();

    void on_recherche_e_2_clicked();

  void on_PDF_e_2_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    Entreprise e;
    Offre o;




};
#endif // MAINWINDOW_H
