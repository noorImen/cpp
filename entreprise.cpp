#include "entreprise.h"
#include <QSqlQuery>

#include <QtDebug>
#include <QSqlError>
#include<QSqlQueryModel>
#include<QObject>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QPdfWriter>
#include <QPainter>
#include <QPrinter>
#include <QFile>
#include <QTextDocument>
#include <QDate>
#include <QRegularExpression>

#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QPieSeries>
#include <utility> // pour std::move



Entreprise::Entreprise() {
    nom_entreprise="";adresse="";telephone="";email="";secteur="";
}

Entreprise::Entreprise(QString nom_entreprise,QString adresse,QString telephone, QString email, QString secteur)
{
    this -> nom_entreprise=nom_entreprise; this -> telephone=telephone;
    this ->email=email; this ->adresse=adresse; this ->secteur=secteur;
}

int Entreprise::getentreprise_id(){return entreprise_id;}
QString Entreprise::getnom_entreprise(){return nom_entreprise;}
QString Entreprise::getadresse(){return adresse;}
QString Entreprise::gettelephone(){return telephone;}
QString Entreprise::getemail(){return email;}
QString Entreprise::getsecteur(){return secteur;}

void Entreprise::setentreprise_id(int entreprise_id){this ->entreprise_id=entreprise_id;}
void Entreprise::setnom_entreprise(QString nom_entreprise){this ->nom_entreprise=nom_entreprise;}
void Entreprise::setadresse(QString adresse){this ->adresse=adresse;}
void Entreprise::settelephone(QString telephone){this ->telephone=telephone;}
void Entreprise::setemail(QString email){this ->email=email;}
void Entreprise::setsecteur(QString secteur){this ->secteur=secteur;}



bool Entreprise::ajouter() {
    QSqlQuery query;

    // Préparation de la requête d'insertion
    query.prepare("INSERT INTO entreprise (id, nom, secteur, adresse, email, telephone) "
                  "VALUES (seq_entreprise_id.NEXTVAL, :nom, :secteur, :adresse, :email, :telephone)");
    query.bindValue(":nom", nom_entreprise);
    query.bindValue(":secteur", secteur);
    query.bindValue(":adresse", adresse);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);

    // Exécuter la requête d'insertion
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'ajout de l'entreprise :" << query.lastError().text();
        return false;
    }

    qDebug() << "Entreprise ajoutée avec succès";
    return true;
}



// Méthode afficher sans décryptage
QSqlQueryModel* Entreprise::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM entreprise");

    // Aucun besoin de décryptage, les emails sont stockés et affichés directement
    for (int i = 0; i < model->rowCount(); ++i) {
        QString email = model->data(model->index(i, 4)).toString(); // Récupérer l'email
        qDebug() << "Email récupéré depuis la base de données :" << email;

        // Aucun traitement supplémentaire, afficher directement les emails
        model->setData(model->index(i, 4), email);
    }

    return model;
}
// RECUPERER

bool Entreprise::recuperer(int id) {
    QSqlQuery query;
    query.prepare("SELECT nom, adresse, telephone, email, secteur FROM entreprise WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        nom_entreprise = query.value(0).toString();
        adresse = query.value(1).toString();
        telephone = query.value(2).toString();
        email = query.value(3).toString();
        secteur = query.value(4).toString();
        return true;
    } else {
        return false;  // Si aucune entreprise avec cet ID n'est trouvée
    }
}

// MODIFIER

bool Entreprise::modifier(int id) {
    QSqlQuery query;
    query.prepare("UPDATE entreprise SET nom = :nom, adresse = :adresse, telephone = :telephone, "
                  "email = :email, secteur = :secteur WHERE id = :id");
    query.bindValue(":id", id);
    query.bindValue(":nom", nom_entreprise);
    query.bindValue(":adresse", adresse);
    query.bindValue(":telephone", telephone);
    query.bindValue(":email", email);
    query.bindValue(":secteur", secteur);

    // Exécuter la requête et vérifier si elle réussit
    if (!query.exec()) {
        qDebug() << "Erreur lors de la mise à jour de l'entreprise : " << query.lastError().text();
        return false; // Retourner false si la requête échoue
    }
    return true; // Retourner true si la requête réussit
}

// SUPPRIMER
bool Entreprise::supprimer(int id) {
    // Vérifier d'abord si l'entreprise existe
    if (!recuperer(id)) {  // Utiliser la méthode recuperer pour vérifier l'existence
        return false;  // Si l'entreprise n'existe pas, retourner false
    }

    QSqlQuery query;
    query.prepare("DELETE FROM entreprise WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la suppression de l'entreprise : " << query.lastError().text();
        return false; // Retourner false si la requête échoue
    }
    return true; // Retourner true si la suppression réussit
}

// AFFICHER     ARCHIVE

QSqlQueryModel* Entreprise::afficherArchives() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM entreprise_archive");

    // Vérifiez s'il y a des erreurs
    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors de la récupération des archives : " << model->lastError().text();
    }

    return model;
}

