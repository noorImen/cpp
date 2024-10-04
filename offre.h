#ifndef OFFRE_H
#define OFFRE_H

#include <QString>
#include <QSqlQueryModel>
#include <QDate>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSlice>
#include <QPieSeries>
#include <QDateTime>
#include <QList>


class Offre {
public:
    Offre();
    Offre(int id, QString titre, QString description, QString competences, QString status, QDate date_limite);

    // Getters
    int getId() const;
    int getIdEntreprise() const;
    QString getTitre() const;
    QString getDescription() const;
    QString getCompetences() const;
    QString getStatus() const;
    QDate getDateLimite() const;

    // Setters
    void setId(int id);
    void setIdEntreprise(int id);
    void setTitre(QString titre);
    void setDescription(QString description);
    void setCompetences(QString competences);
    void setStatus(QString status);
    void setDateLimite(QDate date_limite);

    // CRUD Methods
    bool ajouter(int id_e); // ID de l'entreprise en paramètre
    QSqlQueryModel* afficher();
    bool supprimer(int id);
    bool recuperer(int id);
    bool modifier(int id);

    // Métier Methods
    QSqlQueryModel* afficherArchives();
    QSqlQueryModel* trier(const QString& critere);
    QSqlQueryModel* rechercher(const QString& valeur);
    QPieSeries* statistiquesParCompetences();

    // Méthodes pour PDF
    QString genererContenuPDFOffres(int idEntreprise, QString& nomEntreprise);  // Génère le contenu du PDF avec le nom de l'entreprise
    void genererPDFOffresEntreprise(int idEntreprise, const QString& fichierPDF);  // Génère et enregistre le PDF

    void enregistrerHistorique(int offreId, const QString &action, const QString &details);
    QSqlQueryModel* afficherHistorique();






private:
    int id_o; // ID de l'offre
    int id_e; // ID de l'entreprise (clé étrangère)
    QString titre;
    QString description;
    QString competences;
    QString status;
    QDate date_limite;
};

#endif // OFFRE_H
