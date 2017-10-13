#ifndef KORREKTSWIMPL_H
#define KORREKTSWIMPL_H
//
#include <QDialog>
#include "ui_KorrektSw.h"
#include "berechnungen.h"
//


class KorrektSwImpl : public QDialog, public Ui::KorrektSw
{
Q_OBJECT
private:
	void LeseKonfig();
	double FaktorBrixPlato;
	double Gaerungskorrektur;
	QString FormelBrixPlato;
	double SWAnstellen;
public:
	void setFaktorBrixPlato(double value);
	double getFaktorBrixPlato();
	void setSWAnstellen(double value);
	double getSWAnstellen();
	void setRefraktometerInaktiv(bool value);
	bool RefraktometerInaktiv;
	void BerDichte();
	bool abgebrochen;
	double getSw();
	KorrektSwImpl( QWidget * parent = 0,  Qt::WindowFlags f = 0 );
private slots:
	void MyReject();
	void MyAccept();
	void slot_SwPlatoChanged(double value);
	void slot_SwDichteChanged(double value);
	void slot_SwBrixChanged(double value);
    void on_doubleSpinBox_B_valueChanged(double arg1);
    //void on_doubleSpinBox_valueChanged(double arg1);

    void on_spinBox_SwBrix_valueChanged(double arg1);
    //void on_doubleSpinBox_valueChanged(double arg1);
    //void on_doubleSpinBox_2(double value);


};
#endif