// TRi -----
QSqlQueryModel* Entreprise::trier(const QString& critere) {
    QString queryString = "SELECT * FROM entreprise";

    // Vérifier que le critère est l'un des champs valides
    if (critere == "id") {
        queryString += " ORDER BY id ASC";
    } else if (critere == "nom") {
        queryString += " ORDER BY nom ASC";  // Assurez-vous que c'est le nom de colonne correct
    } else if (critere == "secteur") {
        queryString += " ORDER BY secteur ASC";
    } else {
        qDebug() << "Critère de tri invalide : " << critere;
        return nullptr;  // Retourner un modèle nul si le critère est invalide
    }

    // Créer et exécuter le modèle de requête
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(queryString);

    // Vérification des erreurs
    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors du tri : " << model->lastError().text();
    }

    return model;
}
// RECHERCHE ------------
QSqlQueryModel* Entreprise::rechercher(const QString& valeur) {
    QSqlQuery query;
    QString queryString;

    if (valeur.toInt() > 0) {
        // Recherche par ID (si la valeur est un entier positif)
        queryString = "SELECT * FROM entreprise WHERE id = :valeur";
    } else {
        // Recherche par nom ou secteur
        queryString = "SELECT * FROM entreprise "
                      "WHERE nom LIKE :valeurLike "
                      "OR secteur LIKE :valeurLike";
    }

    query.prepare(queryString);

    if (valeur.toInt() > 0) {
        query.bindValue(":valeur", valeur);
    } else {
        query.bindValue(":valeurLike", "%" + valeur + "%");
    }

    qDebug() << "Executing query:" << queryString;
    qDebug() << "Binding values - " << (valeur.toInt() > 0 ? "exact:" : "LIKE pattern:") << valeur;

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
    }

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(std::move(query));

    return model;
}

// STATISTIQUE ---------------------------

QPieSeries* Entreprise::statistiquesParSecteur() {
    QPieSeries* series = new QPieSeries();

    // Requête SQL pour compter les entreprises par secteur
    QSqlQuery query;
    query.prepare("SELECT secteur, COUNT(*) FROM entreprise GROUP BY secteur");

    if (query.exec()) {
        while (query.next()) {
            QString secteur = query.value(0).toString();
            int count = query.value(1).toInt();
            series->append(secteur, count);
        }
    } else {
        qDebug() << "Erreur lors de la récupération des statistiques par secteur : " << query.lastError().text();
    }

    return series;
}

// genérer modele html pour pdf
QString Entreprise::genererContenuPDF() {
    // Définir un modèle HTML avec du CSS pour la mise en page
    QString html = R"(
    <html>
    <head>
        <style>
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 0;
            }
            h1 {
                text-align: center;
                color: #333;
                margin-bottom: 20px;
            }
            table {
                width: 100%; /* Prendre toute la largeur disponible */
                border-collapse: collapse;
                margin: 0;
            }
            th, td {
                border: 1px solid #dddddd;
                padding: 15px; /* Augmentation du padding pour plus d'espace */
                text-align: left;
                font-size: 12px; /* Taille de la police pour les en-têtes */
                height: 60px; /* Hauteur des cellules augmentée */
                white-space: nowrap; /* Empêcher le retour à la ligne */
            }
            th {
                background-color: #f2f2f2;
                font-weight: bold;
                font-size: 10px; /* Taille de la police des en-têtes */
            }
            td {
                font-size: 8px; /* Taille de la police pour les données */
            }
            .footer {
                text-align: center;
                font-size: 12px;
                color: #666;
                margin-top: 30px;
            }
        </style>
    </head>
    <body>
        <h1>Rapport des Entreprises</h1>
        <table>
            <tr>
                <th>ID</th>
                <th>Nom</th>
                <th>Secteur</th>
                <th>Adresse</th>
                <th>Email</th>
                <th>Telephone</th>
            </tr>
    )";

    // Ajouter les entreprises à la table dans la boucle
    QSqlQuery query("SELECT * FROM entreprise");
    while (query.next()) {
        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString adresse = query.value(2).toString();
        QString telephone = query.value(3).toString();
        QString email = query.value(4).toString();
        QString secteur = query.value(5).toString();

        html += QString(
                    "<tr>"
                    "<td>%1</td>"
                    "<td>%2</td>"
                    "<td>%3</td>"
                    "<td>%4</td>"
                    "<td>%5</td>"
                    "<td>%6</td>"
                    "</tr>"
                    ).arg(id).arg(nom).arg(adresse).arg(telephone).arg(email).arg(secteur);
    }

    html += QString(R"(
        </table>
        <div class='footer'>
            <p>Ce rapport a été généré automatiquement le %1.</p>
        </div>
    </body>
    </html>
    )").arg(QDate::currentDate().toString("dd/MM/yyyy"));

    return html;
}

// gerer le pdf
void Entreprise::genererPDF(const QString& fichierPDF) {
    QString contenuHTML = genererContenuPDF();  // Récupérer le contenu HTML généré

    QTextDocument document;
    document.setHtml(contenuHTML);

    // Configurer le QPrinter pour créer un PDF
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fichierPDF);

    // Définir les marges de la page
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    // Imprimer le document dans le fichier PDF
    document.print(&printer);
}



