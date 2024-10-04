#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"entreprise.h"
#include"offre.h"
#include<QMessageBox>
#include<QIntValidator>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QGraphicsScene>
#include <QSqlQuery>
#include <QFileDialog>
#include <QRegularExpression>
#include <QUrl>
#include <QDesktopServices>
#include <QDate>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->affiche_e->setModel(e.afficher());
    ui->affiche_o->setModel(o.afficher());
    ui->his->setModel(o.afficherHistorique());

    afficherArchives();
    afficherStatistiques();
    afficherStatistiquesParCompetences();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ajouter_e_clicked() {
    // Récupérer les valeurs des champs
    QString nom = ui->nom_e->text();
    QString adresse = ui->adresse_e->text();
    QString telephone = ui->tel_e->text();
    QString email = ui->email_e->text();
    QString secteur = ui->secteur_e->text();

    // Vérifications des champs
    if (nom.isEmpty()) {
        QMessageBox::warning(this, "Erreur de saisie", "Veuillez entrer un nom valide.");
        return;
    }

    if (adresse.isEmpty()) {
        QMessageBox::warning(this, "Erreur de saisie", "Veuillez entrer une adresse valide.");
        return;
    }

    // Vérification de la validité du numéro de téléphone
    QRegularExpression regexTel("\\d{8,15}"); // Numéro de téléphone de 8 à 15 chiffres
    if (!regexTel.match(telephone).hasMatch()) {
        QMessageBox::warning(this, "Erreur de saisie", "Veuillez entrer un numéro de téléphone valide (8 à 15 chiffres).");
        return;
    }


    // Vérification de la validité de l'email
    QRegularExpression regexEmail("^\\S+@\\S+\\.\\S+$");
    QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (email.isEmpty() || !emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Erreur de saisie", "Veuillez entrer une adresse email valide.");
        return;
    }

    if (secteur.isEmpty()) {
        QMessageBox::warning(this, "Erreur de saisie", "Veuillez entrer un secteur valide.");
        return;
    }

    // Tous les champs sont valides, ajout de l'entreprise
    Entreprise e(nom, adresse, telephone, email, secteur);
    if (e.ajouter()) {
        QMessageBox::information(this, "Succès", "L'entreprise a été ajoutée avec succès.");
        // Actualisation du tableau des entreprises après l'ajout
        ui->affiche_e->setModel(e.afficher());
        afficherStatistiques();

    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout de l'entreprise.");
    }
}



void MainWindow::on_commandLinkButton_clicked()
{
    int id = ui->nv_ide->text().toInt();

    Entreprise e;

    // Vérifier si l'ID existe et récupérer les informations
    if (e.recuperer(id)) {
        ui->nv_nom->setText(e.getnom_entreprise());
        ui->nv_adresse->setText(e.getadresse());
        ui->nv_tel->setText(e.gettelephone());
        ui->nv_email->setText(e.getemail());
        ui->nv_secteur->setText(e.getsecteur());
        QMessageBox::information(this, "Succès", "Informations récupérées avec succès.");
    } else {
        QMessageBox::warning(this, "Erreur", "Aucune entreprise trouvée avec cet ID.");
    }
}




