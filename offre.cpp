#include "offre.h"

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
#include <QStringList>



#include <utility> // pour std::move

Offre::Offre() : id_o(0), id_e(0), titre(""), description(""), competences(""), status(""), date_limite(QDate()) {}

Offre::Offre(int id, QString titre, QString description, QString competences, QString status, QDate date_limite)
    : id_o(id), titre(titre), description(description), competences(competences), status(status), date_limite(date_limite) {}

int Offre::getId() const { return id_o; }
int Offre::getIdEntreprise() const { return id_e; }
QString Offre::getTitre() const { return titre; }
QString Offre::getDescription() const { return description; }
QString Offre::getCompetences() const { return competences; }
QString Offre::getStatus() const { return status; }
QDate Offre::getDateLimite() const { return date_limite; }


void Offre::setId(int id) { id_o = id; }
void Offre::setIdEntreprise(int id) { id_e = id; }
void Offre::setTitre(QString titre) { this->titre = titre; }
void Offre::setDescription(QString description) { this->description = description; }
void Offre::setCompetences(QString competences) { this->competences = competences; }
void Offre::setStatus(QString status) { this->status = status; }
void Offre::setDateLimite(QDate date_limite) { this->date_limite = date_limite; }

bool Offre::ajouter(int id_e) {
    QSqlQuery query;

    // Vérification de l'existence de l'ID d'entreprise
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM entreprise WHERE id = :id");
    checkQuery.bindValue(":id", id_e);
    checkQuery.exec();

    if (checkQuery.next() && checkQuery.value(0).toInt() == 0) {
        qDebug() << "L'ID d'entreprise fourni n'existe pas.";
        return false; // L'entreprise n'existe pas, retournez false
    }

    // Préparation de la requête d'insertion
    query.prepare("INSERT INTO offre (id, titre, description, competences, status, id_entreprise, date_limite) "
                  "VALUES (seq_offre_id.NEXTVAL, :titre, :description, :competences, :status, :id_entreprise, :date_limite)");

    query.bindValue(":titre", titre);
    query.bindValue(":description", description);
    query.bindValue(":competences", competences);
    query.bindValue(":status", status);
    query.bindValue(":id_entreprise", id_e); // Lier l'ID d'entreprise
    query.bindValue(":date_limite", date_limite); // Lier la date limite

    // Exécution de la requête
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'ajout de l'offre : " << query.lastError().text();
        return false; // Retourne false en cas d'erreur
    }

    // Optionnel : Afficher un message de succès dans le débogage
    qDebug() << "L'offre a été ajoutée avec succès.";

    // Récupération de l'ID inséré
    QSqlQuery lastIdQuery;
    lastIdQuery.prepare("SELECT seq_offre_id.CURRVAL FROM dual");
    lastIdQuery.exec();
    if (lastIdQuery.next()) {
        int offreId = lastIdQuery.value(0).toInt();
        // Enregistrer l'historique de l'ajout
        enregistrerHistorique(offreId, "ajout", "Ajout d'une nouvelle offre : " + titre);
    }

    return true; // Retourne true si l'insertion réussit
}




// Afficher toutes les offres
QSqlQueryModel* Offre::afficher() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM offre");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Compétences"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Statut"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("ID Entreprise"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Date Limite"));
    return model;
}

// ------------- SUPPRIMER ---------------------
bool Offre::supprimer(int id) {
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM offre WHERE id = :id");
    checkQuery.bindValue(":id", id);

    // Exécuter la requête de vérification
    if (!checkQuery.exec()) {
        qDebug() << "Erreur lors de la vérification de l'ID : " << checkQuery.lastError().text();
        return false; // Erreur lors de l'exécution de la requête
    }

    // Vérifiez si l'ID existe
    if (!checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        qDebug() << "L'ID fourni n'existe pas.";
        return false; // Retourner false si l'ID n'existe pas
    }

    // Préparation de la requête de suppression
    QSqlQuery query;
    query.prepare("DELETE FROM offre WHERE id = :id");
    query.bindValue(":id", id);

    // Exécuter la requête de suppression
    if (!query.exec()) {
        qDebug() << "Erreur lors de la suppression de l'offre : " << query.lastError().text();
        return false; // Retourner false si l'exécution échoue
    }
    enregistrerHistorique(id, "suppression", "Suppression de l'offre : " + titre);

    return true; // Retourner true si la suppression réussit
}
// Récupérer une offre
bool Offre::recuperer(int id) {
    QSqlQuery query;
    query.prepare("SELECT titre, description, competences, status, id_entreprise, date_limite FROM offre WHERE id = :id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        titre = query.value(0).toString();
        description = query.value(1).toString();
        competences = query.value(2).toString();
        status = query.value(3).toString();
        id_e = query.value(4).toInt();
        date_limite = query.value(5).toDate();
        return true;
    }
    return false; // Si aucune offre avec cet ID n'est trouvée
}

