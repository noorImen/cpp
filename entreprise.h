#ifndef ENTREPRISE_H
#define ENTREPRISE_H
#include <QString>
#include<QSqlQueryModel>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSlice>
#include <QPieSeries>



class Entreprise
{
public:
    Entreprise();
    Entreprise(QString,QString,QString,QString,QString);

    int getentreprise_id();
    QString getnom_entreprise();
    QString getadresse();
    QString gettelephone();
    QString getemail();
    QString getsecteur();

    void setentreprise_id(int);
    void setnom_entreprise(QString);
    void setadresse(QString);
    void settelephone(QString);
    void setemail(QString);
    void setsecteur(QString);


    // Crud ---------------------
    bool ajouter();
    QSqlQueryModel* afficher();
    bool supprimer(int id);

    // les deux
    bool recuperer(int id);
    bool modifier(int id);
    // ------------------
// ------ MéTIER --------------

    QSqlQueryModel* afficherArchives();
    QSqlQueryModel* trier(const QString& critere);
    QSqlQueryModel* rechercher(const QString& )    ;

    QPieSeries* statistiquesParSecteur();

    QString genererContenuPDF();

    void genererPDF(const QString& fichierPDF);








    //---------------------------





private:
    int entreprise_id;
    QString nom_entreprise, adresse, telephone, email, secteur;
};

#endif // ENTREPRISE_H