void MainWindow::on_pushButton_clicked()
{
    int id = ui->nv_ide->text().toInt();

    // Vérifier si l'ID est valide
    if (id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide.");
        return;
    }

    // Créer une instance de l'objet Entreprise
    Entreprise e;

    // Récupérer l'entreprise existante pour obtenir les valeurs actuelles
    if (!e.recuperer(id)) {
        QMessageBox::warning(this, "Erreur", "Aucune entreprise trouvée avec cet ID.");
        return;
    }

    // Vérifier chaque champ et mettre à jour uniquement les champs modifiés
    QString newNom = ui->nv_nom->text();
    QString newAdresse = ui->nv_adresse->text();
    QString newTelephone = ui->nv_tel->text();
    QString newEmail = ui->nv_email->text();
    QString newSecteur = ui->nv_secteur->text();

    // Si le champ est vide, garder l'ancienne valeur
    if (!newNom.isEmpty()) e.setnom_entreprise(newNom);
    if (!newAdresse.isEmpty()) e.setadresse(newAdresse);
    if (!newTelephone.isEmpty()) e.settelephone(newTelephone);
    if (!newEmail.isEmpty()) e.setemail(newEmail);
    if (!newSecteur.isEmpty()) e.setsecteur(newSecteur);

    // Appliquer la modification
    if (e.modifier(id)) {
        QMessageBox::information(this, "Succès", "Les informations ont été modifiées avec succès.");
        ui->affiche_e->setModel(e.afficher());
        afficherStatistiques();


    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification.");
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    int id = ui->id_sup->text().toInt();

    // Vérifier si l'ID est valide
    if (id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide.");
        return;
    }

    Entreprise e;
    if (e.supprimer(id)) {
        QMessageBox::information(this, "Succès", "L'entreprise a été supprimée avec succès.");
        ui->affiche_e->setModel(e.afficher());  // Actualiser la vue
          ui->archive_e->setModel(e.afficherArchives());
        afficherStatistiques();

    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression : Aucune entreprise trouvée avec cet ID.");
    }
}


// AFFICHER ARCHiVE

void MainWindow::afficherArchives() {
    Entreprise e;
    QSqlQueryModel* model = e.afficherArchives();

    ui->archive_e->setModel(model); // Remplacez par le nom de votre QTableView

    // Ajustez la taille des colonnes pour s'adapter à la taille de la fenêtre
    ui->archive_e->resizeColumnsToContents();
}

void MainWindow::on_tri_e_clicked()
{
    // Récupérer le critère sélectionné dans le QComboBox
    QString critere = ui->criter->currentText();  // Assurez-vous que le QComboBox contient 'id', 'nom', et 'secteur'

    // Vérifier que le critère sélectionné est valide
    if (critere.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un critère de tri.");
        return;
    }

    Entreprise e;
    QSqlQueryModel* model = e.trier(critere);  // Appeler la méthode trier avec le critère sélectionné

    // Afficher le modèle trié dans la table
    if (model) {
        ui->affiche_e->setModel(model);
    } else {
        QMessageBox::warning(this, "Erreur", "Le tri n'a pas pu être effectué.");
    }
}


void MainWindow::on_recherche_e_clicked()
{
    QString valeur = ui->rech_e->text(); // Récupération de la valeur saisie

    QSqlQueryModel* model = e.rechercher(valeur);

    // Afficher le modèle de recherche dans la table
    ui->affiche_e->setModel(model);
}

// AFFICHER STAT ---------

void MainWindow::afficherStatistiques() {
    Entreprise e;

    // Récupérer la série de statistiques (pie chart)
    QPieSeries* series = e.statistiquesParSecteur();

    // Créer un objet QChart pour afficher le graphique
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des entreprises par secteur");

    // Créer un QChartView pour intégrer le QChart
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);  // Améliorer le rendu

    // Créer une QGraphicsScene et ajouter le QChartView
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->addWidget(chartView);

    // Ajouter la scène dans le QGraphicsView (stat_e)
    ui->stat_e->setScene(scene);

    // Redimensionner la scène pour s'adapter au QGraphicsView
    chartView->resize(ui->stat_e->size());  // Ajuste la taille du chartView à celle de stat_e
}


void MainWindow::on_PDF_e_clicked()
{
    // Ouvrir une boîte de dialogue pour choisir l'emplacement du fichier PDF
    QString fichierPDF = QFileDialog::getSaveFileName(this, "Enregistrer le PDF", "", "*.pdf");

    if (!fichierPDF.isEmpty()) {
        if (!fichierPDF.endsWith(".pdf", Qt::CaseInsensitive)) {
            fichierPDF += ".pdf";  // Ajouter l'extension si elle n'est pas fournie
        }

        // Créer une instance de l'objet Entreprise
        Entreprise e;

        // Générer le PDF à l'emplacement choisi
        e.genererPDF(fichierPDF);

        // Informer l'utilisateur que le PDF a été généré avec succès
        QMessageBox::information(this, "Succès", "Le PDF a été généré avec succès.");
    } else {
        // Si l'utilisateur annule l'opération
        QMessageBox::warning(this, "Annulé", "La génération du PDF a été annulée.");
    }
}

// ----------------------- MODULE OFFRE -----------------------------------


void MainWindow::on_ajouter_e_2_clicked()