// Modifier une offre
bool Offre::modifier(int id) {
    // Vérifier si l'offre existe
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM offre WHERE id = :id");
    checkQuery.bindValue(":id", id);

    if (!checkQuery.exec() || !checkQuery.next() || checkQuery.value(0).toInt() == 0) {
        qDebug() << "L'offre avec cet ID n'existe pas.";
        return false; // Retourne false si l'offre n'existe pas
    }

    // Vérification de l'existence de l'ID d'entreprise
    QSqlQuery entrepriseQuery;
    entrepriseQuery.prepare("SELECT COUNT(*) FROM entreprise WHERE id = :id");
    entrepriseQuery.bindValue(":id", id_e);

    if (!entrepriseQuery.exec() || !entrepriseQuery.next() || entrepriseQuery.value(0).toInt() == 0) {
        qDebug() << "L'ID d'entreprise fourni n'existe pas.";
        return false; // L'entreprise n'existe pas, retournez false
    }

    // Préparation de la requête de mise à jour
    QSqlQuery query;
    query.prepare("UPDATE offre SET titre = :titre, description = :description, competences = :competences, "
                  "status = :status, id_entreprise = :id_entreprise, date_limite = :date_limite WHERE id = :id");

    query.bindValue(":id", id);
    query.bindValue(":titre", titre);
    query.bindValue(":description", description);
    query.bindValue(":competences", competences);
    query.bindValue(":status", status);
    query.bindValue(":id_entreprise", id_e); // ID de l'entreprise
    query.bindValue(":date_limite", date_limite);

    enregistrerHistorique(id, "modification", "Modification de l'offre : " + titre);


    return query.exec(); // Retourne true si la mise à jour réussit
}

// TRI
QSqlQueryModel* Offre::trier(const QString& critere) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    QString queryString = "SELECT * FROM offre";

    // Vérification des critères de tri
    if (critere == "id") {
        queryString += " ORDER BY id ASC";
    } else if (critere == "titre") {
        queryString += " ORDER BY titre ASC";
    } else if (critere == "date_limite") {
        queryString += " ORDER BY date_limite ASC";
    } else {
        qDebug() << "Critère de tri invalide : " << critere;
        return nullptr;  // Retourner null si le critère est invalide
    }

    // Préparer et exécuter la requête
    query.prepare(queryString);
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête de tri : " << query.lastError().text();
        return nullptr;
    }

    // Assigner le résultat à QSqlQueryModel
    model->setQuery(query);

    // Vérification des erreurs
    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors du tri : " << model->lastError().text();
        return nullptr;
    }

    return model;

}

// RECHERCHE
QSqlQueryModel* Offre::rechercher(const QString& valeur) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    QString queryString;

    // Recherche par ID si c'est un entier positif
    if (valeur.toInt() > 0) {
        queryString = "SELECT * FROM offre WHERE id = :valeur";
        query.prepare(queryString);
        query.bindValue(":valeur", valeur.toInt());
    } else {
        // Sinon, recherche par titre ou competences
        queryString = "SELECT * FROM offre WHERE titre LIKE :valeur OR competences LIKE :valeur";
        query.prepare(queryString);
        query.bindValue(":valeur", "%" + valeur + "%");
    }

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête de recherche : " << query.lastError().text();
        return nullptr;
    }

    model->setQuery(std::move(query));

    // Vérification des erreurs
    if (model->lastError().isValid()) {
        qDebug() << "Erreur lors de la recherche : " << model->lastError().text();
        return nullptr;
    }

    return model;
}


// Génère le contenu HTML pour les offres d'une entreprise, avec son nom
// Génération du contenu HTML pour le PDF
QString Offre::genererContenuPDFOffres(int idEntreprise, QString& nomEntreprise) {
    // Vérification de l'existence de l'entreprise et récupération de son nom
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT nom FROM entreprise WHERE id = :id");
    checkQuery.bindValue(":id", idEntreprise);

    if (!checkQuery.exec() || !checkQuery.next()) {
        return "";  // L'entreprise n'existe pas
    }

    // Récupérer le nom de l'entreprise
    nomEntreprise = checkQuery.value(0).toString();

    // Récupérer les offres associées à cette entreprise
    QSqlQuery query;
    query.prepare("SELECT * FROM offre WHERE id_entreprise = :id");
    query.bindValue(":id", idEntreprise);
    query.exec();

    // Si aucune offre n'existe pour cette entreprise
    if (!query.next()) {
        return "Aucune offre trouvée pour l'entreprise : " + nomEntreprise;
    }

    // Génération du contenu HTML pour le PDF
    QString html = QString(
                       "<html>"
                       "<head>"
                       "<style>"
                       "body { font-family: Arial, sans-serif; margin: 0; padding: 0; }"
                       "h1 { text-align: center; color: #333; margin-bottom: 20px; }"
                       "table { width: 100%; border-collapse: collapse; margin: 0; }"
                       "th, td { border: 1px solid #dddddd; padding: 15px; text-align: left; font-size: 12px; height: 50px; white-space: nowrap; }"
                       "th { background-color: #f2f2f2; font-weight: bold; }"
                       ".footer { text-align: center; font-size: 12px; color: #666; margin-top: 30px; }"
                       "</style>"
                       "</head>"
                       "<body>"
                       "<h1>Offres pour l'entreprise : %1</h1>"
                       "<table>"
                       "<tr>"
                       "<th>ID</th>"
                       "<th>Titre</th>"
                       "<th>Description</th>"
                       "<th>Compétences</th>"
                       "<th>Status</th>"
                       "<th>Date Limite</th>"
                       "</tr>"
                       ).arg(nomEntreprise);

    // Ajouter chaque offre dans le tableau
    query.first(); // Positionner sur le premier enregistrement
    do {
        int id = query.value(0).toInt();
        QString titre = query.value(1).toString();
        QString description = query.value(2).toString();
        QString competences = query.value(3).toString();
        QString status = query.value(4).toString();

        // Récupérer la date limite et vérifier si elle est valide
        QVariant dateLimiteVar = query.value(5); // Récupérer la date en tant que QVariant
        QDate dateLimite = dateLimiteVar.toDate();

        // Vérifier si la date est valide
        if (dateLimite.isValid()) {
            html += QString(
                        "<tr>"
                        "<td>%1</td>"
                        "<td>%2</td>"
                        "<td>%3</td>"
                        "<td>%4</td>"
                        "<td>%5</td>"
                        "<td>%6</td>"
                        "</tr>"
                        ).arg(id).arg(titre).arg(description).arg(competences).arg(status).arg(dateLimite.toString("dd/MM/yyyy"));
        } else {
            qDebug() << "La date limite est invalide ou vide pour l'offre ID :" << id;
            html += QString(
                        "<tr>"
                        "<td>%1</td>"
                        "<td>%2</td>"
                        "<td>%3</td>"
                        "<td>%4</td>"
                        "<td>%5</td>"
                        "<td>Non spécifiée</td>"
                        "</tr>"
                        ).arg(id).arg(titre).arg(description).arg(competences).arg(status);
        }
    } while (query.next());

    html += QString(
                "</table>"
                "<div class='footer'>"
                "<p>Ce rapport a été généré automatiquement le %1.</p>"
                "</div>"
                "</body>"
                "</html>"
                ).arg(QDate::currentDate().toString("dd/MM/yyyy"));

    return html;
}


// Générer le PDF avec le nom de l'entreprise
void Offre::genererPDFOffresEntreprise(int idEntreprise, const QString& fichierPDF) {
    QString nomEntreprise;
    QString contenuHTML = genererContenuPDFOffres(idEntreprise, nomEntreprise);

    // Si le contenu est vide, l'entreprise n'existe pas ou il n'y a pas d'offres
    if (contenuHTML.isEmpty()) {
        QMessageBox::warning(nullptr, "Erreur", "Aucune offre trouvée ou l'ID d'entreprise n'existe pas.");
        return;
    }

    // Générer le PDF
    QTextDocument document;
    document.setHtml(contenuHTML);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fichierPDF);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    document.print(&printer);
}

// Méthode pour générer des statistiques par compétences
QPieSeries* Offre::statistiquesParCompetences() {
    QMap<QString, int> competencesMap; // Pour stocker les compétences et leur compte
    QSqlQuery query;

    // Récupérer toutes les offres
    query.prepare("SELECT competences FROM offre");
    if (!query.exec()) {
        qDebug() << "Erreur lors de la récupération des compétences : " << query.lastError().text();
        return nullptr; // Retourner null en cas d'erreur
    }

    // Parcourir toutes les offres
    while (query.next()) {
        QString competencesStr = query.value(0).toString();
        // Séparer les compétences par une virgule ou un espace
        QStringList competencesList = competencesStr.split(QRegularExpression("[,\\s]+"), Qt::SkipEmptyParts);
        // Compter chaque compétence
        for (const QString& competence : competencesList) {
            QString trimmedCompetence = competence.trimmed(); // Enlever les espaces superflus
            if (!trimmedCompetence.isEmpty()) {
                competencesMap[trimmedCompetence]++; // Incrémente le compte pour cette compétence
            }
        }
    }

    // Créer un QPieSeries pour stocker les compétences
    QPieSeries* series = new QPieSeries();
    for (auto it = competencesMap.constBegin(); it != competencesMap.constEnd(); ++it) {
        series->append(it.key(), it.value()); // Ajouter chaque compétence et son compte au graphique
    }

    return series; // Retourner le QPieSeries
}


void Offre::enregistrerHistorique(int offreId, const QString &action, const QString &details) {
    QSqlQuery query;
    query.prepare("INSERT INTO historique_offre (offre_id, action, details) VALUES (:offre_id, :action, :details)");
    query.bindValue(":offre_id", offreId);
    query.bindValue(":action", action);
    query.bindValue(":details", details);

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'enregistrement de l'historique : " << query.lastError().text();
    }
}


QSqlQueryModel* Offre::afficherHistorique() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM historique_offre ORDER BY timestamp DESC");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("ID Offre"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Action"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Timestamp"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Détails"));

    return model;
}