{
    QString titre = ui->titre_o->text();
    QString description = ui->des_o->toPlainText(); // Utilisez toPlainText() pour QTextEdit
    QString competences = ui->com_o->text();
    QString status = ui->status_o->currentText(); // Assurez-vous que status_o est un QComboBox
    int id_e = ui->id_eo->text().toInt(); // ID de l'entreprise
    QDate dateLimite = ui->date_limite_o->date(); // Exemple de récupération d'une date

    // Validation des champs
    if (titre.isEmpty() || description.isEmpty() || competences.isEmpty() || status.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    // Création de l'objet Offre
    Offre o(0, titre, description, competences, status, dateLimite); // Par exemple, date limite à aujourd'hui
    if (o.ajouter(id_e)) { // Ajouter avec l'ID de l'entreprise
        QMessageBox::information(this, "Succès", "L'offre a été ajoutée avec succès.");
        ui->affiche_o->setModel(o.afficher());
        afficherStatistiquesParCompetences();
        ui->his->setModel(o.afficherHistorique());

    } else {
        QMessageBox::critical(this, "Erreur", "L'ID d'entreprise fourni n'existe pas.");
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    int id = ui->id_supo->text().toInt();
    Offre o;

    if (o.supprimer(id)) {
        QMessageBox::information(this, "Succès", "L'offre a été supprimée avec succès.");
        ui->affiche_o->setModel(o.afficher());
        afficherStatistiquesParCompetences();
        ui->his->setModel(o.afficherHistorique());


    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la suppression : Aucune offre trouvée avec cet ID.");
    }
}


void MainWindow::on_commandLinkButton_2_clicked()
{
    bool ok;
    int id = ui->nv_ido->text().toInt(&ok); // Get the ID from input and check if it's a valid integer

    // Vérifier si l'ID est valide
    if (!ok || id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide.");
        return;
    }

    Offre o;

    // Vérifiez si l'offre existe avant de récupérer les informations
    if (!o.recuperer(id)) {
        QMessageBox::warning(this, "Erreur", "Aucune offre trouvée avec cet ID.");
        return;
    }

    // Récupérer les informations de l'offre
    ui->nv_titre->setText(o.getTitre());
    ui->nv_des->setText(o.getDescription());
    ui->nv_com->setText(o.getCompetences());
    ui->nv_st->setCurrentText(o.getStatus());
    ui->nv_ideo->setText(QString::number(o.getIdEntreprise())); // ID d'entreprise
    QMessageBox::information(this, "Succès", "Informations récupérées avec succès.");
}


void MainWindow::on_pushButton_4_clicked()
{
    bool ok;
    int id = ui->nv_ido->text().toInt(&ok); // Get the ID from input and check if it's a valid integer

    // Vérifier si l'ID est valide
    if (!ok || id <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID valide.");
        return;
    }

    Offre o;

    // Récupérer l'offre existante pour obtenir les valeurs actuelles
    if (!o.recuperer(id)) {
        QMessageBox::warning(this, "Erreur", "Aucune offre trouvée avec cet ID.");
        return;
    }

    // Récupérer les valeurs des champs
    QString newTitre = ui->nv_titre->text();
    QString newDescription = ui->nv_des->toPlainText();
    QString newCompetences = ui->nv_com->text();
    QString newStatus = ui->nv_st->currentText();
    int newIdEntreprise = ui->nv_ideo->text().toInt(&ok); // ID d'entreprise saisi

    // Vérification de l'ID d'entreprise
    if (!ok || newIdEntreprise <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID d'entreprise valide.");
        return;
    }

    // Mettre à jour les valeurs uniquement si elles sont modifiées
    if (!newTitre.isEmpty()) o.setTitre(newTitre);
    if (!newDescription.isEmpty()) o.setDescription(newDescription);
    if (!newCompetences.isEmpty()) o.setCompetences(newCompetences);
    o.setStatus(newStatus); // Toujours mettre à jour le statut
    o.setIdEntreprise(newIdEntreprise); // Toujours mettre à jour l'ID d'entreprise
    // Appliquer la modification
    if (o.modifier(id)) {
        QMessageBox::information(this, "Succès", "Les informations ont été modifiées avec succès.");
        ui->affiche_o->setModel(o.afficher());
        afficherStatistiquesParCompetences();
        ui->his->setModel(o.afficherHistorique());


    } else {
        QMessageBox::critical(this, "Erreur", "Échec de la modification : Vérifiez l'ID de l'offre ou de l'entreprise.");
    }
}


void MainWindow::on_tri_e_2_clicked()
{
    // Récupérer le critère sélectionné dans le QComboBox
    QString critere = ui->criter_2->currentText();  // Assurez-vous que le QComboBox contient 'id', 'nom', et 'secteur'

    // Vérifier que le critère sélectionné est valide
    if (critere.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un critère de tri.");
        return;
    }

    Offre o;
    QSqlQueryModel* model = o.trier(critere);  // Appeler la méthode trier avec le critère sélectionné

    // Afficher le modèle trié dans la table
    if (model) {
        ui->affiche_o->setModel(model);
    } else {
        QMessageBox::warning(this, "Erreur", "Le tri n'a pas pu être effectué.");
    }
}


void MainWindow::on_recherche_e_2_clicked()
{
    QString valeur = ui->rech_o->text(); // Récupération de la valeur saisie

    QSqlQueryModel* model = o.rechercher(valeur);

    // Afficher le modèle de recherche dans la table
    ui->affiche_o->setModel(model);
}


void MainWindow::on_PDF_e_2_clicked()
{
    int idEntreprise = ui->pdf_o->text().toInt();

    if (idEntreprise <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID d'entreprise valide.");
        return;
    }

    // Ouvrir une boîte de dialogue pour choisir l'emplacement du fichier PDF
    QString fichierPDF = QFileDialog::getSaveFileName(this, "Enregistrer le PDF", "", "*.pdf");

    if (!fichierPDF.isEmpty()) {
        if (!fichierPDF.endsWith(".pdf", Qt::CaseInsensitive)) {
            fichierPDF += ".pdf";  // Ajouter l'extension si elle n'est pas fournie
        }

        // Générer le PDF des offres pour cette entreprise
        Offre o;
        o.genererPDFOffresEntreprise(idEntreprise, fichierPDF);

        QMessageBox::information(this, "Succès", "Le PDF a été généré avec succès.");
    } else {
        // Si l'utilisateur annule l'opération
        QMessageBox::warning(this, "Annulé", "La génération du PDF a été annulée.");
    }
}

void MainWindow::afficherStatistiquesParCompetences() {
    Offre o;
    // Récupérer la série de statistiques (pie chart)
    QPieSeries* series = o.statistiquesParCompetences();
    if (!series) {
        QMessageBox::warning(this, "Erreur", "Impossible de récupérer les statistiques.");
        return;
    }

    // Créer un objet QChart pour afficher le graphique
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des compétences");

    // Créer un QChartView pour intégrer le QChart
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing); // Améliorer le rendu

    // Créer une QGraphicsScene et ajouter le QChartView
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->addWidget(chartView);

    // Ajouter la scène dans le QGraphicsView (stat_o)
    ui->stat_o->setScene(scene);
    ui->stat_o->show(); // Afficher le graphique

    chartView->resize(ui->stat_o->size());  // Ajuste la taille du chartView à celle de stat_e

}

void MainWindow::on_pushButton_5_clicked()
{
    // Récupérer les informations de l'offre à partager
    QString title = "Titre de l'offre"; // Remplacez par le titre réel de l'offre
    QString description = "Description de l'offre"; // Remplacez par la description réelle de l'offre
    QString link = "http://exemple.com/offre/123"; // Remplacez par le lien réel de l'offre

    // Créer l'URL de partage LinkedIn
    QString linkedinUrl = QString("https://www.linkedin.com/shareArticle?mini=true&url=%1&title=%2&summary=%3&source=%4")
                              .arg(QUrl::toPercentEncoding(link))  // Encodez l'URL
                              .arg(QUrl::toPercentEncoding(title))  // Encodez le titre
                              .arg(QUrl::toPercentEncoding(description))  // Encodez la description
                              .arg("Votre Site Web"); // Remplacez par le nom de votre site

    // Ouvrir le lien dans le navigateur par défaut
    QDesktopServices::openUrl(QUrl(linkedinUrl));
}


